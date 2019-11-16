#include <stdio.h>
#include <string.h>

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

int AnaliseJogoDaVelha(char *posicao, int CasasLivres, char jogador, int *nodosVisitados, int printaJogadas) {
	int resultado;
	(*nodosVisitados)++;
	if (FechouLinha(posicao)) {
		resultado = -1;
	}	
	else if (CasasLivres == 0) {
		resultado = 0;
	} else {
		char adversario;
		int MelhorResultado = -200;
		int i;
		for (i = 0; i < 9; i++) {
			if (posicao[i] == ' ') {
				posicao[i] = jogador;
				adversario = (jogador == 'x') ? 'o' : 'x';
				int ResultadoDoAdversario = AnaliseJogoDaVelha(posicao, CasasLivres - 1, adversario, nodosVisitados, 0);				
				int ResultadoDaJogada = -ResultadoDoAdversario;
				if (printaJogadas) {
					printf("%d\t%d\n", ResultadoDaJogada, i);
				}
				if (ResultadoDaJogada > MelhorResultado) {
					MelhorResultado = ResultadoDaJogada;
				}
				posicao[i] = ' ';
			}
		}
		resultado = MelhorResultado;
	}
	return resultado;
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

/**
 * Conta os 'x' e 'o' para determinar de quem é a vez
 * Quem começa o jogo é o 'x', então se empatar a contagem o 'x' joga
 */
char quemJoga(char *posicao) {
	int i;
	int nx = 0;
	int no = 0;
	for (i = 0; i < 9; i++) {
		if (posicao[i] == 'x') {
			nx++;
		} else if (posicao[i] == 'o') {
			no++;
		}
	}
	if (nx > no) { 
		return 'o';
	} else {
		return 'x';
	}
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

int main() {
	char jogo[10];
	char jogador = 'x';
	int jogada;
	int nodosVisitados;
	int espacos;
	int printJogadas = 0;
	int resultado;
	strcpy(jogo, "         ");
	while (1) {
		printaJogo(jogo);
		nodosVisitados = 0;
		espacos = casasRestantes(jogo);
		resultado = AnaliseJogoDaVelha(jogo, espacos, jogador, &nodosVisitados, 1);
		printf("Nodos visitados: %d\n", nodosVisitados);
		printf("Jogador: '%c'\nResultado: %d\n", jogador, resultado);
		scanf(" %d", &jogada);
		jogo[jogada] = jogador;
		if (jogador == 'x') {
			jogador = 'o';
		} else {
			jogador = 'x';
		}
	}
	return 0;
}
