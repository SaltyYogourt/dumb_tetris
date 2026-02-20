#define SDL_MAIN_USE_CALLBACKS 1  
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "main.h"
#include "tetromino.h"


SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("Stupid Tetris Clone", "1.0", "com.saltyyogourt.tetrisclone");
    
    GameState *gamestate = SDL_calloc(1, sizeof(GameState));
    //Character *chara = SDL_calloc(1, sizeof(Character));
    *appstate = gamestate;
    
    gamestate->window = NULL;
    gamestate->renderer = NULL;
    Player *player = &gamestate->player;
    SDL_memset(gamestate->board, T_EMPTY, sizeof(gamestate->board[0][0])*BOARD_HEIGHT*BOARD_WIDTH);

    int tetromino_id = T_T;
    if(argc > 1) tetromino_id = (char)argv[1][0]-48;
    SDL_Log("%d\n", tetromino_id);

    init_tetrominos(gamestate->piece_data);
    player->x = BOARD_WIDTH/2;
    player->y = 1;
    player->rot = 0;
    //player->tetromino = &gamestate->piece_data[T_T]; //lmao. should this data be duplicateD? just get it working for now. we can cast it to a pointer later.
    player->tetromino = &gamestate->piece_data[tetromino_id]; //lmao. should this data be duplicateD? just get it working for now. we can cast it to a pointer later.
    
    gamestate->board[19][5] = 2;

    SDL_SetRenderVSync(gamestate->renderer, 1);
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("examples/entry", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &gamestate->window, &gamestate->renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(gamestate->renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    return SDL_APP_CONTINUE;  
}


SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  
    }
    //TODO: EVENT HANDLING SHOULD NOT UPDATE THE GAMESTATE!
    //probably best to inform the game it has to do an action(s) on each tick/frame.
    //we should also avoid only polling for game inputs ever n ticks. that would be annoying to the player
    if (event->type == SDL_EVENT_KEY_DOWN){
        if (event->key.scancode == SDL_SCANCODE_D){
           ((GameState *)appstate)->player.move_x = T_MOVE_RIGHT;
        }
        else if (event->key.scancode == SDL_SCANCODE_A){
           ((GameState *)appstate)->player.move_x = T_MOVE_LEFT;
        }
        else if (event->key.scancode == SDL_SCANCODE_W){
           ((GameState *)appstate)->player.rot_dir = 1;
        }
        else if (event->key.scancode == SDL_SCANCODE_S){
           ((GameState *)appstate)->player.rot_dir = -1;
        }
    }
    return SDL_APP_CONTINUE;  
}

void update_game(GameState *gamestate)
{
    //move player
    Player *player = &gamestate->player;
    unsigned char *block;
    Point offsets[4];  
    get_abs_offsets(player, player->rot, offsets); 
    int i,j;
    for(i = 0; BOARD_HEIGHT > i; i++){
        for(j = 0; BOARD_WIDTH > j; j++)
        {
            block = &gamestate->board[i][j];
            //TODO: check for lines.
        }
    }
    
    //collision checking
    unsigned char collision = check_collision(gamestate, player->rot);
    
    if ( !(collision & T_BOUND_BELOW) ) player->y++;

    //handle movement
    if ( player->move_x == T_MOVE_RIGHT && !(collision & T_BOUND_RIGHT) )
        player->x++;
    else if ( player->move_x == T_MOVE_LEFT && !(collision & T_BOUND_LEFT) )
        player->x--;

    gamestate->player.move_x = T_MOVE_STILL;

    //check if we fucked up and ended up inside of a block; push us out. Hacky.
    while (check_collision(gamestate,player-> rot) & T_BOUND_OVERLAP){
        player->y--;
    }

    //handle rotation
    if (player->rot_dir) {
        int new_rot = player->rot+player->rot_dir;
       
        if (new_rot > 3) new_rot = 0;
        else if (new_rot < 0) new_rot = 3; 

        while (check_collision(gamestate, new_rot) & (T_BOUND_OVERLAP | T_BOUND_FLOOR)){
            player->y--;
        }

        while (check_collision(gamestate, new_rot) & T_BOUND_RIGHT_WALL){
            player->x--;
        }

        while (check_collision(gamestate, new_rot) & T_BOUND_LEFT_WALL){
            player->x++;
        }

        player->rot_dir = 0;
        player->rot = new_rot;
    }
}

void draw_board(GameState *gamestate)
{
    SDL_SetRenderDrawColor(gamestate->renderer, 16, 16, 16, SDL_ALPHA_OPAQUE);  
    SDL_RenderClear(gamestate->renderer);

    SDL_FRect rect;
    rect.w = rect.h = CELL_SIZE;
    int i,j;
        
    for(i = 0; BOARD_HEIGHT > i; i++){
        for(j = 0; BOARD_WIDTH > j; j++){
            if(gamestate->board[i][j] == T_EMPTY) continue;
            rect.x = j*CELL_SIZE;
            rect.y = i*CELL_SIZE;
            if(gamestate->board[i][j] == T_PLAYER) SDL_SetRenderDrawColor(gamestate->renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
            else if(gamestate->board[i][j] == T_BLOCK) SDL_SetRenderDrawColor(gamestate->renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(gamestate->renderer,&rect);
        }
    }
    
    SDL_Color player_color = gamestate->player.tetromino->color;
    SDL_SetRenderDrawColor(gamestate->renderer, player_color.r, player_color.g, player_color.b, SDL_ALPHA_OPAQUE);
    Point points_to_draw[4];
    get_abs_offsets(&gamestate->player, gamestate->player.rot, points_to_draw);

    for(i=0; 4 > i; ++i){
        rect.x = points_to_draw[i].x*CELL_SIZE;
        rect.y = points_to_draw[i].y*CELL_SIZE;
        SDL_RenderFillRect(gamestate->renderer,&rect);
    }
    SDL_RenderPresent(gamestate->renderer);
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    const Uint64 ticks = SDL_GetTicks();
    GameState *gamestate = appstate;
    draw_board(gamestate);
    while((ticks - gamestate->last_tick) >= TICKRATE){
        update_game(gamestate);
        gamestate->last_tick += TICKRATE;
    }
    
    return SDL_APP_CONTINUE;  
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
}
