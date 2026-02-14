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

enum { 
    T_BOUND_BELOW = ( 1 << 0 ),
    T_BOUND_RIGHT = ( 1 << 1 ), //whether we're checking if there's a block or a wall to our right, we're colliding to the right.
    T_BOUND_LEFT  = ( 1 << 2 )
};

enum { T_MOVE_STILL, T_MOVE_RIGHT, T_MOVE_LEFT };

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
    char rot;
    PieceData tetromino;
    //unsigned char tetrominoIdx;
    char move_x;
} Player;

typedef struct {
    unsigned char board[BOARD_HEIGHT][BOARD_WIDTH];
    Uint64 last_tick;
    PieceData piece_data[7];
    Player player;
} GameState;

void init_tetrominos(PieceData *t){
    PieceData I, O, T, J, L, S, Z; 

    I.color.r = 64; I.color.g = 64; I.color.b = 255;

    memcpy( I.offset[0], (Point[]){ {-1,0}, {1,0}, {2,0} }, sizeof(Point[3]) ); //rot 0
    memcpy( I.offset[1], (Point[]){ {0,-1}, {0,1}, {0,2} }, sizeof(Point[3]) ); //rot 1
    memcpy( I.offset[2], I.offset[0], sizeof(Point[3]) ); //rot 2
    memcpy( I.offset[3], I.offset[1], sizeof(Point[3]) ); //rot 3

    O.color.r = 255; O.color.g = 165; O.color.b = 0; 

    memcpy( O.offset[0], (Point[]){ {1,0}, {0,1}, {1,1} },  sizeof(Point[3]) );
    memcpy( O.offset[1], O.offset[0], sizeof(Point[3])); 
    memcpy( O.offset[2], O.offset[0], sizeof(Point[3]));
    memcpy( O.offset[3], O.offset[0], sizeof(Point[3])); //it's a fucking square.


    T.color.r = 224; T.color.b = 224; T.color.g = 0;

    memcpy( T.offset[0], (Point[]){ {-1,0}, {1,0}, {0,-1} }, sizeof(Point[3]) ); //rot 0
    memcpy( T.offset[1], (Point[]){ {0,1}, {0,-1}, {1,0} }, sizeof(Point[3]) ); //rot 3
    memcpy( T.offset[2], (Point[]){ {-1,0}, {1,0}, {0,1} }, sizeof(Point[3]) ); //rot 2
    memcpy( T.offset[3], (Point[]){ {0,1}, {0,-1}, {-1,0} }, sizeof(Point[3]) ); //rot 1
    t[0] = I; t[1] = O; t[2] = T;
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
    player->rot = 0;
    player->tetromino = gamestate->piece_data[2]; //lmao. should this data be duplicateD? just get it working for now. we can cast it to a pointer later.
    
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
    //TODO: EVENT HANDLING SHOULD NOT UPDATE THE GAMESTATE!
    //probably best to inform the game it has to do an action(s) on each tick/frame.
    //we should also avoid only polling for game inputs ever n ticks. that would be annoying to the player
    if (event->type == SDL_EVENT_KEY_DOWN){
        if (event->key.scancode == SDL_SCANCODE_D){
           //((GameState *)appstate)->player.x += 1;
           ((GameState *)appstate)->player.move_x = T_MOVE_RIGHT;
        }
        else if (event->key.scancode == SDL_SCANCODE_A){
           //((GameState *)appstate)->player.x -= 1;
           ((GameState *)appstate)->player.move_x = T_MOVE_LEFT;
        }
        else if (event->key.scancode == SDL_SCANCODE_W){
           if (++((GameState *)appstate)->player.rot > 3) ((GameState *)appstate)->player.rot = 0;
        }
        else if (event->key.scancode == SDL_SCANCODE_S){
           if (--((GameState *)appstate)->player.rot < 0) ((GameState *)appstate)->player.rot = 3;
        }
        SDL_Log("%d\n", ((GameState*)appstate)->player.rot);
    }
    return SDL_APP_CONTINUE;  
}

void get_abs_offsets(Player *player, Point *points){
    points[0].x = player->x;
    points[0].y = player->y;
    unsigned char rot = player->rot;
    for(int i = 0; 3 > i; ++i){ // fill the values
        points[i+1].x = (player->x+player->tetromino.offset[rot][i].x);
        points[i+1].y = (player->y+player->tetromino.offset[rot][i].y);
    }
}

unsigned char check_collision(GameState *gamestate){
    Player *player = &gamestate->player;
    Point abs_points[4];
    get_abs_offsets(player, abs_points); 
    unsigned char collision_flag = 0;
    for(int i = 0; 4 > i; ++i){

        if(abs_points[i].x+1 > BOARD_WIDTH-1)
            collision_flag |= T_BOUND_RIGHT;
        else if(gamestate->board[abs_points[i].y][abs_points[i].x+1] != 0)
            collision_flag |= T_BOUND_RIGHT;

        if(abs_points[i].x-1 < 0)
            collision_flag |= T_BOUND_LEFT;
        else if(gamestate->board[abs_points[i].y][abs_points[i].x-1] != 0)
            collision_flag |= T_BOUND_LEFT;

        if(abs_points[i].y+1 > BOARD_HEIGHT-1)
            collision_flag |= T_BOUND_BELOW;
        else if(gamestate->board[abs_points[i].y+1][abs_points[i].x] != 0)
            collision_flag |= T_BOUND_BELOW;
        }
    return collision_flag;
}

void update_game(GameState *gamestate)
{
    //move player
    Player *player = &gamestate->player;
    unsigned char *block;
    Point offsets[4];  
    get_abs_offsets(player, offsets); 
    int i,j;
    SDL_Log("Player Y: %d\n", player->y);
    for(i = 0; BOARD_HEIGHT > i; i++){
        for(j = 0; BOARD_WIDTH > j; j++)
        {
            block = &gamestate->board[i][j];
            if(*block == T_PLAYER) *block = T_EMPTY; //clear that shit out.
            //TODO: check for lines.
        }
    }
    
    //collision checking
    unsigned char collision = check_collision(gamestate);
    
    if( !(collision & T_BOUND_BELOW)) player->y++;

    //handle movement
    if ( gamestate->player.move_x == T_MOVE_RIGHT && !(collision & T_BOUND_RIGHT) )
        player->x++;
    else if ( gamestate->player.move_x == T_MOVE_LEFT && !(collision & T_BOUND_LEFT) )
        player->x--;

    gamestate->player.move_x = T_MOVE_STILL;
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
    Point points_to_draw[4];
    get_abs_offsets(&gamestate->player, points_to_draw);
    for(i=0; 4 > i; ++i){
        //SDL_Log("x: %d, y: %d\n", points_to_draw[i].x, points_to_draw[i].y);
        rect.x = points_to_draw[i].x*CELL_SIZE;
        rect.y = points_to_draw[i].y*CELL_SIZE;
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
        update_game(gamestate);
        gamestate->last_tick += TICKRATE;
    }
    
    return SDL_APP_CONTINUE;  
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
}
