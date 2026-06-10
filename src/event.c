#include "event.h"
#include "game.h"
#include "main.h"
#include "menu.h"
#include "state.h"

void gameplay_event(GameState *gamestate, SDL_Event *event){
    if (event->type == SDL_EVENT_KEY_DOWN){
        switch (event->key.scancode){
            case SDL_SCANCODE_D:
                movx(gamestate, T_MOVE_RIGHT);
                break;
            case SDL_SCANCODE_A:
                movx(gamestate, T_MOVE_LEFT);
                break;
            case SDL_SCANCODE_W:
                rot(gamestate, ROT_DIR_CLOCKWISE);
                break;
            case SDL_SCANCODE_S:
                rot(gamestate, ROT_DIR_COUNTERCLOCKWISE);
                break;
            case SDL_SCANCODE_C:
                hold_tetromino(gamestate);
                break;
            case SDL_SCANCODE_P:
                game_pause(gamestate);
                break;
            case SDL_SCANCODE_SPACE:
                hard_drop(gamestate, false);
                break;
            case SDL_SCANCODE_LSHIFT:
                soft_drop(gamestate, false);
                break;
#ifdef DEBUG
            case SDL_SCANCODE_Q:
                gamestate->gravity+=8/64.0f;
                break;
            case SDL_SCANCODE_E:
                gamestate->gravity-=8/64.0f;
                break;
            case SDL_SCANCODE_R:
                gamestate->player.y = 3;
                gamestate->gravity_step = 0.0f;
                break;
#endif
            default:
                break;
        }
    }
    else if(event->type == SDL_EVENT_KEY_UP){
        switch (event->key.scancode) {
            case SDL_SCANCODE_LSHIFT:
                soft_drop_release(gamestate);
                break;
            default:
                break;
        }
    }
}

void pause_event(GameState *gamestate, SDL_Event *event){
    if (event->type == SDL_EVENT_KEY_DOWN){
        Menu *pause_menu = get_pause_menu();
        switch (event->key.scancode){
            case SDL_SCANCODE_P:
                setNextState(gamestate, &gamestate->states[STATE_GAMEPLAY]);
                break;
            case SDL_SCANCODE_UP:
                pause_menu->up(pause_menu);
                break;
            case SDL_SCANCODE_DOWN:
                pause_menu->down(pause_menu);
                break;
            case SDL_SCANCODE_RETURN:
                pause_menu->item[pause_menu->current].click(gamestate);
                break;
            default:
                break;
        }
    }
}

void game_menu_event(GameState *gamestate, SDL_Event *event){
    if (event->type == SDL_EVENT_KEY_DOWN){
        Menu *game_menu = get_game_menu();
        MenuElement current = game_menu->item[game_menu->current];
        switch (event->key.scancode){
            case SDL_SCANCODE_LEFT:
                game_menu->up(game_menu);
                break;
            case SDL_SCANCODE_RIGHT:
                game_menu->down(game_menu);
                break;
            case SDL_SCANCODE_RETURN:
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
        Menu *sub_menu = get_current_submenu();
        MenuElement current = sub_menu->item[sub_menu->current];
        switch (event->key.scancode){
            case SDL_SCANCODE_UP:
                sub_menu->up(sub_menu);
                break;
            case SDL_SCANCODE_DOWN:
                sub_menu->down(sub_menu);
                break;
            case SDL_SCANCODE_BACKSPACE:
                //todo: go back
                break;
            case SDL_SCANCODE_RETURN:
                if(current.arg){
                    void *args[] = {gamestate, current.arg};
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
