#include <stdio.h>

int AnaliseJogoDaVelha(char *posicao, int CasasLivres, char Jogador) {

	if (CasasLivres == 0) {
		return 0;
	} else {
		int MelhorResultado = -1;   // inicia com o pior resultado possível
		int i;
		for (i = 0; i < 9; i++) {
			if (posicao[i] == ' ') {
				posicao[i] = Jogador;
				// Chama a análise do ponto de vista do adversário
				Jogador = (Jogador == 'x') ? 'o' : 'x';
				int ResultadoDoAdversario = AnaliseJogoDaVelha(posicao, CasasLivres-1, Jogador);
				int ResultadoDaJogada = -ResultadoDoAdversario;
				if (ResultadoDaJogada > MelhorResultado) {
					MelhorResultado = ResultadoDaJogada;
				}
				posicao[i] = ' ';
			}
		}
		return MelhorResultado;
	}
}

void constStrToCharArray(char *str, char *array) {

	int i;
	for (i = 0; i < 9; i++) {
		array[i] = str[i];
	}
}

int main() {

	char tabuleiro[9];
	constStrToCharArray("         ", tabuleiro);
	printf("%s\n", tabuleiro);
	printf("%d\n", AnaliseJogoDaVelha(tabuleiro, 9, 'x'));
	return 0;
}
