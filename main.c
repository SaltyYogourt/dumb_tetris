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
    unsigned char tetrominoIdx;
    char move_x;
    bool move_y;
} Tetromino;

typedef struct {
    unsigned char board[BOARD_HEIGHT][BOARD_WIDTH];
    Uint64 last_tick;
    Tetromino player;
} GameState;

PieceData tetrominos[7] = { {{64,255,64}}, {{-1,0}}, {{1,0}}, {{2,0}} };

void init_tetrominos(PieceData *t){
    PieceData I, O, T, J, L, S, Z; 

    //declare I
    I.color.r = 64; I.color.b = 255; I.color.b = 64;
    //I offsets
    memcpy( I.offset[0], (Point[]){ {-1,0}, {1,0}, {2,0} }, sizeof(Point[3]) ); //rot 0
    memcpy( I.offset[1], (Point[]){ {0,-1}, {0,1}, {0,2} }, sizeof(Point[3]) ); //rot 1
    memcpy( I.offset[2], I.offset[0], sizeof(Point[3]) ); //rot 2
    memcpy( I.offset[3], I.offset[1], sizeof(Point[3]) ); //rot 3
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("NoTetris Prayge", "1.0", "com.example.notetris");
    
    GameState *gamestate = SDL_calloc(1, sizeof(GameState));
    //Character *chara = SDL_calloc(1, sizeof(Character));
    *appstate = gamestate;
    
    Tetromino *player = &gamestate->player;
    SDL_memset(gamestate->board, T_EMPTY, sizeof(gamestate->board[0][0])*BOARD_HEIGHT*BOARD_WIDTH);

    player->x = BOARD_WIDTH/2;
    player->y = 1;
    player->tetrominoIdx = T_T; //lmao
    
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
    Tetromino *player = &gamestate->player;
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


    for(i = 0; 4 > i; i++){ //a tetromino will never be anything but 4x4, right?
        for(j = 0; 4 > j; j++)
        {
            block = &gamestate->board[i+player->y][j+player->x];
            player_block = tetrominos[player->tetrominoIdx][i][j];
            //dirty hack so we don't darw over shit
            //making sure we don't render out of bounds
            //skipping any empty blocks so we don't even try to draw them.
            
            //check if there's shit below us or we're at the bottom, stop
            if (player_block == T_PLAYER && (19 < player->y+i+1 || gamestate->board[1+i+player->y][j+player->x] > 1)){
                player->move_y = false;
            }

            //check surroundings
            //if(){}

            if (0 > player->y+i || 19 < player->y+i || *block > 1 || player_block == T_EMPTY)  continue;
            //Boy I'm happy no one has to maintain this shit.
            //oh wait, I do.
            
            *block = player_block;
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
    SDL_RenderPresent(renderer);
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    const Uint64 ticks = SDL_GetTicks();
    GameState *gamestate = appstate;
    draw_board(gamestate);
    //SDL_Log("%I64d\n", ticks);
    while((ticks - gamestate->last_tick) >= TICKRATE){
        update_game(gamestate);
        gamestate->last_tick += TICKRATE;
    }
    
    return SDL_APP_CONTINUE;  
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
}
