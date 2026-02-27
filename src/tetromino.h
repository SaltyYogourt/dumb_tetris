#ifndef TETROMINO_H_INCLUDED
#define TETROMINO_H_INCLUDED

#include "main.h"

enum { ROT_NOP, ROT_INPLACE, ROT_KICK_RIGHT, ROT_KICK_LEFT, ROT_KICK_UP };//kick rot means we rotate in place

void init_tetrominos(PieceData *t);
void get_abs_offsets(int x, int y, PieceData *tetromino, unsigned char rot, Point *points);
void get_abs_offsetsp(Player *player, unsigned char rot, Point *points);
unsigned char check_rotation(GameState *gamestate, char rot);
bool center_column_rule(GameState *gamestate, char rot);
unsigned char check_collisiong1(GameState *gamestate, int rot);
unsigned char check_collisiong2(GameState *gamestate);
unsigned char check_collision(int x, int y, PieceData *tetromino, unsigned char (*board)[10], int rot);

#endif
