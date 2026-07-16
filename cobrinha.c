// snake.c
// descrip
// jaz 15/07/26

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <curses.h>
#include "jazlib.h"

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 20
#endif

#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT 20
#endif

#define SCREEN_FACTOR 2
#define N_INIT_FRUTOS 5

typedef struct {
	int pos_x;
	int pos_y;
	char icon;
} Objeto;

typedef struct {
	int x;
	int y;
} Direcao;

enum Colisao {
	COBRA,
	FRUTA,
	LIMITE
};

void init_curses(WINDOW *win); Objeto *inicializar_cobrinha();
Objeto *inicializar_frutos(); void definir_direcao(Direcao *dir, int key);
void desenhar_objetos(Objeto *array); void atualizar_cobrinha(Objeto *cobrinha, Direcao dir);
enum Colisao detectar_colisao(Objeto *cobrinha, Objeto *frutos);
Objeto *adicionar_fruto(Objeto *frutos); Objeto *adicionar_corpo(Objeto *cobrinha);

int main(void){
	// inicializar tela
	WINDOW *win = initscr();

	init_curses(win);

	// incializar objetos
	Objeto *cobrinha = inicializar_cobrinha();
	Objeto *frutos = inicializar_frutos();

	Direcao dir = {
		.x = 1,
		.y = 0,
	};

	int perdeu = 0;
	while (!perdeu) {
		// recebe direcao do usuario
		int pressed = wgetch(win);

		if (pressed == '\e') {
			break;
		} else {
			definir_direcao(&dir, pressed);
		}

		atualizar_cobrinha(cobrinha, dir);

		switch (detectar_colisao(cobrinha, frutos)) {
			case LIMITE:
			case COBRA:
				perdeu = 1;
				break;

			case FRUTA:
				frutos = adicionar_fruto(frutos);
				cobrinha = adicionar_corpo(cobrinha);
				break;
		}

		// desenhar na janela
		erase(); // apaga o que estava na tela

		desenhar_objetos(cobrinha);
		desenhar_objetos(frutos);

		usleep(125000);
	}

	endwin();

	printf("Score: %d\n", jaz_arr_len(cobrinha)-1);

	jaz_arr_free(cobrinha);
	jaz_arr_free(frutos);

	return 0;
}

void desenhar_objetos(Objeto *array) {
	for (size_t i = 0; i < jaz_arr_len(array); i++) {
		mvaddch(array[i].pos_y, array[i].pos_x * SCREEN_FACTOR, array[i].icon);
	}
}

void init_curses(WINDOW *win) {
	// aceita input do usuario
	keypad(win, true);
	nodelay(win, true); // nao espera enter
	curs_set(0); // esconde o cursor
}

Objeto *inicializar_cobrinha() {
	Objeto *head = NULL;

	head = adicionar_corpo(head);

	return head;
}

Objeto *inicializar_frutos() {
	Objeto *head = NULL;

	for (size_t i = 0; i < N_INIT_FRUTOS; i++) {
		head = adicionar_fruto(head);
	}

	return head;
}

void definir_direcao(Direcao *dir, int key){
	if (key == KEY_LEFT) {
		if (dir->x == 1) return;
		dir->x = -1;
		dir->y = 0;
	} else if (key == KEY_RIGHT) {
		if (dir->x == -1) return;
		dir->x = 1;
		dir->y = 0;
	} else if (key == KEY_UP) {
		if (dir->y == 1) return;
		dir->x = 0;
		dir->y = -1;
	} else if (key == KEY_DOWN) {
		if (dir->y == -1) return;
		dir->x = 0;
		dir->y = 1;
	}
}

void atualizar_cobrinha(Objeto *cobrinha, Direcao dir) {
	for (size_t i = jaz_arr_len(cobrinha); i > 0; i--) {
		cobrinha[i].pos_x = cobrinha[i-1].pos_x;
		cobrinha[i].pos_y = cobrinha[i-1].pos_y;
	}

	// atualizar posicao da cobrinha
	cobrinha[0].pos_x += dir.x;
	cobrinha[0].pos_y += dir.y;
}

enum Colisao detectar_colisao(Objeto *cobrinha, Objeto *frutos) {
	if (cobrinha[0].pos_x == -1 || cobrinha[0].pos_x == SCREEN_WIDTH+1 || cobrinha[0].pos_y == -1 || cobrinha[0].pos_y == SCREEN_HEIGHT+1) {
		return LIMITE;
	}

	for (size_t i = jaz_arr_len(cobrinha); i > 1; i--) {
		if (cobrinha[0].pos_x == cobrinha[i].pos_x && cobrinha[0].pos_y == cobrinha[i].pos_y) {
			// colidimos com a cobrinha
			return COBRA;
		}
	}

	for (size_t i = 0; i < jaz_arr_len(frutos); i++) {
		if (cobrinha[0].pos_x == frutos[i].pos_x && cobrinha[0].pos_y == frutos[i].pos_y) {
			// colidimos com um fruto
			jaz_arr_deleteat(frutos, i);
			return FRUTA;
		}
	}
}

Objeto *adicionar_fruto(Objeto *frutos) {
	Objeto fruta = {0};
	fruta.icon = '@';

	fruta.pos_x = (rand() % SCREEN_WIDTH);
	fruta.pos_y = (rand() % SCREEN_HEIGHT);

	jaz_arr_append(frutos, fruta);

	return frutos;
}

Objeto *adicionar_corpo(Objeto *cobrinha) {
	Objeto corpo = {0};

	if (cobrinha == NULL) {
		corpo.icon = 'O';
	} else {
		corpo.pos_x = cobrinha[0].pos_x;
		corpo.pos_y = cobrinha[0].pos_y;
		corpo.icon = 'o';
	}

	jaz_arr_append(cobrinha, corpo);

	return cobrinha;
}
