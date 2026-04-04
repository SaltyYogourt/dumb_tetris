#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

#include "tetromino.h"
#include "game.h"

typedef struct {
    void (*update)(GameState *gamestate);
    void (*render)(GameState *gamestate);
    void (*input)(GameState *gamestate);
    void (*enter)(GameState *gamestate);
    void (*exit)(GameState *gamestate);
} State;


#endif
