#ifndef DRAW_H_INCLUDED
#define DRAW_H_INCLUDED

#include "tetromino.h"
#include "game.h"

void draw_game(GameState *gamestate);
void draw_board(unsigned char (*board)[10], SDL_Renderer *renderer);
void draw_player(Player *player, SDL_Renderer *renderer);
void draw_player_shadow(Player *player, unsigned char (*board)[10], SDL_Renderer *renderer);
void draw_tetromino_on_board(int x, int y, PieceData *tetromino, int rot, SDL_Color color, SDL_Renderer *renderer);

#endif
