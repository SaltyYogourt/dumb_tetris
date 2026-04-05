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

typedef struct {
    char x;
    char y;
} Point; //a point in 2D space

typedef struct { 
    SDL_Color color;
    Point offset[4][4]; //we ONLY have four offsets including 0,0, right? 
} PieceData;

typedef struct {
    PieceData *tetromino;
    unsigned char x;
    unsigned char y;
    unsigned char tetromino_id;
    char rot;
} Player;

typedef struct {
    float gravity;
    float gravity_step;
    SDL_Window *window;
    SDL_Renderer *renderer;
    Player player;
    Uint64 last_tick;
    Uint64 deltatime;
    PieceData piece_data[7];
    unsigned char board[BOARD_HEIGHT][BOARD_WIDTH];
} GameState;

enum { T_EMPTY, T_PLAYER, T_BLOCK };

#endif
