#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include "game.h"

extern char scancode_game_events[SDL_SCANCODE_COUNT];
extern char scancode_menu_events[SDL_SCANCODE_COUNT];

enum { T_MOVE_STILL, T_MOVE_RIGHT, T_MOVE_LEFT };
enum { ROT_DIR_CLOCKWISE = 1, ROT_DIR_COUNTERCLOCKWISE = -1 };

enum KeyCommandsGame : char {
    GAME_CMD_INVALID = -1,
    GAME_ROT_CW,
    GAME_ROT_CCW,
    GAME_LEFT,
    GAME_RIGHT,
    GAME_SOFTDROP,
    GAME_HARDDROP,
    GAME_HOLD,
    GAME_PAUSE,

#ifdef DEBUG
    GAME_DEBUG_RESET_GRAVITY,
    GAME_DEBUG_INCREASE_GRAVITY,
    GAME_DEBUG_DECREASE_GRAVITY,
#endif
    GAME_CMD_COUNT //number of commands for array bound reasons, never use
};
enum KeyCommandsMenu : char {
    MENU_CMD_INVALID = -1,
    MENU_UP,
    MENU_DOWN,
    MENU_LEFT,
    MENU_RIGHT,
    MENU_SELECT,
    MENU_BACK,
    MENU_CMD_COUNT //ditto
};

#define CMD_COUNT GAME_CMD_COUNT+MENU_CMD_COUNT

extern unsigned short game_event_codes[GAME_CMD_COUNT];
extern unsigned short menu_event_codes[MENU_CMD_COUNT];

void default_events();
void gameplay_event(GameState *gamestate, SDL_Event *event);
void gameover_event(GameState *gamestate, SDL_Event *event);
void pause_event(GameState *gamestate, SDL_Event *event);
void game_menu_event(GameState *gamestate, SDL_Event *event);
void sub_menu_event(GameState *gamestate, SDL_Event *event);

#endif
