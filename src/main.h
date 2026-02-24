#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

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
enum { T_I, T_O, T_T, T_J, T_L, T_S, T_Z };

enum { 
    T_BOUND_BELOW      = ( 1 << 0 ),
    T_BOUND_RIGHT      = ( 1 << 1 ), //whether we're checking if there's a block or a wall to our right, we're colliding to the right.
    T_BOUND_LEFT       = ( 1 << 2 ),
    T_BOUND_OVERLAP    = ( 1 << 3 ),
    T_BOUND_RIGHT_WALL = ( 1 << 4 ),
    T_BOUND_LEFT_WALL  = ( 1 << 5 ),
    T_BOUND_FLOOR      = ( 1 << 6 ),
    T_BOUND_CEILING    = ( 1 << 7 ),
};

enum { T_MOVE_STILL, T_MOVE_RIGHT, T_MOVE_LEFT };

void update_game(GameState *gamestate);
void draw_board(GameState *gamestate);

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]);
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event);
SDL_AppResult SDL_AppIterate(void *appstate);
void SDL_AppQuit(void *appstate, SDL_AppResult result);
#endif
