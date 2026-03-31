#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_stdinc.h>

#include "main.h"
#include "tetromino.h"
#include "draw.h"
#include "game.h"


void hard_drop(GameState *gamestate, bool sonicdrop){
    gamestate->player.y = get_player_floor(&gamestate->player, gamestate->board);
    if(!sonicdrop)
        gamestate->lock_time = 0;
}

void reset_delay(GameState *gamestate)
{
    //TODO: do we want to reset lock delay N number of times?
    gamestate->lock_time = LOCK_DELAY;
}

void lock_piece(GameState *gamestate) {
   Point abs_points[4];
   get_abs_offsetsp(&gamestate->player, gamestate->player.rot, abs_points);

   for(int i = 0; 4 > i; ++i){
        gamestate->board[abs_points[i].y][abs_points[i].x] = 1;
   }
}

void push_history(unsigned char idx, unsigned char history[4]){
   history[0] = history[1];
   history[1] = history[2];
   history[2] = history[3];
   history[3] = idx;
}

void new_tetromino(GameState *gamestate, int id, int height){
    Player *player = &gamestate->player;
    player->tetromino = &gamestate->piece_data[id];
    player->tetromino_id = id; 
    player->rot = 0;
    //FIXME: currently we don't care if we push up "out of bounds" which breaks shit. write the "fail condition", the game shouldn't crash JUST because you lost.
    player->y = height;
    player->x = 3; //TODO: calculate "center" of piece. or, alternatively, hardcode that shit per piecedata
}

unsigned char get_random_tetromino(unsigned char history[4]){
    int i,j,piece_idx;
    for(i = 0; TETROMINO_RAND_RETRIES > i; ++i){
        piece_idx = SDL_rand(TETROMINO_COUNT);
        for(j = 0; TETROMINO_HISTORY_LEN > j; ++j){
            if(piece_idx == history[j]) break;
            else if(j == TETROMINO_HISTORY_LEN-1) return piece_idx;
        }
    }
    
    SDL_Log("Returning piece in history.");
    return piece_idx;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("Stupid Tetris Clone", "1.0", "com.saltyyogourt.tetrisclone");
    
    GameState *gamestate = SDL_calloc(1, sizeof(GameState));

    *appstate = gamestate;

    gamestate->window = NULL;
    gamestate->renderer = NULL;
    gamestate->last_tick = SDL_GetTicks();
    gamestate->gravity = 1.0f/64.0f;
    gamestate->gravity_step = 0.0f;
    gamestate->lock_time = LOCK_DELAY;
    
    //initialize history to Z pieces. there's a reason why we do this, im sure
    SDL_memset(gamestate->piece_history_idx, T_Z, 4);

    Player *player = &gamestate->player;

    SDL_memset(gamestate->board, T_EMPTY, sizeof(gamestate->board[0][0])*BOARD_HEIGHT*BOARD_WIDTH);

    int tetromino_id = T_J;
    if(argc > 1) tetromino_id = (char)argv[1][0]-48;
    SDL_Log("%d\n", tetromino_id);

    init_tetrominos(gamestate->piece_data);

    new_tetromino(gamestate, tetromino_id, STARTING_Y);

    //random seed: time
    Sint64 time;
    SDL_GetCurrentTime(&time); 
    SDL_srand(time);

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("examples/entry", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &gamestate->window, &gamestate->renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(gamestate->renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_SetRenderVSync(gamestate->renderer, 1);
    SDL_SetRenderDrawBlendMode(gamestate->renderer, SDL_BLENDMODE_BLEND);

    return SDL_APP_CONTINUE;  
}


SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    GameState *gamestate = (GameState*)appstate;
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  
    }
    if (event->type == SDL_EVENT_KEY_DOWN){
        switch (event->key.scancode){
            case SDL_SCANCODE_D:
                movx(gamestate, T_MOVE_RIGHT);
                break;
            case SDL_SCANCODE_A:
                movx(gamestate, T_MOVE_LEFT);
                break;
            case SDL_SCANCODE_W:
                rot(gamestate, ROT_DIR_CLOCKWISE);
                break;
            case SDL_SCANCODE_S:
                rot(gamestate, ROT_DIR_COUNTERCLOCKWISE);
                break;
            case SDL_SCANCODE_P:
                gamestate->gravity=2.0f;
                break;
            case SDL_SCANCODE_Q:
                gamestate->gravity+=8/64.0f;
                break;
            case SDL_SCANCODE_E:
                gamestate->gravity-=8/64.0f;
                break;
            case SDL_SCANCODE_R:
                gamestate->player.y = 3;
                gamestate->gravity_step = 0.0f;
                break;
            case SDL_SCANCODE_SPACE:
                hard_drop(gamestate, false);
                break;
            default:
                break;
        }
    }
    return SDL_APP_CONTINUE;  
}

