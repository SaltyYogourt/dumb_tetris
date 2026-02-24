#include "tetromino.h"
#include "main.h"
#include <SDL3/SDL.h>

void init_tetrominos(PieceData *t){
    PieceData I, O, T, J, L, S, Z; 

    I.color.r = 64; I.color.g = 64; I.color.b = 255;

    memcpy( I.offset[0], (Point[]){ {-1,0}, {0,0}, {1,0}, {2,0} }, sizeof(Point[4]) ); //rot 0
    memcpy( I.offset[1], (Point[]){ {0,-1}, {0,0}, {0,1}, {0,2} }, sizeof(Point[4]) ); //rot 1
    memcpy( I.offset[2], I.offset[0], sizeof(Point[4]) ); //rot 2
    memcpy( I.offset[3], I.offset[1], sizeof(Point[4]) ); //rot 3

    O.color.r = 255; O.color.g = 165; O.color.b = 0; 

    memcpy( O.offset[0], (Point[]){ {0,0}, {1,0}, {0,1}, {1,1} },  sizeof(Point[4]) );
    memcpy( O.offset[1], O.offset[0], sizeof(Point[4])); 
    memcpy( O.offset[2], O.offset[0], sizeof(Point[4]));
    memcpy( O.offset[3], O.offset[0], sizeof(Point[4])); //it's a fucking square.

    T.color.r = 224; T.color.b = 224; T.color.g = 0;

    memcpy( T.offset[0], (Point[]){ {0,0},  {-1,0}, {1,0}, {0,1} }, sizeof(Point[4]) ); //rot 0
    memcpy( T.offset[1], (Point[]){ {0,-1}, {0,0}, {0,1}, {-1,0} }, sizeof(Point[4]) ); //rot 1
    memcpy( T.offset[2], (Point[]){ {1,1}, {0,0}, {-1,1}, {0,1} }, sizeof(Point[4]) ); //rot 2
    memcpy( T.offset[3], (Point[]){ {0,-1}, {0,0}, {0,1}, {1,0} }, sizeof(Point[4]) ); //rot 3

    L.color.r = 255; L.color.g = 180; L.color.b = 50;

    memcpy( L.offset[0], (Point[]){ {-1,0},  {0,0}, {1,0}, {-1,1} }, sizeof(Point[4]) ); //rot 0
    memcpy( L.offset[1], (Point[]){ {-1,-1}, {0,-1}, {0,0}, {0,1} }, sizeof(Point[4]) ); //rot 1
    memcpy( L.offset[2], (Point[]){ {1,0}, {1,1}, {0,1}, {-1,1} }, sizeof(Point[4]) ); //rot 2
    memcpy( L.offset[3], (Point[]){ {0,-1}, {0,0}, {0,1}, {1,1} }, sizeof(Point[4]) ); //rot 3

    J.color.r = 0; J.color.g = 0; J.color.b = 156; 

    memcpy( J.offset[0], (Point[]){ {-1,0},  {0,0}, {1,0}, {1,1} }, sizeof(Point[4]) ); //rot 0
    memcpy( J.offset[1], (Point[]){ {-1,1}, {0,-1}, {0,0}, {0,1} }, sizeof(Point[4]) ); //rot 1
    memcpy( J.offset[2], (Point[]){ {-1,0}, {1,1}, {0,1}, {-1,1} }, sizeof(Point[4]) ); //rot 2
    memcpy( J.offset[3], (Point[]){ {0,-1}, {0,0}, {0,1}, {1,-1} }, sizeof(Point[4]) ); //rot 3

    S.color.r = 255; S.color.g = 50; S.color.b = 232;

    memcpy( S.offset[0], (Point[]){ {0,0}, {-1,0}, {0,-1}, {1,-1} }, sizeof(Point[4]) ); //rot 0
    memcpy( S.offset[1], (Point[]){ {-1,-1}, {-1,0}, {0,0}, {0,1} }, sizeof(Point[4]) ); //rot 1
    memcpy( S.offset[2], S.offset[0], sizeof(Point[4]) ); //rot 2
    memcpy( S.offset[3], S.offset[1], sizeof(Point[4]) ); //rot 3

    Z.color.r = 78; Z.color.g = 255; Z.color.b = 50;

    memcpy( Z.offset[0], (Point[]){ {0,0}, {1,0}, {0,-1}, {-1,-1} }, sizeof(Point[4]) ); //rot 0
    memcpy( Z.offset[1], (Point[]){ {1,-1}, {1,0}, {0,0}, {0,1} }, sizeof(Point[4]) ); //rot 1
    memcpy( Z.offset[2], Z.offset[0], sizeof(Point[4]) ); //rot 2
    memcpy( Z.offset[3], Z.offset[1], sizeof(Point[4]) ); //rot 3

    t[T_I] = I; t[T_O] = O; t[T_T] = T; t[T_J] = J; t[T_L] = L; t[T_S] = S; t[T_Z] = Z;
}

