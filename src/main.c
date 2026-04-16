#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_stdinc.h>

#include "main.h"
#include "tetromino.h"
#include "draw.h"
#include "game.h"
#include "event.h"
#include "state.h"

#define LUP_LIST_COUNT 14

static LevelData lup_list[LUP_LIST_COUNT];
static short lup_point = 0;

//replace this by serializing data from a config or something?
void init_lup_data(){
    //https://tetris.wiki/Tetris_The_Grand_Master_3_Terror-Instinct#Speed_timings_2
    lup_list[0] = (LevelData){ 
        .level = 0,
        .gravity = 4.0f,
    };
    lup_list[1] = (LevelData){ 
        .level = 20,
        .gravity = 32.0f,
    };
    lup_list[2] = (LevelData){ 
        .level = 30,
        .gravity = 64.0f,
    };
    lup_list[3] = (LevelData){ 
        .level = 33,
        .gravity = 96.0f,
    };
    lup_list[4] = (LevelData){ 
        .level = 36,
        .gravity = 128.0f,
    };
    lup_list[5] = (LevelData){ 
        .level = 39,
        .gravity = 160.0f,
    };
    lup_list[6] = (LevelData){ 
        .level = 43,
        .gravity = 192.0f,
    };
    lup_list[7] = (LevelData){ 
        .level = 47,
        .gravity = 224.0f,
    };
    lup_list[8] = (LevelData){ 
        .level = 51,
        .gravity = 256.0f,
    };
    lup_list[9] = (LevelData){ 
        .level = 100,
        .gravity = 512.0f,
    };
    lup_list[10] = (LevelData){ 
        .level = 130,
        .gravity = 768.0f,
    };
    lup_list[11] = (LevelData){ 
        .level = 160,
        .gravity = 1024.0f,
    };
    lup_list[12] = (LevelData){ 
        .level = 250,
        .gravity = 768.0f,
    };
    lup_list[13] = (LevelData){ 
        .level = 300,
        .gravity = 5120.0f,
    };
}

void hold_tetromino(GameState *gamestate){
    Player *player = &gamestate->player;
    if(gamestate->held_once){
        return;
    }
    if(player->held_tetromino_id == 255){ //PLACEHOLDER FOR "EMPTY"
        player->held_tetromino_id = player->tetromino_id;
        new_tetromino(gamestate, get_next(gamestate), STARTING_Y);
    }
    else {
        player->tetromino_id ^= player->held_tetromino_id; 
        player->held_tetromino_id ^= player->tetromino_id;
        new_tetromino(gamestate, player->tetromino_id ^= player->held_tetromino_id, STARTING_Y);
    }
    gamestate->held_once = 1;
}

int get_next(GameState *gamestate){
    int id = gamestate->next_tetromino_id;
    gamestate->next_tetromino_id = get_random_tetromino(gamestate->piece_history_idx);
    return id;
}

LevelData *get_level_up_data(short level){
    LevelData *data = NULL;
    for(short i = lup_point; LUP_LIST_COUNT > i; ++i){
        short data_level = lup_list[i].level;
        if (data_level == level){
            lup_point = i;
            data = lup_list+i;
            break;
        }
        else if (data_level > level){
            break;
        }
    }
    return data;
}

void commit_level_up(GameState *gamestate, LevelData *lup_data){
    gamestate->gravity = lup_data->gravity/256.0f;
    //TODO: if we have other stuff to adjust, do it here.
    //FIXME: have handling for any NULL/0 values. we have an issue where if we want to only update certain values per level
    //rather than everything, we will try to update them all.
}

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

