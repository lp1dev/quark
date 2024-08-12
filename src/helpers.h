#ifndef _HELPERS_H_
#define _HELPERS_H_

void list_nodes(char *parent, lxb_dom_node_t *node);
char *get_node_text(lxb_dom_node_t* node);
char *get_tag_name(lxb_dom_node_t *node);
void print_rect(SDL_Rect rect);

#endif