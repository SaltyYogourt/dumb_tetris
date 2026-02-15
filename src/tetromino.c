#include <SDL3/SDL.h>
#include "main.h"

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

    //memcpy( T.offset[0], (Point[]){ {-1,0}, {1,0}, {0,-1} }, sizeof(Point[3]) ); //rot 0
    //memcpy( T.offset[1], (Point[]){ {0,1}, {0,-1}, {1,0} }, sizeof(Point[3]) ); //rot 3
    //memcpy( T.offset[2], (Point[]){ {-1,0}, {1,0}, {0,1} }, sizeof(Point[3]) ); //rot 2
    //memcpy( T.offset[3], (Point[]){ {0,1}, {0,-1}, {-1,0} }, sizeof(Point[3]) ); //rot 1
    memcpy( T.offset[0], (Point[]){ {0,-1}, {1,-1}, {-1,-1} }, sizeof(Point[3]) ); //rot 0
    memcpy( T.offset[1], (Point[]){ {0,-1}, {0,-2}, {-1,-1} }, sizeof(Point[3]) ); //rot 3
    memcpy( T.offset[2], (Point[]){ {1,0}, {-1,0}, {0,-1} }, sizeof(Point[3]) ); //rot 0
    memcpy( T.offset[3], (Point[]){ {0,-1}, {0,-2}, {1,-1} }, sizeof(Point[3]) ); //rot 3
    t[0] = I; t[1] = O; t[2] = T;
}

void get_abs_offsets(Player *player, unsigned char rot, Point *points){
    //we should use the tetromino lol
    points[0].x = player->x;
    points[0].y = player->y;
    //unsigned char rot = player->rot;
    for(int i = 0; 3 > i; ++i){ // fill the values
        points[i+1].x = (player->x+player->tetromino->offset[rot][i].x);
        points[i+1].y = (player->y+player->tetromino->offset[rot][i].y);
    }
}

unsigned char check_collision(GameState *gamestate, int rot){
    Player *player = &gamestate->player;
    Point abs_points[4];
    //get_abs_offsets(player, player->rot, abs_points); 
    get_abs_offsets(player, rot, abs_points); 
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

        if(abs_points[i].x < BOARD_WIDTH && abs_points[i].x > 0 && abs_points[i].y < BOARD_HEIGHT) { //don't shoot ourselves in the foot indexing OOB
            //SDL_Log("%d,%d val: %d", abs_points[i].x, abs_points[i].y, gamestate->board[abs_points[i].y][abs_points[i].x]);
            int tile = gamestate->board[abs_points[i].y][abs_points[i].x];
            //if(gamestate->board[abs_points[i].y][abs_points[i].x] != 0)
            if(tile != 0)
                collision_flag |= T_BOUND_OVERLAP;
            }
        }

    return collision_flag;
}
