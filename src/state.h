#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

#include "game.h"
#define STATES_COUNT 8

enum {
    STATE_MENU,
    STATE_GAMEPLAY,
    STATE_GAME_OVER,
    STATE_PAUSE,
};

typedef struct State {
    void (*update)(GameState *gamestate);
    void (*render)(GameState *gamestate);
    void (*input)(GameState *gamestate, SDL_Event *event);
    void (*enter)(GameState *gamestate);
    void (*exit)(GameState *gamestate);
} State;

//Ideally, we'd also have some kind of StateManager to help us manage the states.
//because I am lazy, we will stuff them all into GameState globally.
void setNextState(GameState *gamestate, State *state);
bool changeState(GameState *gamestate);

#endif
