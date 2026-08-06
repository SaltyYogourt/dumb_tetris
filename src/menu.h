#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include "game.h"
typedef struct Menu Menu;
extern Menu *controls_submenu;

#define MENU_STATE_COUNT 2

enum {
    STATE_MENU_MAIN,
    STATE_MENU_SUB,
};

typedef struct {
    char *text;
    char *rtext;
    void *arg;
    void (*click)(void *args);
} MenuElement;

typedef struct Menu {
    unsigned int current;
    unsigned int item_count;
    void (*up)(Menu *self);
    void (*down)(Menu *self);
    Menu *parent;
    MenuElement item[];
} Menu;

void load_controls_buttons(MenuElement *items, unsigned int *current_button);

void up(Menu *self);
void down(Menu *self);
Menu create_menu(MenuElement items[], int item_len);
//TODO: rename these, conflicts with enter/exit state funcs...
void enter_submenu(void *args);
void exit_submenu(void *args);

void enter_menu(GameState *gamestate);
void exit_menu(GameState *gamestate);

Menu *get_gameover_menu();
Menu *get_pause_menu();
Menu *get_game_menu();
Menu *get_current_submenu();

#endif
