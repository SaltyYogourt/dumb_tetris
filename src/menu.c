#include "menu.h"
#include "main.h"
#include "state.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>

Menu *current_submenu = NULL;

State menu_states[MENU_STATE_COUNT];

void init_menu_states(){
}

Menu controls_submenu = {
    .item_count = 2,
    .current = 0,
    .up = up,
    .down = down,
    .item = NULL,
    .parent = NULL,
};

static MenuElement settings_items[] = {
    { .text = "Controls", 
        .click = enter_submenu,
        .arg = &controls_submenu },
    { .text = "PLACEHOLDER", },
    { .text = "PLACEHOLDER", },
    { .text = "PLACEHOLDER", },
    { .text = "PLACEHOLDER", },
    { .text = "PLACEHOLDER", },
};

Menu settings_submenu = {
    .item_count = 6,
    .current = 0,
    .up = up,
    .down = down,
    .item = settings_items,
    .parent = NULL,
};

static MenuElement gameover_items[] = { 
    { .text = "Restart",
        .click = menu_game_start,
        .arg = NULL },
    { .text = "Exit",
        .click = pause_exit_to_menu,
        .arg = NULL },
};

Menu gameover_menu = {
    .item_count = 2,
    .current = 0,
    .up = up,
    .down = down,
    .item = gameover_items,
    .parent = NULL,
};

static MenuElement pause_items[] = { 
    { .text = "Resume",
        .click = pause_unpause,
        .arg = NULL },
    { .text = "Restart",
        .click = pause_restart,
        .arg = NULL },
    { .text = "Exit",
        .click = pause_exit_to_menu,
        .arg = NULL },
};

Menu pause_menu = {
    .item_count = 3,
    .current = 0,
    .up = up,
    .down = down,
    .item = pause_items,
    .parent = NULL,
};

static MenuElement main_menu_items[] = {
    { .text = "Start",
        .click = menu_game_start,
        .arg = NULL },
    { .text = "Online"},
    { .text = "Settings",
        .click = enter_submenu,
        .arg = &settings_submenu },
    { .text = "Exit", },
};

Menu main_menu = {
    .item_count = 4,
    .current = 0,
    .up = up,
    .down = down,
    .item = main_menu_items,
    .parent = NULL,
};

void up(Menu *self){
    self->current--;
    if(self->current > self->item_count-1)
        self->current = self->item_count-1;
}

void down(Menu *self){
    self->current++;
    self->current %= self->item_count;
}

void enter_submenu(void *args){
    Menu *parent = current_submenu;
    current_submenu = (Menu*)(((void**)(args))[0]);
    if(!parent){
        parent = &main_menu;
    }
    current_submenu->parent = parent;
    current_submenu->current = 0;
    GameState *gamestate  = (GameState*)(((void**)(args))[1]);
    if(gamestate->current_state != &menu_states[STATE_MENU_SUB]){
        setNextState(gamestate, &menu_states[STATE_MENU_SUB]);
    }
}

void exit_submenu(void *args){
    if(!current_submenu->parent)
        return; //how did we get here?
    if(current_submenu->parent == &main_menu){
        setNextState((GameState*)args, &menu_states[STATE_MENU_MAIN]);
        current_submenu = NULL;
    }
    else{
        current_submenu = current_submenu->parent;
    }
}

Menu *get_gameover_menu(){
    return &gameover_menu;
}

Menu *get_pause_menu(){
    return &pause_menu;
}

Menu *get_game_menu(){
    return &main_menu;
}

Menu *get_current_submenu(){
    return current_submenu;
}
