#include "draw.h"
#include "tetromino.h"
#include "game.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3_ttf/SDL_ttf.h>

enum {  CORNER_DISPLAY_BOTTOM = 0b01,
        CORNER_DISPLAY_RIGHT  = 0b10,
};

enum {  CORNER_DISPLAY_TOP_LEFT,
        CORNER_DISPLAY_BOTTOM_LEFT = CORNER_DISPLAY_BOTTOM,
        CORNER_DISPLAY_TOP_RIGHT = CORNER_DISPLAY_RIGHT,
        CORNER_DISPLAY_BOTTOM_RIGHT = CORNER_DISPLAY_RIGHT | CORNER_DISPLAY_BOTTOM,
};

#define CORNER_DISPLAY_WIDTH_PADDING 0.05f
#define CORNER_DISPLAY_HEIGHT_PADDING 0.015f
#define CORNER_DISPLAY_HEIGHT_FRACTION 0.3f
#define CORNER_DISPLAY_TITLE_FRACTION 0.2f

//warning! every function is responsible for setting rect's attributes on its own!!!!!
SDL_FRect rect;

//FIXME: holy shit. clean this up.
const int CELL_SIZE = WINDOW_WIDTH > WINDOW_HEIGHT ? WINDOW_HEIGHT/BOARD_HEIGHT : WINDOW_WIDTH/BOARD_WIDTH;
const float GAME_BOARD_WIDTH = CELL_SIZE*BOARD_WIDTH;
const int game_board_clear_size = (WINDOW_WIDTH-GAME_BOARD_WIDTH)/2; //FIXME: sane name needed
const int corner_display_w_padding = game_board_clear_size*CORNER_DISPLAY_WIDTH_PADDING;
const int corner_display_h_padding = WINDOW_HEIGHT*CORNER_DISPLAY_HEIGHT_PADDING;
const int corner_display_w = game_board_clear_size-(corner_display_w_padding*2);
const int corner_display_h = WINDOW_HEIGHT*CORNER_DISPLAY_HEIGHT_FRACTION;
const int corner_display_title_height = corner_display_w*CORNER_DISPLAY_TITLE_FRACTION;
int start_pos = (WINDOW_WIDTH/2)-(BOARD_WIDTH/2)*CELL_SIZE;

static CornerDisplay next_tetromino_corner_display;
static CornerDisplay held_tetromino_corner_display;

//static 
static SDL_Color font_color = { .r = 224, .g = 224, .b = 224, .a = 255 };

void draw_init(GameState *gamestate){
    TTF_Init();
    calculate_corner_display(&next_tetromino_corner_display, CORNER_DISPLAY_TOP_RIGHT);
    calculate_corner_display(&held_tetromino_corner_display, CORNER_DISPLAY_TOP_LEFT);


    //TODO: think of clean way to pull this from a cdn/remote repository?
    char *font_path = "resources/fonts/PressStart2P-Regular.ttf";

    gamestate->font = TTF_OpenFont(font_path, 18.0f);
    SDL_Log("%s\n", SDL_GetError());
    //font shit here
}

void debug_gravity(GameState *gamestate){
    char debug_text[64];
    SDL_snprintf(debug_text, 64, "%f, x: %d, y: %d, idx: %d\nlevel: %d", gamestate->gravity, gamestate->player.x, gamestate->player.y, gamestate->player.tetromino_id, gamestate->level);
    SDL_SetRenderDrawColor(gamestate->renderer, 255,255,255,255);
    SDL_RenderDebugText(gamestate->renderer, 0, 24, debug_text);
}

void draw_game(GameState *gamestate){
    SDL_SetRenderDrawColor(gamestate->renderer, 16, 16, 16, SDL_ALPHA_OPAQUE);  
    SDL_RenderClear(gamestate->renderer);
    draw_board(gamestate->board, gamestate->renderer);
    draw_player(&gamestate->player, gamestate->renderer);
    draw_player_shadow(&gamestate->player, gamestate->board, gamestate->renderer);

    //Next display
    draw_corner_display(gamestate->renderer, &next_tetromino_corner_display, gamestate->font, "Next");
    draw_tetromino_in_corner_display(gamestate->renderer, &next_tetromino_corner_display, &gamestate->piece_data[gamestate->next_tetromino_id]);

    draw_corner_display(gamestate->renderer, &held_tetromino_corner_display, gamestate->font, "Held");
    if(gamestate->player.held_tetromino_id != 255){
        draw_tetromino_in_corner_display(gamestate->renderer, &held_tetromino_corner_display, &gamestate->piece_data[gamestate->player.held_tetromino_id]);
    }
    debug_gravity(gamestate);
    
    SDL_RenderPresent(gamestate->renderer);
}