void movx(GameState *gamestate, int dir){ 
    unsigned char collision = check_collisiong2(gamestate);
    switch (dir) {
        case T_MOVE_RIGHT:
            if(!(collision & T_BOUND_RIGHT)) {
                gamestate->player.x++;
                reset_delay(gamestate);
            }
            break;
        case T_MOVE_LEFT:
            if(!(collision & T_BOUND_LEFT)) {
                gamestate->player.x--;
                reset_delay(gamestate);
            }
            break;
        default: 
            break;
    }
}

void rot(GameState *gamestate, int dir){
    Player *player = &gamestate->player;
    int new_rot = player->rot+dir;
   
    if (new_rot > 3) new_rot = 0;
    else if (new_rot < 0) new_rot = 3; 

    switch (check_rotation(gamestate, new_rot)) {
            case ROT_INPLACE:
                    player->rot = new_rot;
                    reset_delay(gamestate);
                    break;
            case ROT_KICK_RIGHT:
                    player->rot = new_rot;
                    player->x++;
                    reset_delay(gamestate);
                    break;
            case ROT_KICK_LEFT:
                    player->rot = new_rot;
                    player->x--;
                    reset_delay(gamestate);
                    break;
            default:
            case ROT_NOP:
                    break;
    }
}

/* 
 * get count of lines on board
 * and return number as value
 * and positions in *lines
 */
unsigned int get_lines(GameState *gamestate, unsigned int *lines){
    Player *player = &gamestate->player;
    unsigned char block;
    char i,j;
    unsigned int line_count = 0;
    for(i = BOARD_HEIGHT-1; i >= 0; --i){
        for(j = BOARD_WIDTH-1; j >= 0; --j){
            block = gamestate->board[i][j];
            if(!block) break;
        }
        if(block) {
            lines[line_count++] = i;
        }
    }
    return line_count;
}

void collapse_line(GameState *gamestate, int line){
    unsigned char block;
    for(int i = line; i >= 0; --i){
        for(int j = BOARD_WIDTH-1; j >= 0; --j){
            if(i == line) { 
                gamestate->board[i][j] = 0; 
                continue;
            }
            block = gamestate->board[i][j];
            if(!block) continue;
            gamestate->board[i+1][j] = block;
            gamestate->board[i][j] = T_EMPTY;
        }
    }
}

void update_game(GameState *gamestate)
{
    Player *player = &gamestate->player;
    unsigned char collision = check_collisiong2(gamestate);

    //fall
    if (collision & T_BOUND_BELOW);
    else if(SDL_floor((gamestate->gravity_step+=gamestate->gravity))){
        while(SDL_floor(gamestate->gravity_step) > 0 && !(check_collisiong2(gamestate) & T_BOUND_BELOW)) {
            player->y++;
            gamestate->gravity_step-=1;
        }
    }

    //check if we fucked up and ended up inside of a block; push us out. Hacky.
    //while (check_collisiong2(gamestate) & T_BOUND_OVERLAP){
    //    player->y--;
    //}

    unsigned int lines_to_kill[BOARD_HEIGHT];
    if(check_collisiong2(gamestate) & T_BOUND_BELOW){
        if(--gamestate->lock_time < 0){
            lock_piece(gamestate);
            int lines = get_lines(gamestate, lines_to_kill);
            if(lines){ 
                for(int i = 0; lines > i; ++i){
                    //when we collapse a line the next line to collapse goes down by 1 each time--AKA, each line is "i" times lower.
                    collapse_line(gamestate, (lines_to_kill[i])+i);
                }
            }
            //TODO: use lines for score
            unsigned char new_tetromino_idx = get_random_tetromino(gamestate->piece_history_idx);
            new_tetromino(gamestate, new_tetromino_idx, STARTING_Y);
            if(check_collisiong2(gamestate) & T_BOUND_OVERLAP) { 
                SDL_Log("oops");
            }
            push_history(new_tetromino_idx, gamestate->piece_history_idx);
            reset_delay(gamestate);
            gamestate->gravity_step = 0;
        }
    }
}

Uint64 lag = 0;    
SDL_AppResult SDL_AppIterate(void *appstate)
{
    const Uint64 current_ticks = SDL_GetTicks();
    GameState *gamestate = appstate;

    gamestate->deltatime = current_ticks - gamestate->last_tick;
    gamestate->last_tick = current_ticks;
    lag += gamestate->deltatime;
    
    while(lag >= TICK){ 
        update_game(gamestate);
        lag -= TICK;
    }

    draw_game(gamestate);

    SDL_Delay(1);
    return SDL_APP_CONTINUE;  
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    Point points[4];
    get_abs_offsetsp(&((GameState*)appstate)->player, ((GameState*)appstate)->player.rot, points);
    SDL_Log("tetromino idx: %d\n%d %d %d %d", ((GameState*)appstate)->player.tetromino_id, points[0].y, points[1].y, points[2].y, points[3].y);
}
