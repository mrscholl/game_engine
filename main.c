#include <stdio.h>
#include <string.h>

/**
 * Em um jogo da velha, se não houverem erros, o resultado esperado é empate,
 * mas é preferível realizar uma jogada que possibilite mais chances de vitória caso o
 * adversário erre.
 */
typedef struct features {
	int melhorJogada;
	int resultado;
	float probDerrota;
	float probVitoria;
	float probEmpate;
}
FeaturesPosicao;

/**
 * Inverte o resultado e troca chances de vitória por derrota
 */
FeaturesPosicao inverteFeaturesPosicao(FeaturesPosicao *p) {
	float aux = p->probDerrota;
	p->probDerrota = p->probVitoria;
	p->probVitoria = aux;
	p->resultado   = -p->resultado;
}

/**
 * Verifica se houve vitória no jogo da velha
 */
int FechouLinha(char *pos) {
	int i;
	// Linhas horizontais
	for (i = 0; i < 9; i += 3) {
		if (pos[i] != ' ' && (pos[i] == pos[i+1]) && (pos[i+1] == pos[i+2])) {
			return 1;
		}
	}
	// Linhas verticais
	for (i = 0; i < 3; i++) {
		if (pos[i] != ' ' && (pos[i] == pos[i+3]) && (pos[i+3] == pos[i+6])) {
			return 1;
		}
	}
	// Diagonal decrescente
	if (pos[0] != ' ' && (pos[0] == pos[4]) && (pos[4] == pos[8])) {
		return 1;
	}
	// Diagonal crescente
	if (pos[2] != ' ' && (pos[2] == pos[4]) && (pos[4] == pos[6])) {
		return 1;
	}
	return 0;
}

/**
 * Grava a posição do jogo em um arquivo no formato que será usado pela rede neural.
 */
void gravaEmArquivo(FILE *f, char *posicao, int melhorJogada) {
	int i;
	for (i = 0; i < 9; i++) {
		if (posicao[i] == 'x') {
			fprintf(f, "1,0,0,");
		} else if (posicao[i] == 'o') {
			fprintf(f, "0,1,0,");
		} else {
			fprintf(f, "0,0,1,");
		}
	}
	fprintf(f, "c%d\n", melhorJogada);
}

/**
 * Imprime na tela a casa, o resultado e as probabilidades da jogada
 */
void imprimeJogada(int jogada, FeaturesPosicao *p) {
	if (p->resultado == 0) {	
		printf("%d\t%c\t%.2f\t%.2f\t%.2f\n", jogada, 'E', p->probVitoria, p->probEmpate, p->probDerrota);
	} else if (p->resultado == -1) {	
		printf("%d\t%c\t%.2f\t%.2f\t%.2f\n", jogada, 'D', p->probVitoria, p->probEmpate, p->probDerrota);
	} else if (p->resultado == 1) {
		printf("%d\t%c\t%.2f\t%.2f\t%.2f\n", jogada, 'V', p->probVitoria, p->probEmpate, p->probDerrota);
	}
}

/**
 * Encontra a melhor variação possível para o jogador atual
 * Que é o mesmo que a pior variação para o adversário
 */
