#include "event.h"
#include "game.h"
#include "main.h"

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
            case SDL_SCANCODE_P:
                gamestate->gravity=2.0f;
                break;
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
            case SDL_SCANCODE_SPACE:
                hard_drop(gamestate, false);
                break;
            default:
                break;
        }
    }
}
