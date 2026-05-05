#include "menu.h"

Menu create_menu(MenuElement items[], int item_len){
    Menu menu = {0}; 
    menu.item_count = item_len;
    menu.up = up;
    menu.down = down;
    return menu;
}

void up(Menu *self){
    self->current--;
    self->current %= self->item_count;
}

void down(Menu *self){
    self->current++;
    self->current %= self->item_count;
}
