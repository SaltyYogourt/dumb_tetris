#ifndef TETROMINO_H_INCLUDED
#define TETROMINO_H_INCLUDED

#include "main.h"

enum { ROT_NOP, ROT_INPLACE, ROT_KICK_RIGHT, ROT_KICK_LEFT, ROT_KICK_UP };//kick rot means we rotate in place

void init_tetrominos(PieceData *t);
void get_abs_offsets(Player *player, unsigned char rot, Point *points);
unsigned char check_rotation(GameState *gamestate, char rot);
bool center_column_rule(GameState *gamestate, char rot);
unsigned char check_collision(GameState *gamestate, int rot);

#endif
