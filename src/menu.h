#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include "game.h"
typedef struct Menu Menu;

#define MENU_STATE_COUNT 2

enum {
    STATE_MENU_MAIN,
    STATE_MENU_SUB,
};

typedef struct {
    char *text;
    void *arg;
    void (*click)(void *args);
} MenuElement;

typedef struct Menu {
    unsigned int current;
    int item_count;
    void (*up)(Menu *self);
    void (*down)(Menu *self);
    Menu *parent;
    MenuElement item[];
} Menu;

void up(Menu *self);
void down(Menu *self);
Menu create_menu(MenuElement items[], int item_len);
void enter_submenu(void *args);
void exit_submenu(void *args);

Menu *get_gameover_menu();
Menu *get_pause_menu();
Menu *get_game_menu();
Menu *get_current_submenu();

#endif
