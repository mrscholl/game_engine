#include <stdlib.h>
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
FeaturesPosicao jogoDaVelha(char *posicao, int casasLivres, char jogador) {
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
				FeaturesPosicao featuresJogada = jogoDaVelha(posicao, casasLivres - 1, adversario);
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
	}
	return featPosicao;
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

int fimDeJogo(char *jogo, int espacos) {
    if (FechouLinha(jogo) || espacos == 0) {
       return 1;
    }
    return 0;
}

char oponente(char jogador) {
    if (jogador == 'x') {
        return 'o';
    }
    return 'x';
}

/**
 * Recebe a entrada do usuário
 */
void usuarioJoga(char *jogo, int espacos, char jogador) {
    int casa;
    scanf(" %d", &casa);
    jogo[casa - 1] = jogador;
}

/**
 * Chama a função recursiva
 */
void computadorJoga(char *jogo, int espacos, char jogador) {
    int casa   = jogoDaVelha(jogo, espacos, jogador).melhorJogada;
    jogo[casa] = jogador;
}

/**
 * Verifica se é a vez do computador ou do jogador
 * e chama a função correspondente
 */
void jogar(char *vezDeQuem, char *jogo, int espacos, char jogador) {
    if (strcmp(vezDeQuem, "computador") == 0) {
        computadorJoga(jogo, espacos, jogador);
        vezDeQuem = "usuario";
    } else {
        printf("\n");
        imprimeJogo(jogo);
        usuarioJoga(jogo, espacos, jogador);
        vezDeQuem = "computador";
    }
    espacos = espacos - 1;
    if (!fimDeJogo(jogo, espacos)) {
        jogar(vezDeQuem, jogo, espacos, oponente(jogador));
    } else {
        printf("\n");
        imprimeJogo(jogo);
    }
}

char *novoJogo() {
    char *jogo = (char *) malloc(10 * sizeof(char));
    strcpy(jogo, "         ");
    return jogo;
}

int main() {
    char *jogo;
	char opcao;
	printf("Digite 1 para jogar como X.\n");
	printf("Digite 2 para jogar como O.\n");
	printf("Digite 3 para análise.\n");
	opcao = getchar();
	switch (opcao) {
	    case '1':
	    jogo = novoJogo();
		jogar("jogador", jogo, 9, 'x');
		break;
        case '2':
		jogar("computador", novoJogo(), 9, 'x');
	}
	return 0;
}