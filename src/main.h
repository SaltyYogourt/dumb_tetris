#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <SDL3/SDL.h>
#include "game.h"

enum { T_MOVE_STILL, T_MOVE_RIGHT, T_MOVE_LEFT };

void update_game(GameState *gamestate);

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]);
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event);
SDL_AppResult SDL_AppIterate(void *appstate);
void SDL_AppQuit(void *appstate, SDL_AppResult result);
#endif
