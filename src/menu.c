#include "menu.h"
#include "main.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>

Menu main_menu = {
    .item_count = 4,
    .current = 0,
    .up = up,
    .down = down,
    .item = { 
        { .text = "Start",
            .click = game_menu_start },
        { .text = "Online"},
        { .text = "Settings"},
        { .text = "Exit", },
    },
    .parent = NULL,
};

Menu settings_menu = {
    .item_count = 6,
    .current = 0,
    .up = up,
    .down = down,
    .item = {
        { .text = "Controls", },
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
            .click = pause_unpause },
        { .text = "Restart",
            .click = pause_restart },
        { .text = "Exit",
            .click = pause_exit_to_menu },
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
Menu *get_pause_menu(){
    return &pause_menu;
}

Menu *get_game_menu(){
    return &main_menu;
}