void enter_exit_placeholder(GameState *gamestate){
    return;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("Stupid Tetris Clone", "1.0", "com.saltyyogourt.tetrisclone");
    
    GameState *gamestate = SDL_calloc(1, sizeof(GameState));

    *appstate = gamestate;

    gamestate->window = NULL;
    gamestate->renderer = NULL;
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
    player->held_tetromino_id = 255;

    //random seed: time
    Sint64 time;
    SDL_GetCurrentTime(&time); 
    SDL_srand(time);

    //State init
    gamestate->states = (State*)SDL_malloc(sizeof(State)*STATES_COUNT);

    gamestate->states[STATE_GAMEPLAY].update = game_loop;
    gamestate->states[STATE_GAMEPLAY].render = draw_game;
    gamestate->states[STATE_GAMEPLAY].input = gameplay_event;
    gamestate->states[STATE_GAMEPLAY].enter = enter_exit_placeholder;
    gamestate->states[STATE_GAMEPLAY].exit = enter_exit_placeholder;

    gamestate->states[STATE_PAUSE].update = pause_loop; 
    gamestate->states[STATE_PAUSE].render = draw_pause;
    gamestate->states[STATE_PAUSE].input = pause_event;
    gamestate->states[STATE_PAUSE].enter = pause_enter;
    gamestate->states[STATE_PAUSE].exit = pause_exit;

    //we're going pointer chasing baby
    gamestate->next_state = NULL;
    gamestate->current_state = &gamestate->states[STATE_GAMEPLAY];

    init_lup_data();
    LevelData *lup_data = NULL;
    if((lup_data = get_level_up_data(gamestate->level))){
        commit_level_up(gamestate, lup_data);
    }

    return SDL_APP_CONTINUE;  
}



SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    GameState *gamestate = (GameState*)appstate;
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  
    }
    
    gamestate->current_state->input(gamestate, event);
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
            LevelData *lup_data = NULL;
            if(lines){ 
                for(int i = 0; lines > i; ++i){
                    //when we collapse a line the next line to collapse goes down by 1 each time--AKA, each line is "i" times lower.
                    collapse_line(gamestate, (lines_to_kill[i])+i);
                    if((lup_data = get_level_up_data(++(gamestate->level)))){
                        commit_level_up(gamestate, lup_data);
                    }
                }
            }
            //TODO: use lines for score
            unsigned char new_tetromino_idx = get_next(gamestate);
            new_tetromino(gamestate, new_tetromino_idx, STARTING_Y);
            if(check_collisiong2(gamestate) & T_BOUND_OVERLAP) { 
                SDL_Log("oops");
            }
            push_history(new_tetromino_idx, gamestate->piece_history_idx);
            reset_delay(gamestate);
            gamestate->gravity_step = 0;
            gamestate->held_once = 0;
        }
    }
}
Uint64 lag = 0;    
void game_loop(GameState *gamestate){
    const Uint64 current_ticks = SDL_GetTicks();

    gamestate->deltatime = current_ticks - gamestate->last_tick;
    gamestate->last_tick = current_ticks;
    lag += gamestate->deltatime;
    
    while(lag >= TICK){ 
        update_game(gamestate);
        lag -= TICK;
    }
}

void game_pause(GameState *gamestate){
    //handle state change from "game" to "pause".
}

void pause_loop(GameState *gamestate){
    //pass;
}

void pause_enter(GameState *gamestate){
    gamestate->pause_tick = SDL_GetTicks();
}

void pause_exit(GameState *gamestate){
    gamestate->last_tick += (SDL_GetTicks()-gamestate->pause_tick);
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    GameState *gamestate = appstate;
    gamestate->current_state->update(gamestate);

    if(!changeState(gamestate)){
        //TODO: if shit fucked up, do something
    }

    gamestate->current_state->render(gamestate);

    SDL_Delay(1);
    return SDL_APP_CONTINUE;  
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    Point points[4];
    get_abs_offsetsp(&((GameState*)appstate)->player, ((GameState*)appstate)->player.rot, points);
    SDL_Log("tetromino idx: %d\n%d %d %d %d", ((GameState*)appstate)->player.tetromino_id, points[0].y, points[1].y, points[2].y, points[3].y);
}
