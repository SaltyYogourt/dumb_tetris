#include "tetromino.h"
#include <SDL3/SDL.h>

void init_tetrominos(PieceData *t){
    PieceData I, O, T, J, L, S, Z; 

    I.color.r = 64; I.color.g = 64; I.color.b = 255; I.color.a = 255;

    memcpy( I.offset[0], (Point[]){ {-1,0}, {0,0}, {1,0}, {2,0} }, sizeof(Point[4]) ); //rot 0
    memcpy( I.offset[1], (Point[]){ {0,-1}, {0,0}, {0,1}, {0,2} }, sizeof(Point[4]) ); //rot 1
    memcpy( I.offset[2], I.offset[0], sizeof(Point[4]) ); //rot 2
    memcpy( I.offset[3], I.offset[1], sizeof(Point[4]) ); //rot 3

    O.color.r = 255; O.color.g = 165; O.color.b = 0; O.color.a = 255;

    memcpy( O.offset[0], (Point[]){ {0,0}, {1,0}, {0,1}, {1,1} },  sizeof(Point[4]) );
    memcpy( O.offset[1], O.offset[0], sizeof(Point[4])); 
    memcpy( O.offset[2], O.offset[0], sizeof(Point[4]));
    memcpy( O.offset[3], O.offset[0], sizeof(Point[4])); //it's a fucking square.

    T.color.r = 224; T.color.b = 224; T.color.g = 0; T.color.a = 255;

    memcpy( T.offset[0], (Point[]){ {0,0},  {-1,0}, {1,0}, {0,1} }, sizeof(Point[4]) ); //rot 0
    memcpy( T.offset[1], (Point[]){ {0,-1}, {0,0}, {0,1}, {-1,0} }, sizeof(Point[4]) ); //rot 1
    memcpy( T.offset[2], (Point[]){ {1,1}, {0,0}, {-1,1}, {0,1} }, sizeof(Point[4]) ); //rot 2
    memcpy( T.offset[3], (Point[]){ {0,-1}, {0,0}, {0,1}, {1,0} }, sizeof(Point[4]) ); //rot 3

    L.color.r = 255; L.color.g = 180; L.color.b = 50; L.color.a = 255;

    memcpy( L.offset[0], (Point[]){ {-1,0},  {0,0}, {1,0}, {-1,1} }, sizeof(Point[4]) ); //rot 0
    memcpy( L.offset[1], (Point[]){ {-1,-1}, {0,-1}, {0,0}, {0,1} }, sizeof(Point[4]) ); //rot 1
    memcpy( L.offset[2], (Point[]){ {1,0}, {1,1}, {0,1}, {-1,1} }, sizeof(Point[4]) ); //rot 2
    memcpy( L.offset[3], (Point[]){ {0,-1}, {0,0}, {0,1}, {1,1} }, sizeof(Point[4]) ); //rot 3

    J.color.r = 0; J.color.g = 0; J.color.b = 156; J.color.a = 255;

    memcpy( J.offset[0], (Point[]){ {-1,0},  {0,0}, {1,0}, {1,1} }, sizeof(Point[4]) ); //rot 0
    memcpy( J.offset[1], (Point[]){ {-1,1}, {0,-1}, {0,0}, {0,1} }, sizeof(Point[4]) ); //rot 1
    memcpy( J.offset[2], (Point[]){ {-1,0}, {1,1}, {0,1}, {-1,1} }, sizeof(Point[4]) ); //rot 2
    memcpy( J.offset[3], (Point[]){ {0,-1}, {0,0}, {0,1}, {1,-1} }, sizeof(Point[4]) ); //rot 3

    S.color.r = 255; S.color.g = 50; S.color.b = 232; S.color.a = 255;

    memcpy( S.offset[0], (Point[]){ {0,1}, {-1,1}, {0,0}, {1,0} }, sizeof(Point[4]) ); //rot 0
    memcpy( S.offset[1], (Point[]){ {-1,-1}, {-1,0}, {0,0}, {0,1} }, sizeof(Point[4]) ); //rot 1
    memcpy( S.offset[2], S.offset[0], sizeof(Point[4]) ); //rot 2
    memcpy( S.offset[3], S.offset[1], sizeof(Point[4]) ); //rot 3

    Z.color.r = 78; Z.color.g = 255; Z.color.b = 50; Z.color.a = 255;

    memcpy( Z.offset[0], (Point[]){ {0,1}, {1,1}, {0,0}, {-1,0} }, sizeof(Point[4]) ); //rot 0
    memcpy( Z.offset[1], (Point[]){ {1,-1}, {1,0}, {0,0}, {0,1} }, sizeof(Point[4]) ); //rot 1
    memcpy( Z.offset[2], Z.offset[0], sizeof(Point[4]) ); //rot 2
    memcpy( Z.offset[3], Z.offset[1], sizeof(Point[4]) ); //rot 3

    t[T_I] = I; t[T_O] = O; t[T_T] = T; t[T_J] = J; t[T_L] = L; t[T_S] = S; t[T_Z] = Z;
}

int get_player_floor(Player *player, unsigned char (*board)[10]){
    int y = player->y;
    while(~check_collision(player->x, y, player->tetromino, board, player->rot) & T_BOUND_BELOW){
            y++;
    }
    return y;
}

void get_abs_offsetsp(Player *player, unsigned char rot, Point *points){
    get_abs_offsets(player->x, player->y, player->tetromino, rot, points); 
}

