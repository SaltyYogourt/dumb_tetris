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
    .item = { 
        { .text = "PLACEHOLDER_CTRL", },
        { .text = "PLACEHOLDER_CTRL", },
    },
    .parent = NULL,
};

Menu settings_submenu = {
    .item_count = 6,
    .current = 0,
    .up = up,
    .down = down,
    .item = {
        { .text = "Controls", 
          .click = enter_submenu,
          .arg = &controls_submenu },
        { .text = "PLACEHOLDER", },
        { .text = "PLACEHOLDER", },
        { .text = "PLACEHOLDER", },
        { .text = "PLACEHOLDER", },
        { .text = "PLACEHOLDER", },
    },
    .parent = NULL,
};

Menu pause_menu = {
    .item_count = 3,
    .current = 0,
    .up = up,
    .down = down,
    .item = { 
        { .text = "Resume",
            .click = pause_unpause,
            .arg = NULL },
        { .text = "Restart",
            .click = pause_restart,
            .arg = NULL },
        { .text = "Exit",
            .click = pause_exit_to_menu,
            .arg = NULL },
    },
    .parent = NULL,
};

Menu main_menu = {
    .item_count = 4,
    .current = 0,
    .up = up,
    .down = down,
    .item = { 
        { .text = "Start",
            .click = menu_game_start,
            .arg = NULL },
        { .text = "Online"},
        { .text = "Settings",
          .click = enter_submenu,
          .arg = &settings_submenu },
        { .text = "Exit", },
    },
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
    current_submenu = (Menu*)(((void**)(args))[0]);
    SDL_Log("%d\n", current_submenu->item_count);
    setNextState((GameState*)(((void**)(args))[1]), &menu_states[STATE_MENU_SUB]);
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