void get_abs_offsets(Player *player, unsigned char rot, Point *points){
    //unsigned char rot = player->rot;
    for(int i = 0; 4 > i; ++i){ // fill the values
        points[i].x = (player->x+player->tetromino->offset[rot][i].x);
        points[i].y = (player->y+player->tetromino->offset[rot][i].y);
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
    if (!(check_collision(gamestate, rot ^ T_BOUND_OVERLAP))) return ROT_INPLACE;
    if (center_column_rule(gamestate, rot) || gamestate->player.tetromino_id == T_I) return ROT_NOP;

    //sliiiide to the right
    gamestate->player.x++;
    if (!(check_collision(gamestate, rot) ^ (T_BOUND_OVERLAP | T_BOUND_RIGHT_WALL))) return ROT_KICK_RIGHT;

    //sliiiiiiide to the left...?
    gamestate->player.x -= 2;
    if (!(check_collision(gamestate, rot) ^ (T_BOUND_OVERLAP | T_BOUND_LEFT_WALL))) return ROT_KICK_LEFT;

    //well shit.
    gamestate->player.x++;
    return ROT_NOP;
}

bool center_column_rule(GameState *gamestate, char rot){
     if(!(check_collision(gamestate, rot) ^ (T_BOUND_RIGHT_WALL | T_BOUND_LEFT_WALL))) return false;
    char x, y;
    Point abs_points[4];
    get_abs_offsets(&gamestate->player, rot, abs_points);
    for (int i = 0; 3 > i; ++i){
        for (int j = 0; 3 > j; ++j){
            x = gamestate->player.x-1+j; 
            y = gamestate->player.y-1+i; 
            if (gamestate->board[y][x] != T_EMPTY && j == 1) {
                //this sucks.
                for (int k = 0; 4 > k; ++k) { 
                    if(abs_points[k].x == x && abs_points[k].y == y)
                        return true;
                }
            }
        }
    }
    return false;
}

//loadbearing function
unsigned char check_collision(GameState *gamestate, int rot){
    Player *player = &gamestate->player;
    Point abs_points[4];
    //get_abs_offsets(player, player->rot, abs_points); 
    get_abs_offsets(player, rot, abs_points); 
    unsigned char collision_flag = 0;
    for(int i = 0; 4 > i; ++i){

        if(abs_points[i].x > BOARD_WIDTH-1)
            collision_flag |= T_BOUND_RIGHT_WALL;
        else if(abs_points[i].x+1 > BOARD_WIDTH-1)
            collision_flag |= T_BOUND_RIGHT;
        else if(gamestate->board[abs_points[i].y][abs_points[i].x+1] != 0)
            collision_flag |= T_BOUND_RIGHT;

        if(abs_points[i].x < 0)
            collision_flag |= T_BOUND_LEFT_WALL; 
        else if(abs_points[i].x-1 < 0)
            collision_flag |= T_BOUND_LEFT;
        else if(gamestate->board[abs_points[i].y][abs_points[i].x-1] != 0)
            collision_flag |= T_BOUND_LEFT;

        if(abs_points[i].y > BOARD_HEIGHT-1)
            collision_flag |= T_BOUND_FLOOR;
        else if(abs_points[i].y+1 > BOARD_HEIGHT-1)
            collision_flag |= T_BOUND_BELOW;
        else if(gamestate->board[abs_points[i].y+1][abs_points[i].x] != 0)
            collision_flag |= T_BOUND_BELOW;

        if(abs_points[i].x < BOARD_WIDTH && abs_points[i].x > 0 && abs_points[i].y < BOARD_HEIGHT) { //don't shoot ourselves in the foot indexing OOB
            int tile = gamestate->board[abs_points[i].y][abs_points[i].x];
            if(tile != 0)
                collision_flag |= T_BOUND_OVERLAP;
            }
        }

    return collision_flag;
}
