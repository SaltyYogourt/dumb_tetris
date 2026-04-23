#ifndef DRAW_H_INCLUDED
#define DRAW_H_INCLUDED

#include "tetromino.h"
#include "game.h"

typedef struct CornerDisplay {
    int x, y;
    int w, h;
    int title_h;
    char *title_text;
} CornerDisplay;

void draw_init(GameState *gamestate);
void draw_game(GameState *gamestate);
void get_corner_display_center(CornerDisplay *display, int *x, int *y);
void calculate_corner_display(CornerDisplay *display, int pos, char *title);
void draw_corner_display(SDL_Renderer *renderer, CornerDisplay *display);
void draw_tetromino_in_corner_display(SDL_Renderer *renderer, CornerDisplay *display, PieceData *tetromino);
void draw_pause(GameState *gamestate);
void draw_board(unsigned char (*board)[10], SDL_Renderer *renderer);
void draw_player(Player *player, SDL_Renderer *renderer);
void draw_player_shadow(Player *player, unsigned char (*board)[10], SDL_Renderer *renderer);
void draw_tetromino_on_board(int x, int y, PieceData *tetromino, int rot, SDL_Color color, SDL_Renderer *renderer);

#endif
