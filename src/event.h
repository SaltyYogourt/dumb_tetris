#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include "game.h"

enum { T_MOVE_STILL, T_MOVE_RIGHT, T_MOVE_LEFT };
enum { ROT_DIR_CLOCKWISE = 1, ROT_DIR_COUNTERCLOCKWISE = -1 };

void gameplay_event(GameState *gamestate, SDL_Event *event);

#endif
