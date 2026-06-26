#include "event.h"
#include "game.h"
#include "main.h"
#include "menu.h"
#include "state.h"

int scancode_game_events[256] = { -1 };
int scancode_menu_events[256] = { -1 };

void default_events(){
    scancode_game_events[SDL_SCANCODE_UP] = GAME_ROT_CW;
    scancode_game_events[SDL_SCANCODE_DOWN] = GAME_ROT_CCW;
    scancode_game_events[SDL_SCANCODE_LEFT] = GAME_LEFT;
    scancode_game_events[SDL_SCANCODE_RIGHT] = GAME_RIGHT;
    scancode_game_events[SDL_SCANCODE_LSHIFT] = GAME_SOFTDROP;
    scancode_game_events[SDL_SCANCODE_SPACE] = GAME_HARDDROP;
    scancode_game_events[SDL_SCANCODE_C] = GAME_HOLD;
    scancode_game_events[SDL_SCANCODE_P] = GAME_PAUSE;


    scancode_menu_events[SDL_SCANCODE_UP] = MENU_UP;
    scancode_menu_events[SDL_SCANCODE_DOWN] = MENU_DOWN;
    scancode_menu_events[SDL_SCANCODE_LEFT] = MENU_LEFT;
    scancode_menu_events[SDL_SCANCODE_RIGHT] = MENU_RIGHT;
     
    scancode_menu_events[SDL_SCANCODE_Z] = MENU_SELECT;
    scancode_menu_events[SDL_SCANCODE_X] = MENU_BACK;
}
 

 
void gameplay_event(GameState *gamestate, SDL_Event *event){
    short scancode_ev = scancode_game_events[event->key.scancode];
    if (event->type == SDL_EVENT_KEY_DOWN){
        printf("Keycode: %s\n", SDL_GetScancodeName(event->key.scancode));
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
    short scancode_ev = scancode_menu_events[event->key.scancode];
    SDL_Log("%d\n", scancode_ev);
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
