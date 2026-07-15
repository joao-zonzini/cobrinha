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

typedef struct {
	int pos_x;
	int pos_y;
	char icon;
} Objeto;

typedef struct {
	int x;
	int y;
} Direcao;

void init_curses(WINDOW *win); Objeto *inicializar_cobrinha();
Objeto *inicializar_frutos();

int main(void){
	// inicializar tela
	WINDOW *win = initscr();

	init_curses(win);

	// incializar objetos
	Objeto *cobrinha = inicializar_cobrinha();
	Objeto *frutos = inicializar_frutos();

	Objeto corpo = {0};
	corpo.icon = 'o';

	Direcao dir = {
		.x = 1,
		.y = 0,
	};

	while (true) {
		// recebe direcao do usuario
		int pressed = wgetch(win);

		if (pressed == KEY_LEFT) {
			if (dir.x == 1) continue;
			dir.x = -1;
			dir.y = 0;
		} else if (pressed == KEY_RIGHT) {
			if (dir.x == -1) continue;
			dir.x = 1;
			dir.y = 0;
		} else if (pressed == KEY_UP) {
			if (dir.y == 1) continue;
			dir.x = 0;
			dir.y = -1;
		} else if (pressed == KEY_DOWN) {
			if (dir.y == -1) continue;
			dir.x = 0;
			dir.y = 1;
		} else if (pressed == '\e') {
			break;
		}

		for (size_t i = jaz_arr_len(cobrinha); i > 0; i--) {
			cobrinha[i].pos_x = cobrinha[i-1].pos_x;
			cobrinha[i].pos_y = cobrinha[i-1].pos_y;
		}

		// atualizar posicao da snake
		cobrinha[0].pos_x += dir.x;
		cobrinha[0].pos_y += dir.y;

		//verificar se fruta eh comida
		for (size_t i = 0; i < jaz_arr_len(frutos); i++) {
			if (cobrinha[0].pos_x == frutos[i].pos_x && cobrinha[0].pos_y == frutos[i].pos_y) {
				frutos[i].pos_x = (rand() % SCREEN_WIDTH);
				frutos[i].pos_y = (rand() % SCREEN_HEIGHT);

				corpo.pos_x = cobrinha[0].pos_x - dir.x;
				corpo.pos_y = cobrinha[0].pos_y - dir.y;

				jaz_arr_append(cobrinha, corpo);

				break;
			}
		}

		// desenhar na janela
		erase(); // apaga o que estava na tela

		for (size_t i = 0; i < jaz_arr_len(frutos); i++) {
				mvaddch(frutos[i].pos_y, frutos[i].pos_x * 2, frutos[i].icon);
		}

		for (size_t i = 0; i < jaz_arr_len(cobrinha); i++) {
				mvaddch(cobrinha[i].pos_y, cobrinha[i].pos_x * 2, cobrinha[i].icon);
		}

		usleep(125000);
	}

	endwin();

	printf("%d\n", jaz_arr_len(cobrinha)-1);

	jaz_arr_free(cobrinha);
	jaz_arr_free(frutos);

	return 0;
}

void init_curses(WINDOW *win) {
	// aceita input do usuario
	keypad(win, true);
	nodelay(win, true); // nao espera enter
	curs_set(0); // esconde o cursor
}

Objeto *inicializar_cobrinha() {
	Objeto *head = NULL;

	Objeto cabeca = {
		.pos_x = 0,
		.pos_y = 0,
		.icon = '#',
	};

	jaz_arr_append(head, cabeca);

	return head;
}

Objeto *inicializar_frutos() {
	Objeto *head = NULL;

	Objeto fruta = {0};
	fruta.icon = '@';

	for (size_t i = 0; i < 5; i++) {
		fruta.pos_x = (rand() % SCREEN_WIDTH);
		fruta.pos_y = (rand() % SCREEN_HEIGHT);

		jaz_arr_append(head, fruta);
	}

	return head;
}
