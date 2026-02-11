#define SDL_MAIN_USE_CALLBACKS 1  
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define MOVE_SPEED 16
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define CELL_SIZE 24

#define TICKRATE 250

#define WINDOW_WIDTH BOARD_WIDTH*CELL_SIZE
#define WINDOW_HEIGHT BOARD_HEIGHT*CELL_SIZE

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

enum { T_EMPTY, T_PLAYER, T_BLOCK };
enum { T_I, T_O, T_T, T_J, T_L, T_S, T_Z };

int tetrominos_old[7][4][4] = {
    {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}}, // I
    {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}}, // O
    {{0,0,0,0},{0,1,0,0},{1,1,1,0},{0,0,0,0}}, // T
    {{0,0,0,0},{1,0,0,0},{1,1,1,1},{0,0,0,0}}, // J
    {{0,0,0,0},{0,0,0,1},{1,1,1,1},{0,0,0,0}}, // L
    {{0,0,0,0},{0,0,1,1},{0,1,1,0},{0,0,0,0}}, // S
    {{0,0,0,0},{1,1,0,0},{0,1,1,0},{0,0,0,0}}, // Z
};

typedef struct {
    char x;
    char y;
} Point; //a point in 2D space

typedef struct { 
    SDL_Color color;
    Point offset[4][3]; //we ONLY have four offsets excluding 0,0, right? 
} PieceData;

typedef struct {
    unsigned char x;
    unsigned char y;
    unsigned char rot;
    PieceData tetromino;
    //unsigned char tetrominoIdx;
    char move_x;
    bool move_y;
} Player;

typedef struct {
    unsigned char board[BOARD_HEIGHT][BOARD_WIDTH];
    Uint64 last_tick;
    PieceData piece_data[7];
    Player player;
} GameState;

void init_tetrominos(PieceData *t){
    PieceData I, O, T, J, L, S, Z; 

    //declare I
    I.color.r = 64; I.color.g = 64; I.color.b = 255;
    //I offsets
    memcpy( I.offset[0], (Point[]){ {-1,0}, {1,0}, {2,0} }, sizeof(Point[3]) ); //rot 0
    memcpy( I.offset[1], (Point[]){ {0,-1}, {0,1}, {0,2} }, sizeof(Point[3]) ); //rot 1
    memcpy( I.offset[2], I.offset[0], sizeof(Point[3]) ); //rot 2
    memcpy( I.offset[3], I.offset[1], sizeof(Point[3]) ); //rot 3

    O.color.r = 255; O.color.g = 165; O.color.b = 0; 

    memcpy( O.offset[0], (Point[]){ {1,0}, {0,1}, {1,1} },  sizeof(Point[3]) );
    memcpy( O.offset[1], O.offset[0], sizeof(Point[3])); 
    memcpy( O.offset[2], O.offset[0], sizeof(Point[3]));
    memcpy( O.offset[3], O.offset[0], sizeof(Point[3])); //it's a fucking square.

    t[0] = I; t[1] = O;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("NoTetris Prayge", "1.0", "com.example.notetris");
    
    GameState *gamestate = SDL_calloc(1, sizeof(GameState));
    //Character *chara = SDL_calloc(1, sizeof(Character));
    *appstate = gamestate;
    
    Player *player = &gamestate->player;
    SDL_memset(gamestate->board, T_EMPTY, sizeof(gamestate->board[0][0])*BOARD_HEIGHT*BOARD_WIDTH);

    init_tetrominos(gamestate->piece_data);
    player->x = BOARD_WIDTH/2;
    player->y = 1;
    player->tetromino = gamestate->piece_data[0]; //lmao. should this data be duplicateD? just get it working for now.
    
    gamestate->board[19][5] = 2;
    SDL_SetRenderVSync(renderer, 1);
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("examples/entry", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    return SDL_APP_CONTINUE;  
}


SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  
    }
    if (event->type == SDL_EVENT_KEY_DOWN){
        if (event->key.scancode == SDL_SCANCODE_D){
           ((GameState *)appstate)->player.x += 1;
        }
        else if (event->key.scancode == SDL_SCANCODE_A){
           ((GameState *)appstate)->player.x -= 1;
        }
    }
    return SDL_APP_CONTINUE;  
}


void update_game(GameState *gamestate)
{
    //move player
    Player *player = &gamestate->player;
    unsigned char *block;
    unsigned char player_block;
    int i,j;
    player->move_y = true;
    SDL_Log("Player Y: %d\n", player->y);

    for(i = 0; BOARD_HEIGHT > i; i++){
        for(j = 0; BOARD_WIDTH > j; j++)
        {

            block = &gamestate->board[i][j];
            if(*block == T_PLAYER) *block = T_EMPTY; //clear that shit out.
            //TODO: check for lines.
        }
    }


    if(player->move_y) player->y++;
}

void draw_board(GameState *gamestate)
{
    //Tetromino *player = &gamestate->player;

    SDL_SetRenderDrawColor(renderer, 16, 16, 16, SDL_ALPHA_OPAQUE);  
    SDL_RenderClear(renderer);

    SDL_FRect rect;
    rect.w = rect.h = CELL_SIZE;
    int i,j;
        
    for(i = 0; BOARD_HEIGHT > i; i++){
        for(j = 0; BOARD_WIDTH > j; j++){
            if(gamestate->board[i][j] == T_EMPTY) continue;
            rect.x = j*CELL_SIZE;
            rect.y = i*CELL_SIZE;
            if(gamestate->board[i][j] == T_PLAYER) SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
            else if(gamestate->board[i][j] == T_BLOCK) SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer,&rect);
        }
    }
    
    SDL_Color player_color = gamestate->player.tetromino.color;
    SDL_SetRenderDrawColor(renderer, player_color.r, player_color.g, player_color.b, SDL_ALPHA_OPAQUE);
    rect.x = gamestate->player.x*CELL_SIZE;
    rect.y = gamestate->player.y*CELL_SIZE;
    SDL_RenderFillRect(renderer,&rect);
    for(i=0; 3 > i; ++i){
        rect.x = (gamestate->player.x+gamestate->player.tetromino.offset[0][i].x)*CELL_SIZE;
        rect.y = (gamestate->player.y+gamestate->player.tetromino.offset[0][i].y)*CELL_SIZE;
        SDL_RenderFillRect(renderer,&rect);
    }
    SDL_RenderPresent(renderer);
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    const Uint64 ticks = SDL_GetTicks();
    GameState *gamestate = appstate;
    draw_board(gamestate);
    //SDL_Log("%I64d\n", ticks);
    while((ticks - gamestate->last_tick) >= TICKRATE){
        SDL_Log("%d,%d\n", gamestate->player.tetromino.offset[0][0].x,gamestate->player.tetromino.offset[0][0].y);
        update_game(gamestate);
        gamestate->last_tick += TICKRATE;
    }
    
    return SDL_APP_CONTINUE;  
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
}
