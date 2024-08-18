#ifndef _CSS_H_
#define _CSS_H_

/*

css_property is a linked list

 */

typedef struct css_property css_property;

struct css_property
{
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

typedef struct css_color css_color;

struct css_color
{
    int r;
    int g;
    int b;
    int a;
};

typedef struct render_properties
{
    css_color background_color;
    css_color color;
    int font_size;
} render_properties;

typedef struct future_render future_render;

struct future_render
{
    SDL_Rect rect;
    SDL_Surface surface;
    render_properties *render_properties;
    css_property *style;
    int style_size;
    char *tag;
    char *innerText;
};

css_property *create_css_property(css_property *prev, char *name, char *str_value, int important, int primary);
css_color apply_style(SDL_Rect *rect, SDL_Rect *root_rect, lxb_html_element_t *el, css_property *style, render_properties *render_properties);
int unit_to_px(css_property * style);

#endif