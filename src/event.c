#include "event.h"
#include "game.h"
#include "state.h"
#include "main.h"
#include "menu.h"
#include <SDL3/SDL_scancode.h>

char event_to_rebind = -1;
char scancode_game_events[SDL_SCANCODE_COUNT];
char scancode_menu_events[SDL_SCANCODE_COUNT];

//USB HID usage tables 0x07 define 0 as no event indicated, so do we.
unsigned short game_event_codes[GAME_CMD_COUNT];
unsigned short menu_event_codes[MENU_CMD_COUNT];

#define MAP_KEY(a,m,k,ev) \
    do { \
       a[k] = ev; \
       m[ev] = k; \
    } while(0)

#define MAP_GAME_KEY(k,ev) MAP_KEY(scancode_game_events,game_event_codes,k,ev)
#define MAP_MENU_KEY(k,ev) MAP_KEY(scancode_menu_events,menu_event_codes,k,ev)

void default_events(){
    SDL_memset(scancode_game_events, -1, SDL_SCANCODE_COUNT);
    SDL_memset(scancode_menu_events, -1, SDL_SCANCODE_COUNT);

    MAP_GAME_KEY(SDL_SCANCODE_UP,GAME_ROT_CW);
    MAP_GAME_KEY(SDL_SCANCODE_DOWN,GAME_ROT_CCW);
    MAP_GAME_KEY(SDL_SCANCODE_LEFT,GAME_LEFT);
    MAP_GAME_KEY(SDL_SCANCODE_RIGHT,GAME_RIGHT);

    MAP_GAME_KEY(SDL_SCANCODE_LSHIFT,GAME_SOFTDROP);
    MAP_GAME_KEY(SDL_SCANCODE_SPACE,GAME_HARDDROP);

    MAP_GAME_KEY(SDL_SCANCODE_C,GAME_HOLD);
    MAP_GAME_KEY(SDL_SCANCODE_P,GAME_PAUSE);

#ifdef DEBUG
    MAP_GAME_KEY(SDL_SCANCODE_R,GAME_DEBUG_RESET_GRAVITY);
    MAP_GAME_KEY(SDL_SCANCODE_1,GAME_DEBUG_INCREASE_GRAVITY);
    MAP_GAME_KEY(SDL_SCANCODE_2,GAME_DEBUG_DECREASE_GRAVITY);
#endif

    MAP_MENU_KEY(SDL_SCANCODE_UP,MENU_UP);
    MAP_MENU_KEY(SDL_SCANCODE_DOWN,MENU_DOWN);
    MAP_MENU_KEY(SDL_SCANCODE_LEFT,MENU_LEFT);
    MAP_MENU_KEY(SDL_SCANCODE_RIGHT,MENU_RIGHT);
     
    MAP_MENU_KEY(SDL_SCANCODE_Z,MENU_SELECT);
    MAP_MENU_KEY(SDL_SCANCODE_X,MENU_BACK);
}
 

 
void gameplay_event(GameState *gamestate, SDL_Event *event){
    short scancode_ev = scancode_game_events[event->key.scancode];
    if (event->type == SDL_EVENT_KEY_DOWN){
        SDL_Log("Keycode: %s\n", SDL_GetScancodeName(event->key.scancode));
        switch (scancode_ev){
            case GAME_RIGHT:
                movx(gamestate, T_MOVE_RIGHT);
                break;
            case GAME_LEFT:
                movx(gamestate, T_MOVE_LEFT);
                break;
            case GAME_ROT_CW:
                rot(gamestate, ROT_DIR_CLOCKWISE);
                break;
            case GAME_ROT_CCW:
                rot(gamestate, ROT_DIR_COUNTERCLOCKWISE);
                break;
            case GAME_HOLD:
                hold_tetromino(gamestate);
                break;
            case GAME_PAUSE:
                game_pause(gamestate);
                break;
            case GAME_HARDDROP:
                hard_drop(gamestate, false);
                break;
            case GAME_SOFTDROP:
                soft_drop(gamestate, false);
                break;
#ifdef DEBUG
            case GAME_DEBUG_INCREASE_GRAVITY:
                gamestate->gravity+=8/64.0f;
                break;
            case GAME_DEBUG_DECREASE_GRAVITY:
                gamestate->gravity-=8/64.0f;
                break;
            case GAME_DEBUG_RESET_GRAVITY:
                gamestate->player.y = 3;
                gamestate->gravity_step = 0.0f;
                break;
#endif
            default:
                break;
        }
    }
    else if(event->type == SDL_EVENT_KEY_UP){
        switch (scancode_ev) {
            case GAME_SOFTDROP:
                soft_drop_release(gamestate);
                break;
            default:
                break;
        }
    }
}

