#ifndef _RENDERING_INTERFACES_H_
#define _RENDERING_INTERFACES_H_

#include <SDL2/SDL.h>

typedef struct {
    int num_element; 
    int max_elements;
    int depth;
} future_render_properties;

typedef struct {
    char *value;
    char *name;
} css_property;

typedef struct {
    SDL_Rect rect;
    SDL_Surface surface;
    future_render_properties *properties;
    SDL_Color color;
    css_property *style;
    char *tag;
    char *innerText;
} future_render;

#endif