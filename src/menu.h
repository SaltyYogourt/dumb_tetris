#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include "game.h"
typedef struct Menu Menu;

typedef struct {
    char *text;
    void (*click)(GameState *gamestate);
} MenuElement;

typedef struct Menu {
    unsigned int current;
    int item_count;
    void (*up)(Menu *self);
    void (*down)(Menu *self);
    MenuElement item[];
} Menu;

void up(Menu *self);
void down(Menu *self);
Menu create_menu(MenuElement items[], int item_len);

Menu *get_pause_menu();
Menu *get_game_menu();

#endif
