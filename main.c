#include <stdio.h>
#include <string.h>

typedef char bool;

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
void gravaEmArquivo(FILE *f, char *posicao) {
	int i;
	for (i = 0; i < 9; i++) {
		if (posicao[i] == 'x') {
			fprintf(f, "1,0,0,");
		}
		else if (posicao[i] == 'o') {
			fprintf(f, "0,1,0,");
		}
		else {
			fprintf(f, "0,0,1,");
		}
	}
}

/**
 * Encontra a melhor variação possível para o jogador atual
 * Que é o mesmo que a pior variação para o adversário
 */
int resolveJogoDaVelha(char *posicao, int casasLivres, char jogador, FILE *f) {
	gravaEmArquivo(f, posicao);
	if (FechouLinha(posicao)) {
		// Derrota
		return -1;
	}	
	else if (casasLivres == 0) {
		// Empate
		return 0;
	} else {
		int melhorResultado = -5;
		int i;
		for (i = 0; i < 9; i++) {
			if (posicao[i] == ' ') {
				posicao[i] = jogador;
				char adversario = (jogador == 'x') ? 'o' : 'x';
				// Chama a função do ponto de vista do adversário
				// Inverte o resultado, pois a derrota dele é a minha vitória
				int resultado = -resolveJogoDaVelha(posicao, casasLivres - 1, adversario, f);
				if (resultado > melhorResultado) {
					melhorResultado = resultado;
				}
				// Remove a marca da jogada para não ter que criar cópia do array
				posicao[i] = ' ';
			}
		}
		return melhorResultado;
	}
}

/**
 * Conta quantas casas restantes há para determinar o nível inicial da recursão
 */
int casasRestantes(char *posicao) {
	int i;
	int casas = 0;
	for (i = 0; i < 9; i++) {
		if (posicao[i] == ' ') {
			casas++;
		}
	}
	return casas;
}

void printaJogo(char *jogo) {
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

void salvaEmArquivo() {
	
	
}

int main() {
	char jogo[10];
	char jogador = 'x';
	int jogada;
	int nodosVisitados = 0;
	int espacos;
	int resultado;
	bool printJogadas = 0;
	strcpy(jogo, "         ");
	FILE *f = fopen("dados_jogo_da_velha.arff", "w");
	espacos = casasRestantes(jogo);
	resolveJogoDaVelha(jogo, 9, 'x', f);
	/*while (1) {
		printaJogo(jogo);
		nodosVisitados = 0;
		espacos = casasRestantes(jogo);
		resultado = AnaliseJogoDaVelha(jogo, espacos, jogador, &nodosVisitados, 1, 0);
		printf("Nodos visitados: %d\n", nodosVisitados);
		printf("Jogador: '%c'\nResultado: %d\n", jogador, resultado);
		scanf(" %d", &jogada);
		jogo[jogada] = jogador;
		if (jogador == 'x') {
			jogador = 'o';
		} else {
			jogador = 'x';
		}
	}*/
	fclose(f);
	return 0;
}
