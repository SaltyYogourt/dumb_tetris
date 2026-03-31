#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <SDL3/SDL.h>

#define MOVE_SPEED 16
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define CELL_SIZE 24

#define TICKRATE 60
#define TICK (1000.0f/TICKRATE)

#define WINDOW_WIDTH BOARD_WIDTH*CELL_SIZE
#define WINDOW_HEIGHT BOARD_HEIGHT*CELL_SIZE

#define LOCK_DELAY 8*TICK

#define STARTING_Y 0

#define TETROMINO_RAND_RETRIES 4
#define TETROMINO_HISTORY_LEN 4

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

typedef struct {
    float gravity;
    float gravity_step;
    int lock_time;
    Uint64 last_tick;
    Uint64 deltatime;
    unsigned char piece_history_idx[4];
    PieceData piece_data[7];
    SDL_Window *window;
    SDL_Renderer *renderer;
    Player player;
    unsigned char board[BOARD_HEIGHT][BOARD_WIDTH];
} GameState;

enum { T_EMPTY, T_PLAYER, T_BLOCK };

#endif
