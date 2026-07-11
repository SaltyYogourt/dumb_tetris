#include "menu.h"
#include "main.h"
#include "state.h"
#include "event.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keyboard.h>

Menu *current_submenu = NULL;

State menu_states[MENU_STATE_COUNT];


void init_menu_states(){
}

void rebind_key(short scancode, short key){

}

Menu *init_controls_submenu(){
    size_t menu_elements_size = CMD_COUNT * sizeof(MenuElement);
    Menu *ctrls = SDL_malloc(sizeof(Menu) + menu_elements_size);
    ctrls->current = 0;
    ctrls->up = up;
    ctrls->down = down;
    ctrls->parent = NULL;
    
    load_controls_buttons(ctrls->item);

    ctrls->item_count = CMD_COUNT-1;
    return ctrls;
}

void load_controls_buttons(MenuElement *items){

    for(int i = 0; GAME_CMD_COUNT > i; ++i){
        items[i].rtext = SDL_strdup(SDL_GetScancodeName(game_event_codes[i]));
        //items[i].click = rebind_key;
    }

    for(int i = GAME_CMD_COUNT; CMD_COUNT > i; ++i){
        items[i].rtext = SDL_strdup(SDL_GetScancodeName(menu_event_codes[i-GAME_CMD_COUNT]));
        //items[i].click = rebind_key;
    }
    items[GAME_ROT_CW].text = "Rotate Clockwise";
    items[GAME_ROT_CCW].text = "Rotate Counter-clockwise";
    items[GAME_LEFT].text = "Move Left";
    items[GAME_RIGHT].text = "Move Right";
    items[GAME_HOLD].text = "Hold";
    items[GAME_SOFTDROP].text = "Softdrop";
    items[GAME_HARDDROP].text = "Harddrop";
    items[GAME_PAUSE].text = "Pause";

    items[GAME_DEBUG_RESET_GRAVITY].text = "DEBUG: Reset Gravity";
    items[GAME_DEBUG_INCREASE_GRAVITY].text = "DEBUG: Increase Gravity";
    items[GAME_DEBUG_DECREASE_GRAVITY].text = "DEBUG: Decrease Gravity";

    items[GAME_CMD_COUNT+MENU_UP].text = "Menu Up";
    items[GAME_CMD_COUNT+MENU_DOWN].text = "Menu Down";
    items[GAME_CMD_COUNT+MENU_LEFT].text = "Menu Left";
    items[GAME_CMD_COUNT+MENU_RIGHT].text = "Menu Right";
    items[GAME_CMD_COUNT+MENU_SELECT].text = "Confirm";
    items[GAME_CMD_COUNT+MENU_BACK].text = "Back";
}

void unload_controls_buttons(MenuElement *items){
    for(int i = 0; CMD_COUNT > i; ++i){
        SDL_free(items[i].text);
    }
}

Menu *controls_submenu = NULL;

Menu controls_submenu_placeholder = {
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
          .rtext = "rtext placeholder",
          .click = enter_submenu,
          .arg = 0 },
        { .text = "PLACEHOLDER", },
        { .text = "PLACEHOLDER", },
        { .text = "PLACEHOLDER", },
        { .text = "PLACEHOLDER", },
        { .text = "PLACEHOLDER", },
    },
    .parent = NULL,
};

Menu gameover_menu = {
    .item_count = 2,
    .current = 0,
    .up = up,
    .down = down,
    .item = { 
        { .text = "Restart",
            .click = menu_game_start,
            .arg = NULL },
        { .text = "Exit",
            .click = pause_exit_to_menu,
            .arg = NULL },
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

void enter_menu(GameState *gamestate){
    controls_submenu = init_controls_submenu();
    settings_submenu.item[0].arg = controls_submenu;
}

void exit_menu(GameState *gamestate){
    //unload_controls_buttons(controls_submenu->item);
    //SDL_free(controls_submenu);
    //controls_submenu = NULL;
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
