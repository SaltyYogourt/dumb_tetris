#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <SDL3/SDL.h>

#define MOVE_SPEED 16
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define CELL_SIZE 24

#define TICKRATE 250

#define WINDOW_WIDTH BOARD_WIDTH*CELL_SIZE
#define WINDOW_HEIGHT BOARD_HEIGHT*CELL_SIZE

typedef struct {
    char x;
    char y;
} Point; //a point in 2D space

typedef struct { 
    SDL_Color color;
    Point offset[4][4]; //we ONLY have four offsets including 0,0, right? 
} PieceData;

typedef struct {
    unsigned char x;
    unsigned char y;
    unsigned char tetromino_id;
    char rot;
    char move_x;
    char rot_dir;
    char rotation;
    PieceData *tetromino;
} Player;

typedef struct {
    unsigned char board[BOARD_HEIGHT][BOARD_WIDTH];
    Uint64 last_tick;
    PieceData piece_data[7];
    Player player;
    SDL_Window *window;
    SDL_Renderer *renderer;
} GameState;

enum { T_EMPTY, T_PLAYER, T_BLOCK };

#endif
