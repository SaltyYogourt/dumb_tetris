#include "draw.h"
#include "tetromino.h"
#include "game.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>

enum {  SMALL_WINDOW_BOTTOM = 0b01,
        SMALL_WINDOW_RIGHT  = 0b10,
};

enum {  SMALL_WINDOW_TOP_LEFT,
        SMALL_WINDOW_BOTTOM_LEFT = SMALL_WINDOW_BOTTOM,
        SMALL_WINDOW_TOP_RIGHT = SMALL_WINDOW_RIGHT,
        SMALL_WINDOW_BOTTOM_RIGHT = SMALL_WINDOW_RIGHT | SMALL_WINDOW_BOTTOM,
};

#define SMALL_WINDOW_WIDTH_PADDING 0.05f
#define SMALL_WINDOW_HEIGHT_PADDING 0.015f
#define SMALL_WINDOW_HEIGHT_FRACTION 0.3f
#define SMALL_WINDOW_TITLE_FRACTION 0.2f

//warning! every function is responsible for setting rect's attributes on its own!!!!!
SDL_FRect rect;
const int CELL_SIZE = WINDOW_WIDTH > WINDOW_HEIGHT ? WINDOW_HEIGHT/BOARD_HEIGHT : WINDOW_WIDTH/BOARD_WIDTH;
const float GAME_BOARD_WIDTH = CELL_SIZE*BOARD_WIDTH;
const int game_board_clear_size = (WINDOW_WIDTH-GAME_BOARD_WIDTH)/2; //FIXME: sane name needed
const int small_window_w_padding = game_board_clear_size*SMALL_WINDOW_WIDTH_PADDING;
const int small_window_h_padding = WINDOW_HEIGHT*SMALL_WINDOW_HEIGHT_PADDING;
const int small_window_w = game_board_clear_size-(small_window_w_padding*2);
const int small_window_h = WINDOW_HEIGHT*SMALL_WINDOW_HEIGHT_FRACTION;
const int small_window_title_height = small_window_w*SMALL_WINDOW_TITLE_FRACTION;
int start_pos = (WINDOW_WIDTH/2)-(BOARD_WIDTH/2)*CELL_SIZE;

void debug_gravity(GameState *gamestate){
    char debug_text[64];
    SDL_snprintf(debug_text, 64, "%f, x: %d, y: %d, idx: %d", gamestate->gravity, gamestate->player.x, gamestate->player.y, gamestate->player.tetromino_id);
    SDL_SetRenderDrawColor(gamestate->renderer, 255,255,255,255);
    SDL_RenderDebugText(gamestate->renderer, 0, 24, debug_text);
}

void draw_game(GameState *gamestate){
    SDL_SetRenderDrawColor(gamestate->renderer, 16, 16, 16, SDL_ALPHA_OPAQUE);  
    SDL_RenderClear(gamestate->renderer);
    //SDL_Log("screen width: %d; board width: %d", WINDOW_WIDTH, BOARD_WIDTH*CELL_SIZE);
    draw_board(gamestate->board, gamestate->renderer);
    draw_player(&gamestate->player, gamestate->renderer);
    draw_player_shadow(&gamestate->player, gamestate->board, gamestate->renderer);
    draw_small_window(gamestate->renderer, "placeholder", SMALL_WINDOW_TOP_LEFT);
    draw_small_window(gamestate->renderer, "placeholder", SMALL_WINDOW_BOTTOM_LEFT);
    draw_small_window(gamestate->renderer, "placeholder", SMALL_WINDOW_TOP_RIGHT);
    draw_small_window(gamestate->renderer, "placeholder", SMALL_WINDOW_BOTTOM_RIGHT);
    debug_gravity(gamestate);
    SDL_RenderPresent(gamestate->renderer);
}

void draw_pause(GameState *gamestate){
    SDL_SetRenderDrawColor(gamestate->renderer, 64,192,192,128);
    SDL_FRect some_rect = {
        .x = 10,
        .y = 10,
        .w = 220,
        .h = 460,
    };

    SDL_RenderFillRect(gamestate->renderer,&some_rect);
    SDL_RenderPresent(gamestate->renderer);
}

void draw_small_window(SDL_Renderer *renderer, char *title, int pos){
    rect.w = small_window_w;
    rect.h = small_window_h;

    //TODO: evaulate if these operations are worth what we save in eliminating branches
    //FIXME: we cast the results to bools so any >1 value becomes 1. find cleaner way to do this?
    rect.x = SDL_abs(((WINDOW_WIDTH-small_window_w)*(bool)(pos & SMALL_WINDOW_RIGHT)) - small_window_w_padding);
    rect.y = SDL_abs(((WINDOW_HEIGHT-small_window_h)*(bool)(pos & SMALL_WINDOW_BOTTOM)) - small_window_h_padding);
  
    SDL_SetRenderDrawColor(renderer, 16, 192, 16, SDL_ALPHA_OPAQUE);  
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 16, 96, 16, SDL_ALPHA_OPAQUE);  
    rect.h = small_window_title_height;
    SDL_RenderFillRect(renderer, &rect);
}

void draw_board(unsigned char (*board)[10], SDL_Renderer *renderer)
{
    rect.w = rect.h = CELL_SIZE;

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
    rect.w = rect.h = CELL_SIZE;
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
