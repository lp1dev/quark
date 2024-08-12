#ifndef _HELPERS_H_
#define _HELPERS_H_
#include <SDL2/SDL.h>


#define FAILED(...)                                                            \
    do {                                                                       \
        fprintf(stderr, __VA_ARGS__);                                          \
        fprintf(stderr, "\n");                                                 \
        exit(EXIT_FAILURE);                                                    \
    }                                                                          \
    while (0)


void list_nodes(char *parent, lxb_dom_node_t *node);
char *get_node_text(lxb_dom_node_t* node);
char *get_tag_name(lxb_dom_node_t *node);
void print_rect(SDL_Rect rect);
int get_css_selectors(lxb_dom_node_t *node);

#endif