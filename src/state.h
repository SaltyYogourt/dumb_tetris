#ifndef DRAW_H_INCLUDED
#define DRAW_H_INCLUDED

#include "tetromino.h"
#include "game.h"

typedef struct {
    void (*update)(void);
    void (*render)(void);
    void (*input)(void);
} State;

extern State Menu;
extern State Gameplay;
extern State GameOver;
extern State Pause;

#endif