FeaturesPosicao jogoDaVelha(char *posicao, int casasLivres, char jogador, int salvaNoArquivo, FILE *f) {
	FeaturesPosicao featPosicao;
	if (FechouLinha(posicao)) {
		// Derrota
		featPosicao.resultado   = -1;
		featPosicao.probVitoria = 0;
		featPosicao.probDerrota = 1;
		featPosicao.probEmpate  = 0;
	}	
	else if (casasLivres == 0) {
		// Empate
		featPosicao.resultado   = 0;
		featPosicao.probVitoria = 0;
		featPosicao.probDerrota = 0;
		featPosicao.probEmpate  = 1;
	} else {
		// Procura pela melhor jogada
		featPosicao.resultado    = -2;
		featPosicao.probVitoria  = 0;
		featPosicao.probDerrota  = 0;
		featPosicao.probEmpate   = 0;
		featPosicao.melhorJogada = -1;
		float probVitoria = 0;
		int i;
		for (i = 0; i < 9; i++) {
			if (posicao[i] == ' ') {
				posicao[i] = jogador;
				char adversario = (jogador == 'x') ? 'o' : 'x';
				// Chama a função do ponto de vista do adversário
				// Inverte o resultado, derrota do adversário = vitória do jogador atual
				FeaturesPosicao featuresJogada = jogoDaVelha(posicao, casasLivres - 1, adversario, salvaNoArquivo, f);
				inverteFeaturesPosicao(&featuresJogada);
				// Se a jogada é a que gera o melhor resultado até o momento, adota ela.
				// Entre jogadas com mesmo resultado adota a que gera mais chances de vitória
				if ((featuresJogada.resultado > featPosicao.resultado) || ((featuresJogada.resultado == featPosicao.resultado) && (featuresJogada.probVitoria > probVitoria))) {
					featPosicao.resultado = featuresJogada.resultado;
					featPosicao.melhorJogada = i;
					probVitoria = featuresJogada.probVitoria;
				}
				// Soma as probabilidades da jogada atual nas probabilidades do nodo
				float divisor = (float) casasLivres;
				featPosicao.probEmpate  += featuresJogada.probEmpate  / divisor;
				featPosicao.probDerrota += featuresJogada.probDerrota / divisor;
				featPosicao.probVitoria += featuresJogada.probVitoria / divisor;
				// Remove a marca da jogada para não ter que criar cópia do array
				posicao[i] = ' ';
			}
		}
		if (salvaNoArquivo && casasLivres > 3) {
			gravaEmArquivo(f, posicao, featPosicao.melhorJogada);
		}
	}
	return featPosicao;
}

/**
 * Salva arquivo no formato utilizado pelo Weka.
 */
void salvaArff(char *jogo) {
	int i;
	FILE *f = fopen("dados_jogo_da_velha.arff", "w");	
	fprintf(f, "@relation JogoDaVelha\n");
	for (i = 0; i < 9; i++) {
		fprintf(f, "@attribute casa%dx numeric\n", i);
		fprintf(f, "@attribute casa%do numeric\n", i);
		fprintf(f, "@attribute casa%dvazia numeric\n", i);
	}
	fprintf(f, "@attribute class {c0,c1,c2,c3,c4,c5,c6,c7,c8}\n");
	fprintf(f, "@data\n");
	jogoDaVelha(jogo, 9, 'x', 1, f);
	fclose(f);
}

/**
 * Imprime o jogo da velha na tela
 */
void imprimeJogo(char *jogo) {
	char c;	
	int i;
	for (i = 0; i < 9; i++) {
		if (jogo[i] == ' ') {		
			c =  '-';
		} else {
			c = jogo[i];
		}
		printf("%c ", c);
		// Quebra linha após 3 casas
		if (((i + 1) % 3) == 0) {
			printf("\n");
		}	
	}
}

int main() {
	char acao;
	char jogo[10];
	strcpy(jogo, "         ");
	printf("Digite 1 para gerar o arquivo com as posições e as jogadas corretas do jogo da velha.\n");
	printf("Digite 2 para jogar.\n");
	acao = getchar();
	if (acao == '1') {
		salvaArff(jogo);
	} else {
		char jogador = 'x';
		int casasLivres = 9;
		while (1) {
			int jogada;
			imprimeJogo(jogo);
			if (casasLivres == 0 || FechouLinha(jogo)) {
				break;
			}
			jogador = 'x';
			scanf(" %d", &jogada);
			jogo[jogada] = jogador;
			casasLivres--;
			jogador = 'o';
			jogada = jogoDaVelha(jogo, casasLivres--, jogador, 0, 0).melhorJogada;
			jogo[jogada] = jogador;
		}
	}
	return 0;
}
