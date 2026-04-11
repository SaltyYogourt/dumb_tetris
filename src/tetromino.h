#ifndef TETROMINO_H_INCLUDED
#define TETROMINO_H_INCLUDED

#include "game.h"

#define TETROMINO_COUNT 7

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

enum { ROT_NOP, ROT_INPLACE, ROT_KICK_RIGHT, ROT_KICK_LEFT, ROT_KICK_UP };//kick rot means we rotate in place

void init_tetrominos(PieceData *t);
int get_player_floor(Player *player, unsigned char (*board)[10]);
void get_abs_offsets(int x, int y, PieceData *tetromino, unsigned char rot, Point *points);
void get_abs_offsetsp(Player *player, unsigned char rot, Point *points);
unsigned char check_rotation(GameState *gamestate, char rot);
bool center_column_rule(GameState *gamestate, char rot);
unsigned char check_collisiong1(GameState *gamestate, int rot);
unsigned char check_collisiong2(GameState *gamestate);
unsigned char check_collision(int x, int y, PieceData *tetromino, unsigned char (*board)[10], int rot);

#endif
