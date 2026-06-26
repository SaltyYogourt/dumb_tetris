#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include "game.h"

enum { T_MOVE_STILL, T_MOVE_RIGHT, T_MOVE_LEFT };
enum { ROT_DIR_CLOCKWISE = 1, ROT_DIR_COUNTERCLOCKWISE = -1 };

enum KeyCommandsGame : int {
    GAME_ROT_CW,
    GAME_ROT_CCW,
    GAME_LEFT,
    GAME_RIGHT,
    GAME_SOFTDROP,
    GAME_HARDDROP,
    GAME_HOLD,
    GAME_PAUSE,

    GAME_DEBUG_RESET_GRAVITY,
    GAME_DEBUG_INCREASE_GRAVITY,
    GAME_DEBUG_DECREASE_GRAVITY,
};
enum KeyCommandsMenu : int {
    MENU_UP,
    MENU_DOWN,
    MENU_LEFT,
    MENU_RIGHT,
    MENU_SELECT,
    MENU_BACK,
};

void default_events();
void gameplay_event(GameState *gamestate, SDL_Event *event);
void gameover_event(GameState *gamestate, SDL_Event *event);
void pause_event(GameState *gamestate, SDL_Event *event);
void game_menu_event(GameState *gamestate, SDL_Event *event);
void sub_menu_event(GameState *gamestate, SDL_Event *event);

#endif
