#ifndef TETROMINO_H_INCLUDED
#define TETROMINO_H_INCLUDED

#include "main.h"

void init_tetrominos(PieceData *t);
void get_abs_offsets(Player *player, unsigned char rot, Point *points);
unsigned char check_collision(GameState *gamestate, int rot);

#endif
