#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <SDL3/SDL.h>

#define MOVE_SPEED 16
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20

#define TICKRATE 60
#define TICK (1000.0f/TICKRATE)

//FIXME: we should read these from a config. alt: rename to "INIT_* ..."
#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 480


/*
// what you're refering to as C, is in fact, GNU/C.
// this won't port to MSVC easily. use mingw?
typedef float Vector2 __attribute__ ((vector_size(sizeof(float)*2)));
*/

#define LOCK_DELAY 8*TICK

#define STARTING_Y 0

#define TETROMINO_RAND_RETRIES 4
#define TETROMINO_HISTORY_LEN 4

typedef struct State State;

typedef struct {
    char x;
    char y;
} Point; //a point in 2D space

typedef struct { 
    SDL_Color color;
    Point offset[4][4]; //we ONLY have four offsets including 0,0, right? 
} PieceData;

typedef struct {
    char x;
    char y;
    unsigned char tetromino_id;
    char rot;
    PieceData *tetromino;
} Player;

//TODO: Poor choice of name in hindsight.
typedef struct GameState {
    float gravity;
    float gravity_step;
    short level;
    int lock_time;
    Uint64 last_tick;
    Uint64 pause_tick;
    Uint64 deltatime;
    unsigned char piece_history_idx[4];
    PieceData piece_data[7];
    SDL_Window *window;
    SDL_Renderer *renderer;
    Player player;
    State *current_state;
    State *next_state;
    State *states;
    unsigned char board[BOARD_HEIGHT][BOARD_WIDTH];
} GameState;

enum { T_EMPTY, T_PLAYER, T_BLOCK };

#endif
