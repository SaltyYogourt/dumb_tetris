#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

#include "game.h"

typedef struct {
    float x;
    float y;
} Vector2;

typedef struct UIElement UIElement;

struct UIElement {
    //Vector2 position;
    UIElement *parent;
    Vector2 position;
    Vector2 offset;
    Vector2 anchor;
};


#endif