//FIXME: too much overlap between menu states... maybe just pass a Menu *menu arg
//and figure shit out with a generic menu_event_handler function or something??
//cuz this is ugly.
void pause_event(GameState *gamestate, SDL_Event *event){
    short scancode_ev = scancode_menu_events[event->key.scancode];
    if (event->type == SDL_EVENT_KEY_DOWN){
        Menu *pause_menu = get_pause_menu();
        switch (scancode_ev){
            case MENU_BACK:
                 pause_unpause(gamestate);
                break;
            case MENU_UP:
                pause_menu->up(pause_menu);
                break;
            case MENU_DOWN:
                pause_menu->down(pause_menu);
                break;
            case MENU_SELECT:
                pause_menu->item[pause_menu->current].click(gamestate);
                break;
            default:
                //hacky shit so game pause button always unpause except if menu P is bound
                if(event->key.scancode == scancode_game_events[GAME_PAUSE])
                        pause_unpause(gamestate);
                break;
        }
    }
}


void gameover_event(GameState *gamestate, SDL_Event *event){
    if (event->type == SDL_EVENT_KEY_DOWN){
        short scancode_ev = scancode_menu_events[event->key.scancode];
        Menu *gameover_menu = get_gameover_menu();
        MenuElement current = gameover_menu->item[gameover_menu->current];
        switch (scancode_ev){
            case MENU_UP:
                gameover_menu->up(gameover_menu);
                break;
            case MENU_DOWN:
                gameover_menu->down(gameover_menu);
                break;
            case MENU_SELECT:
                if(current.arg){
                    void *args[] = {current.arg, gamestate};
                    current.click(args);
                }
                else{
                    current.click(gamestate);
                }
                break;
            default:
                break;
        }
    }
}

void game_menu_event(GameState *gamestate, SDL_Event *event){
    short scancode_ev = -1;
    if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP){
        scancode_ev = scancode_menu_events[event->key.scancode];
        SDL_Log("%d\n", scancode_ev);
    }
    if (event->type == SDL_EVENT_KEY_DOWN){
        Menu *game_menu = get_game_menu();
        MenuElement current = game_menu->item[game_menu->current];
        switch (scancode_ev){
            case MENU_LEFT:
                game_menu->up(game_menu);
                break;
            case MENU_RIGHT:
                game_menu->down(game_menu);
                break;
            case MENU_SELECT:
                //if there's no arg we default to gamestate.
                //this is due to the menu script never having the gamestate in scope.
                //if we could do OOP properly we could avoid
                //all this stupid shit. this is global arguments with extra steps.
                if(current.arg){
                    void *args[] = {current.arg, gamestate};
                    current.click(args);
                }
                else{
                    current.click(gamestate);
                }
                break;
            default:
                break;
        }
    }
}


void sub_menu_event(GameState *gamestate, SDL_Event *event){
    if (event->type == SDL_EVENT_KEY_DOWN){
        short scancode_ev = scancode_menu_events[event->key.scancode];
        Menu *sub_menu = get_current_submenu();
        MenuElement current = sub_menu->item[sub_menu->current];
        switch (scancode_ev){
            case MENU_UP:
                sub_menu->up(sub_menu);
                break;
            case MENU_DOWN:
                sub_menu->down(sub_menu);
                break;
            case MENU_BACK:
                exit_submenu(gamestate);
                break;
            case MENU_SELECT:
                if(current.arg){
                    void *args[] = {current.arg, gamestate};
                    current.click(args);
                }
                else
                    current.click(gamestate);
                break;
            default:
                break;
        }
    }
}

void key_rebind_event(GameState *gamestate, SDL_Event *event){
    short scancode_ev = scancode_menu_events[event->key.scancode];
    Menu *ctrl_menu = get_current_submenu();
    if (event->type == SDL_EVENT_KEY_UP){
        if(scancode_ev == MENU_SELECT){
            //lazy hack because tracking the immediate moment a key is pressed/released is additional work
            //which is outside the scope of this project.
            gamestate->input_locked = 0;
        }
    }
     if(!gamestate->input_locked && event->type == SDL_EVENT_KEY_DOWN){
        //TODO: make this work for both game & menu events properly...
        char *events;
        unsigned short *event_codes;
        if(event_to_rebind < GAME_CMD_COUNT){
            events = scancode_game_events;
            event_codes = game_event_codes;
        }
        else{
            events = scancode_menu_events;
            event_codes = menu_event_codes;
            event_to_rebind -= GAME_CMD_COUNT;
        }
        events[menu_event_codes[event_to_rebind]] = -1;
        events[event->key.scancode] = event_to_rebind;
        event_codes[event_to_rebind] = event->key.scancode;
        menu_event_codes[event_to_rebind] = event->key.scancode;
        ctrl_menu->item[ctrl_menu->current].rtext = SDL_GetScancodeName(event->key.scancode);

        SDL_Log("key: %s, ev_id: %d\n", SDL_GetScancodeName(event->key.scancode), event_to_rebind);
        event_to_rebind = -1;
        gamestate->current_state->input = sub_menu_event; //dirty dirty dirty dirty dirty
    }
}

void key_rebind(void *args){
    event_to_rebind = *(char*)(((void**)(args))[0]);
    GameState *gamestate = (GameState*)(((void**)(args))[1]);
    gamestate->input_locked = 1;
    gamestate->current_state->input = key_rebind_event;

}
