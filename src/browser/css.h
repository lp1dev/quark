#ifndef _CSS_H_
#define _CSS_H_


/*

css_property is a linked list

 */


typedef struct {
    int num_element; 
    int max_elements;
    int depth;
} future_render_properties;

typedef struct css_property css_property;

struct css_property {
    char *str_value;
    int int_value;
    float float_value;
    int value_length;
    char *unit;
    char *name;
    int important;
    int primary;
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

css_property *create_css_property(css_property *prev, char *name, char *str_value, int important, int primary);
SDL_Color apply_style(SDL_Rect *rect, SDL_Rect *root_rect, lxb_html_element_t *el, future_render *item, css_property *style);


#endif