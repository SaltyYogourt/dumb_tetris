#include "state.h"

void setNextState(GameState *gamestate, State *state){
    //TODO: include checks
    gamestate->next_state = state;
}

bool changeState(GameState *gamestate){
    bool success = true;

    if (gamestate->next_state != NULL){
        //TODO: implement checks for failed gamestate change
        gamestate->current_state->exit(gamestate);
        gamestate->next_state->enter(gamestate); 

        gamestate->current_state = gamestate->next_state;
        gamestate->next_state = NULL;
    }
    return success;
}
