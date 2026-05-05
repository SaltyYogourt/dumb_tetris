#include "menu.h"

MenuElement main_menu_items[2] = {
{ .text = "Start" },
{ .text = "Exit" },
};

Menu main_menu = {
    .item_count = 2,
    .current = 0,
    .up = up,
    .down = down,
    .item = { 
        { .text = "Start"},
        { .text = "Exit"},
    }
};

Menu pause_menu = {
    .item_count = 3,
    .current = 0,
    .up = up,
    .down = down,
    .item = { 
        { .text = "Resume"},
        { .text = "Restart"},
        { .text = "Exit"},
    }
};

void up(Menu *self){
    self->current--;
    self->current %= self->item_count;
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
