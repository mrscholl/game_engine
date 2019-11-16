#include <stdio.h>
#include <string.h>

char jogadorOposto(char j) {
	return (j == 'x') ? 'o' : 'x';
}

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

int AnaliseJogoDaVelha(char *posicao, int CasasLivres, char Jogador, int *nodosVisitados) {
	int resultado;
	(*nodosVisitados)++;
	if (FechouLinha(posicao)) {
		resultado = -1;
	}	
	else if (CasasLivres == 0) {
		resultado = 0;
	} else {
		int MelhorResultado = -200;
		int i;
		for (i = 0; i < 9; i++) {
			if (posicao[i] == ' ') {
				posicao[i] = Jogador;
				// Chama a análise do ponto de vista do adversário
				Jogador = jogadorOposto(Jogador);
				int ResultadoDoAdversario = AnaliseJogoDaVelha(posicao, CasasLivres - 1, Jogador, nodosVisitados);
				int ResultadoDaJogada = -ResultadoDoAdversario;
				if (CasasLivres == 6) {
					printf("%d\n", ResultadoDoAdversario);
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
 * Inicializa o jogo através de uma string
 */
void copiaStr(char *str, char *array) {
	int i;
	for (i = 0; i < 9; i++) {
		array[i] = str[i];
	}
}

/**
 * Conta quantas casas restantes há para iniciar a recursão
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
 * Quem começa o jogo é o 'x'
 * Conta os 'x' e 'o' para determinar de quem é a vez
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

/**
 * Verifica se foi usada uma string com 9 caracteres
 */
int valida(char *str) {
	int i = 0;
	while (str[i] != '\0') {
		i++;
	}
	if (i != 9) {
		printf("A string deve ter 9 caracteres\n");
	}
}

int main() {
	int nodosVisitados = 0;
	int resultado;
	char jogo[10];
	char l1[] = {' ', ' ', ' ', '\0'};
	char l2[] = {'o', 'x', 'x', '\0'};
	char l3[] = {' ', ' ', ' ', '\0'};
	char jogador = quemJoga(jogo);
	strcpy(jogo, "");
	strcat(jogo, l1);
	strcat(jogo, l2);
	strcat(jogo, l3);
	resultado = AnaliseJogoDaVelha(jogo, casasRestantes(jogo), jogador, &nodosVisitados);
	printf("Nodos visitados: %d\n", nodosVisitados);
	if (resultado == 0) {
		printf("Empate\n");
	} else if (resultado == -1) {
		printf("Vitória do %c\n", jogadorOposto(jogador));
	} else if (resultado == 1) {
		printf("Vitória do %c\n", jogador);
	}
	return 0;
}
