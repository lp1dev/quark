#ifndef _RENDERING_INTERFACES_H_
#define _RENDERING_INTERFACES_H_

#include <SDL2/SDL.h>

typedef struct {
    int num_element; 
    int max_elements;
    int depth;
} future_render_properties;


/*

css_property is a linked list

 */

typedef struct css_property css_property;

struct css_property {
    char *value;
    char *name;
    bool important;
    css_property *next;
    css_property *prev;
};

typedef struct {
    SDL_Rect rect;
    SDL_Surface surface;
    future_render_properties *properties;
    SDL_Color color;
    css_property *style;
    int style_size;
    char *tag;
    char *innerText;
} future_render;

#endif