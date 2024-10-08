#ifndef _HELPERS_H_
#define _HELPERS_H_
#include <SDL2/SDL.h>
#include "browser/css.h"

#define FAILED(...)                                                            \
    do {                                                                       \
        fprintf(stderr, __VA_ARGS__);                                          \
        fprintf(stderr, "\n");                                                 \
        exit(EXIT_FAILURE);                                                    \
    }                                                                          \
    while (0)


void list_nodes(const char* parent, lxb_dom_node_t *node);
char *get_node_text(lxb_dom_node_t* node);
const char *get_tag_name(lxb_dom_node_t *node);
void print_rect(SDL_Rect rect);
int get_css_selectors(lxb_dom_node_t *node);
css_property *get_last_css_property(css_property *property);
void print_property(css_property *property);
bool is_numeric(char *string, int length);
bool is_unit(char *string, int length);
void add_value_str(css_property *property, const unsigned char *value, int len);
void print_style(css_property *style);
bool is_empty(char *string);
SDL_Color css_color_to_sdl(css_color *color);
char *upper_str(char *str);
char *load_file(char *filename);

#endif