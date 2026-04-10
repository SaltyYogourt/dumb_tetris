#include "draw.h"
#include "tetromino.h"
#include "game.h"
#include <SDL3/SDL.h>

#define SMALL_WINDOW_WIDTH_PADDING 0.015f
#define SMALL_WINDOW_WIDTH_FRACTION (WINDOW_WIDTH/4.0f)-(SMALL_WINDOW_WIDTH_PADDING*2)
#define SMALL_WINDOW_HEIGHT_FRACTION 0.3f
#define SMALL_WINDOW_TITLE_FRACTION 0.2f

SDL_FRect rect;
const int CELL_SIZE = WINDOW_WIDTH > WINDOW_HEIGHT ? WINDOW_HEIGHT/BOARD_HEIGHT : WINDOW_WIDTH/BOARD_WIDTH;
const int small_window_w = WINDOW_WIDTH*SMALL_WINDOW_WIDTH_FRACTION;
const int small_window_h = WINDOW_HEIGHT*SMALL_WINDOW_HEIGHT_FRACTION;
const int small_window_title_height = small_window_w*SMALL_WINDOW_TITLE_FRACTION;
int start_pos = (WINDOW_WIDTH/2)-(BOARD_WIDTH/2)*CELL_SIZE;

void draw_game(GameState *gamestate){
    rect.w = rect.h = CELL_SIZE;
    SDL_Log("screen width: %d; board width: %d", WINDOW_WIDTH, BOARD_WIDTH*CELL_SIZE);
    draw_board(gamestate->board, gamestate->renderer);
    draw_player(&gamestate->player, gamestate->renderer);
    draw_player_shadow(&gamestate->player, gamestate->board, gamestate->renderer);
    SDL_RenderPresent(gamestate->renderer);
}

void draw_small_window(char *title, int pos){
    
}

void draw_board(unsigned char (*board)[10], SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 16, 16, 16, SDL_ALPHA_OPAQUE);  
    SDL_RenderClear(renderer);

    int i,j;

        
    for(i = 0; BOARD_HEIGHT > i; i++){
        for(j = 0; BOARD_WIDTH > j; j++){
            if(board[i][j] == T_EMPTY) continue;
            rect.x = start_pos+(j*CELL_SIZE);
            rect.y = i*CELL_SIZE;
            if(board[i][j] == T_PLAYER) SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
            else if(board[i][j] == T_BLOCK) SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer,&rect);
        }
    }
}

void draw_player(Player *player, SDL_Renderer *renderer){
    draw_tetromino_on_board(player->x, player->y, player->tetromino, player->rot, player->tetromino->color, renderer); 
}

void draw_player_shadow(Player *player, unsigned char (*board)[10], SDL_Renderer *renderer){
    int y = get_player_floor(player, board);
    SDL_Color shadow_color = player->tetromino->color;
    shadow_color.a = 64;
    draw_tetromino_on_board(player->x, y, player->tetromino, player->rot, shadow_color, renderer); 
}

void draw_tetromino_on_board(int x, int y, PieceData *tetromino, int rot, SDL_Color color, SDL_Renderer *renderer){
    int i;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    Point points_to_draw[4];
    get_abs_offsets(x, y, tetromino, rot, points_to_draw);

    for(i=0; 4 > i; ++i){
        rect.x = start_pos+(points_to_draw[i].x*CELL_SIZE);
        rect.y = points_to_draw[i].y*CELL_SIZE;
        SDL_RenderFillRect(renderer,&rect);
    }
}
