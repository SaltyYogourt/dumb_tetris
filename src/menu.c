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
        { .text = "Start"},
        { .text = "Online"},
        { .text = "Settings"},
        { .text = "Exit"},
    }
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
            .click = 0},
    }
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