void get_abs_offsets(int x, int y, PieceData *tetromino, unsigned char rot, Point *points){
    //unsigned char rot = player->rot;
    for(int i = 0; 4 > i; ++i){ // fill the values
        points[i].x = (x+(tetromino->offset[rot][i].x));
        points[i].y = (y+(tetromino->offset[rot][i].y));
    }
}


/* we only run this to check if a rotation fails (check_collision returns |
 * T_BOUND_OVERLAP. we push player's x +1 (if not bounding wall), 
 * check if T_BOUND_OVERLAP, if yes we push x -2, check again,
 * and if both fail the rotation fails (return KICK_NOP).
 * we follow ARS rules like good little coders, so don't forget
 * to check if there's collision when i==1 on the inner array
 * (center column rule, rotation fails)
 */
unsigned char check_rotation(GameState *gamestate, char rot){
    if (!(check_collisiong1(gamestate, rot) & (T_BOUND_OVERLAP | T_BOUND_RIGHT_WALL | T_BOUND_LEFT_WALL))) return ROT_INPLACE;
    //DON'T SWAP THE ORDER OF CONDITIONS!!!
    //T_I FUCKS CENTER COLUMN FUNCTION UP
    if (gamestate->player.tetromino_id == T_I  || center_column_rule(gamestate, rot)) return ROT_NOP;

    Player *player = &gamestate->player;
    //sliiiide to the right
    if (!(check_collision(player->x+1, player->y, player->tetromino, gamestate->board, rot) & (T_BOUND_OVERLAP | T_BOUND_RIGHT_WALL))) return ROT_KICK_RIGHT;

    //sliiiiiiide to the left...?
    if (!(check_collision(player->x-1, player->y, player->tetromino, gamestate->board, rot) & (T_BOUND_OVERLAP | T_BOUND_LEFT_WALL))) return ROT_KICK_LEFT;

    //well shit.
    return ROT_NOP;
}

bool center_column_rule(GameState *gamestate, char rot){
    //if(!(check_collisiong1(gamestate, rot) & (T_BOUND_RIGHT_WALL | T_BOUND_LEFT_WALL))) return false;
    char x, y, i, j;
    Player *player = &gamestate->player;
    //in case we try to access this when we shouldn't. I piece will fuck our shit up and cause a stack smash/overflow
    if(player->tetromino_id == T_I) return true; 
    Point abs_points[4];
    char virtual_board[3][3];
    for(i = 0; i < 3; i++) SDL_memset(virtual_board[i], 0, 3);
    for(i = 0; i < 4; i++){
        virtual_board[(player->tetromino->offset[rot][i].y+1)][player->tetromino->offset[rot][i].x+1] = T_PLAYER;
    }
    get_abs_offsetsp(&gamestate->player, rot, abs_points);
    for (i = 0; 3 > i; ++i){
        for (j = 0; 3 > j; ++j){
            x = gamestate->player.x-1+j; 
            y = gamestate->player.y-1+i; 
            if (gamestate->board[y][x] != T_EMPTY && virtual_board[i][j] == T_PLAYER) {
                    if(j==1) return true;
                    return false;
            }
        }
    }
    return false;
}

unsigned char check_collisiong1(GameState *gamestate, int rot){
        return check_collision(gamestate->player.x, gamestate->player.y, gamestate->player.tetromino, gamestate->board, rot);
}

unsigned char check_collisiong2(GameState *gamestate){
        return check_collisiong1(gamestate, gamestate->player.rot);
}

//loadbearing function
unsigned char check_collision(int x, int y, PieceData *tetromino, unsigned char (*board)[10], int rot){
    Point abs_points[4];
    //get_abs_offsets(player, player->rot, abs_points); 
    get_abs_offsets(x, y, tetromino, rot, abs_points); 
    unsigned char collision_flag = 0;
    for(int i = 0; 4 > i; ++i){

        if(abs_points[i].y > BOARD_HEIGHT-1)
            collision_flag |= T_BOUND_FLOOR;
        else if(abs_points[i].y+1 > BOARD_HEIGHT-1)
            collision_flag |= T_BOUND_BELOW;
        else if(board[abs_points[i].y+1][abs_points[i].x] != 0)
            collision_flag |= T_BOUND_BELOW;

        if(abs_points[i].y < 0) continue;  //don't shoot ourselves in the foot indexing OOB
            
        if(abs_points[i].x > BOARD_WIDTH-1)
            collision_flag |= T_BOUND_RIGHT_WALL;
        if(abs_points[i].x+1 >= BOARD_WIDTH)
            collision_flag |= T_BOUND_RIGHT;
        else if(board[abs_points[i].y][abs_points[i].x+1] != 0)
            collision_flag |= T_BOUND_RIGHT;

        if(abs_points[i].x < 0)
            collision_flag |= T_BOUND_LEFT_WALL; 
        else if(abs_points[i].x-1 < 0)
            collision_flag |= T_BOUND_LEFT;
        else if(board[abs_points[i].y][abs_points[i].x-1] != 0)
            collision_flag |= T_BOUND_LEFT;

        if(abs_points[i].x < BOARD_WIDTH && abs_points[i].x >= 0 && abs_points[i].y < BOARD_HEIGHT && abs_points[i].y >= 0) { //don't shoot ourselves in the foot indexing OOB
            int tile = board[abs_points[i].y][abs_points[i].x];
            if(tile != 0)
                collision_flag |= T_BOUND_OVERLAP;
            }
        }

    return collision_flag;
}
