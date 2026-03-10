#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

#include <game.h>

typedef struct {
    //Vector2 position;
    UIElements *parent;
    Vector2 position;
    Vector2 offset;
    Vector2 anchor;
}UIElement;

#endif
