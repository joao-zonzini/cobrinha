// snake.c
// descrip
// jaz 15/07/26

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <curses.h>
#include <string.h>
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
void atualizar_cobrinha(Objeto *cobrinha, Direcao dir);
enum Colisao detectar_colisao(Objeto *cobrinha, Objeto *frutos);
Objeto *adicionar_fruto(Objeto *frutos, Objeto *cobrinha);
Objeto *adicionar_corpo(Objeto *cobrinha); Objeto *iniciar_limites();
void desenhar_borda(int score); void desenhar_frutos(Objeto *frutos);
void desenhar_cobrinha(Objeto *cobrinha); int esta_na_pos(Objeto *array, Direcao pos);

int main(void){
	// inicializar tela
	WINDOW *win = initscr();

	init_curses(win);

	// incializar objetos
	Objeto *cobrinha = inicializar_cobrinha();
	Objeto *frutos = inicializar_frutos();
	Objeto *limites = iniciar_limites();

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
				frutos = adicionar_fruto(frutos, cobrinha);
				cobrinha = adicionar_corpo(cobrinha);
				break;
		}

		// desenhar na janela
		erase(); // apaga o que estava na tela

		desenhar_borda(jaz_arr_len(cobrinha)-1);
		desenhar_cobrinha(cobrinha);
		desenhar_frutos(frutos);

		usleep(125000);
	}

	endwin();

	printf("Score: %d\n", jaz_arr_len(cobrinha)-1);

	jaz_arr_free(limites);
	jaz_arr_free(cobrinha);
	jaz_arr_free(frutos);

	return 0;
}

void init_curses(WINDOW *win) {
	// aceita input do usuario
	keypad(win, true);
	nodelay(win, true); // nao espera enter
	curs_set(0); // esconde o cursor

	if (has_colors() == FALSE) {
		endwin();
		fprintf(stderr, "Seu terminal nao suporta cores\n");
		exit(1);
	}

	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_WHITE, COLOR_WHITE);

	use_default_colors();
}

Objeto *inicializar_cobrinha() {
	Objeto *head = NULL;

	head = adicionar_corpo(head);

	return head;
}

Objeto *inicializar_frutos() {
	Objeto *frutos = NULL;
	Objeto fruta = {0};
	fruta.icon = '@';

	for (size_t i = 0; i < N_INIT_FRUTOS; i++) {
		fruta.pos_x = (rand() % (SCREEN_WIDTH-1)) + 1;
		fruta.pos_y = (rand() % (SCREEN_HEIGHT-1)) + 1;

		jaz_arr_append(frutos, fruta);
	}

	return frutos;
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
	if (cobrinha[0].pos_x == 0 || cobrinha[0].pos_x == SCREEN_WIDTH || cobrinha[0].pos_y == 0 || cobrinha[0].pos_y == SCREEN_HEIGHT) {
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

Objeto *adicionar_fruto(Objeto *frutos, Objeto *cobrinha) {
	Direcao temp = {0};

	do {
		temp.x = (rand() % (SCREEN_WIDTH-1)) + 1;
		temp.y = (rand() % (SCREEN_HEIGHT-1)) + 1;
	} while(esta_na_pos(frutos, temp) || esta_na_pos(cobrinha, temp));

	Objeto fruta = {0};
	fruta.icon = '@';

	fruta.pos_x = temp.x;
	fruta.pos_y = temp.y;

	jaz_arr_append(frutos, fruta);

	return frutos;
}

Objeto *adicionar_corpo(Objeto *cobrinha) {
	Objeto corpo = {0};

	if (cobrinha == NULL) {
		corpo.pos_x = 1;
		corpo.pos_y = 1;
		// corpo.icon = ACS_DIAMOND;
		corpo.icon = 'O';
	} else {
		corpo.pos_x = cobrinha[0].pos_x;
		corpo.pos_y = cobrinha[0].pos_y;
		corpo.icon = ACS_DIAMOND;
	}

	jaz_arr_append(cobrinha, corpo);

	return cobrinha;
}

Objeto *iniciar_limites() {
	Objeto *limites = NULL;

	Objeto borda = {0};
	borda.icon = '-';

	for (size_t i = 0; i < SCREEN_WIDTH; i++) {
		borda.pos_x = i;
		jaz_arr_append(limites, borda);
	}

	borda.pos_y = SCREEN_HEIGHT;

	for (size_t i = 0; i < SCREEN_WIDTH; i++) {
		borda.pos_x = i;
		jaz_arr_append(limites, borda);
	}

	borda.pos_x = 0;
	borda.icon = '|';

	for (size_t i = 0; i < SCREEN_HEIGHT; i++) {
		borda.pos_y = i;
		jaz_arr_append(limites, borda);
	}

	borda.pos_x = SCREEN_WIDTH;

	for (size_t i = 0; i <= SCREEN_HEIGHT; i++) {
		borda.pos_y = i;
		jaz_arr_append(limites, borda);
	}

	return(limites);
}

void desenhar_borda(int score) {
	char score_str[16];
	sprintf(score_str, "[ Score: %d ]", score);
	mvaddch(0, 0, ACS_ULCORNER);
	mvaddch(0, SCREEN_WIDTH * SCREEN_FACTOR, ACS_URCORNER);
	mvaddch(SCREEN_HEIGHT, 0, ACS_LLCORNER);
	mvaddch(SCREEN_HEIGHT, SCREEN_WIDTH * SCREEN_FACTOR, ACS_LRCORNER);

	for (size_t i = 1; i < SCREEN_WIDTH * SCREEN_FACTOR; i++) {
		mvaddch(0, i, ACS_HLINE);
	}

	for (size_t i = 1; i < SCREEN_WIDTH * SCREEN_FACTOR; i++) {
		mvaddch(SCREEN_HEIGHT, i, ACS_HLINE);
	}

	for (size_t i = 1; i < SCREEN_HEIGHT; i++) {
		mvaddch(i, 0, ACS_VLINE);
	}

	for (size_t i = 1; i < SCREEN_HEIGHT; i++) {
		mvaddch(i, SCREEN_WIDTH * SCREEN_FACTOR, ACS_VLINE);
	}

	mvaddstr(0, SCREEN_WIDTH - 5, score_str);
}

void desenhar_frutos(Objeto *frutos) {
	attron(COLOR_PAIR(1));

	for (size_t i = 0; i < jaz_arr_len(frutos); i++) {
		mvaddch(frutos[i].pos_y, frutos[i].pos_x * SCREEN_FACTOR, '@');
	}

	attroff(COLOR_PAIR(2));
}

void desenhar_cobrinha(Objeto *cobrinha) {
	attron(COLOR_PAIR(2));

	mvaddch(cobrinha[0].pos_y, cobrinha[0].pos_x * SCREEN_FACTOR, 'O');

	for (size_t i = 1; i < jaz_arr_len(cobrinha); i++) {
		mvaddch(cobrinha[i].pos_y, cobrinha[i].pos_x * SCREEN_FACTOR, ACS_DIAMOND);
	}

	attroff(COLOR_PAIR(2));
}

int esta_na_pos(Objeto *array, Direcao pos) {
	for (size_t i = 0; i < jaz_arr_len(array); i++) {
		if (array[i].pos_x == pos.x && array[i].pos_y == pos.y) {
			return 1;
		}
	}
	return 0;
}
