#define SDL_MAIN_USE_CALLBACKS 1  
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "main.h"
#include "tetromino.h"
#include "draw.h"
#include "game.h"

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("Stupid Tetris Clone", "1.0", "com.saltyyogourt.tetrisclone");
    
    GameState *gamestate = SDL_calloc(1, sizeof(GameState));
    //Character *chara = SDL_calloc(1, sizeof(Character));
    *appstate = gamestate;
    
    gamestate->window = NULL;
    gamestate->renderer = NULL;
    gamestate->last_tick = SDL_GetTicks();
    gamestate->gravity = 1.0f/64.0f;
    gamestate->gravity_step = 0.0f;
    Player *player = &gamestate->player;

    SDL_memset(gamestate->board, T_EMPTY, sizeof(gamestate->board[0][0])*BOARD_HEIGHT*BOARD_WIDTH);

    int tetromino_id = T_T;
    if(argc > 1) tetromino_id = (char)argv[1][0]-48;
    SDL_Log("%d\n", tetromino_id);

    init_tetrominos(gamestate->piece_data);

    player->x = BOARD_WIDTH/2;
    player->y = 1;
    player->rot = 0;
    player->tetromino = &gamestate->piece_data[tetromino_id]; //lmao. should this data be duplicateD? just get it working for now. we can cast it to a pointer later.
    player->tetromino_id = tetromino_id;

    //debug testing shit.
    gamestate->board[19][5] = 2;

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
            if(!(collision & T_BOUND_RIGHT)) gamestate->player.x++;
            break;
        case T_MOVE_LEFT:
            if(!(collision & T_BOUND_LEFT)) gamestate->player.x--;
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
                    break;
            case ROT_KICK_RIGHT:
                    player->rot = new_rot;
                    player->x++;
                    break;
            case ROT_KICK_LEFT:
                    player->rot = new_rot;
                    player->x--;
                    break;
            case ROT_NOP:
                    break;
    }
}

void update_game(GameState *gamestate)
{
    //move player
    Player *player = &gamestate->player;
    unsigned char *block;
    Point offsets[4];  
    get_abs_offsetsp(player, player->rot, offsets); 
    int i,j;
    for(i = 0; BOARD_HEIGHT > i; i++){
        for(j = 0; BOARD_WIDTH > j; j++)
        {
            block = &gamestate->board[i][j];
            //TODO: check for lines.
        }
    }
    
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
    while (check_collisiong2(gamestate) & T_BOUND_OVERLAP){
        player->y--;
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
}