void draw_pause(GameState *gamestate){
    SDL_SetRenderDrawColor(gamestate->renderer, 64,192,192,128);
    SDL_FRect some_rect = {
        .x = (WINDOW_WIDTH/2.0f)-110,
        .y = (WINDOW_HEIGHT/2.0f)-230,
        .w = 220,
        .h = 460,
    };

    SDL_RenderFillRect(gamestate->renderer,&some_rect);
    SDL_RenderPresent(gamestate->renderer);
}

void draw_tetromino_in_corner_display(SDL_Renderer *renderer, CornerDisplay *display, PieceData *tetromino){
    int cell_size = display->h/6; //assume our height will always be smaller than the width
    int t_min_w, t_max_w, t_min_h, t_max_h;
    int i;

    t_max_w = t_min_w = tetromino->offset[0][0].x;
    t_max_h = t_min_h = tetromino->offset[0][0].y;

    rect.w = rect.h = cell_size;
    SDL_Color color = tetromino->color;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    for(i=1; 4 > i; ++i){
        int x = tetromino->offset[0][i].x;
        int y = tetromino->offset[0][i].y;
        t_max_w = x > t_max_w ? x : t_max_w;
        t_min_w = x < t_min_w ? x : t_min_w;
        t_max_h = y > t_max_h ? y : t_max_h;
        t_min_h = y < t_min_h ? y : t_min_h;
    }

    int t_width = t_max_w - t_min_w + 1;
    int t_height = t_max_h - t_min_h + 1;
    int center_x, center_y;
    get_corner_display_center(display, &center_x, &center_y);
    int start_x = center_x-(t_width*cell_size/2.0f);
    int start_y = center_y-(t_height*cell_size/2.0f);

    for(i=0; 4 > i; ++i){
        rect.x = SDL_abs(t_min_w*cell_size)+start_x+(tetromino->offset[0][i].x*cell_size);
        rect.y = SDL_abs(t_min_h*cell_size)+start_y+(tetromino->offset[0][i].y*cell_size);
        SDL_RenderFillRect(renderer,&rect);
    }
}

void get_corner_display_center(CornerDisplay *display, int *x, int *y){
    if(x)
        *x = display->x+(display->w/2);
    if(y)
        *y = (display->y+display->title_h)+((display->h-display->title_h)/2);
}

void calculate_corner_display(CornerDisplay *display, int pos){
    display->w = corner_display_w;
    display->h = corner_display_h;
    display->x = SDL_abs(((WINDOW_WIDTH-corner_display_w)*(bool)(pos & CORNER_DISPLAY_RIGHT)) - corner_display_w_padding);
    display->y = SDL_abs(((WINDOW_HEIGHT-corner_display_h)*(bool)(pos & CORNER_DISPLAY_BOTTOM)) - corner_display_h_padding);
    display->title_h = corner_display_title_height;
}

void draw_corner_display(SDL_Renderer *renderer, CornerDisplay *display, TTF_Font *font, char *title){
    rect.w = display->w;
    rect.h = display->h;

    //TODO: evaulate if these operations are worth what we save in eliminating branches
    //FIXME: we cast the results to bools so any >1 value becomes 1. find cleaner way to do this?
    rect.x = display->x;
    rect.y = display->y;
  
    SDL_SetRenderDrawColor(renderer, 16, 192, 16, SDL_ALPHA_OPAQUE);  
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 16, 96, 16, SDL_ALPHA_OPAQUE);  
    rect.h = display->title_h;
    SDL_RenderFillRect(renderer, &rect);

    SDL_FRect dst;
    const float scale = 1.0f;

    SDL_Texture *font_texture = NULL;
    SDL_Surface *text;
    text = TTF_RenderText_Blended(font, title, 0, font_color);
    font_texture = SDL_CreateTextureFromSurface(renderer, text);
    
    /* Center the text and scale it up */
    SDL_SetRenderScale(renderer, scale, scale);
    SDL_GetTextureSize(font_texture, &dst.w, &dst.h);
    dst.x = display->x + (((display->w / scale) - dst.w) / 2);
    dst.y = display->y + (((display->title_h / scale) - dst.h) / 2);

    SDL_RenderTexture(renderer, font_texture, NULL, &dst);

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
