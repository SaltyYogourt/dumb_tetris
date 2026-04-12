#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <SDL3/SDL.h>
#include "game.h"

typedef struct LevelData{
    unsigned short level;
    unsigned short gravity;
    //TODO: DAS, ARE, lock delay, etc...
} LevelData;

LevelData *get_level_up_data(short level);
void update_game(GameState *gamestate);
void game_loop(GameState *gamestate);
void movx(GameState *gamestate, int dir);
void rot(GameState *gamestate, int dir);
unsigned int get_lines(GameState *gamestate, unsigned int *lines);
unsigned char get_random_tetromino(unsigned char history[4]);
void push_history(unsigned char idx, unsigned char history[4]);
void collapse_line(GameState *gamestate, int line);
void reset_delay(GameState *gamestate);
void lock_piece(GameState *gamestate);
void hard_drop(GameState *gamestate, bool sonicdrop);
void new_tetromino(GameState *gamestate, int id, int height);
void game_pause(GameState *gamestate);
void pause_loop(GameState *gamestate);
void pause_enter(GameState *gamestate);
void pause_exit(GameState *gamestate);

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]);
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event);
SDL_AppResult SDL_AppIterate(void *appstate);
void SDL_AppQuit(void *appstate, SDL_AppResult result);
#endif
