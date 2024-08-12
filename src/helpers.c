#include <lexbor/html/interfaces/document.h>
#include <SDL2/SDL.h>

struct item {
  int type;
  char *contents;
};

char *get_node_text(lxb_dom_node_t* node) {
    size_t len;
    return lxb_dom_node_text_content(node, &len);
}

char *get_tag_name(lxb_dom_node_t *node) {
    return lxb_dom_element_qualified_name(lxb_dom_interface_element(node), NULL);

}

void print_rect(SDL_Rect rect) {
    printf("\t->Rect {%i, %i, %i, %i}\n", rect.x, rect.y, rect.w, rect.h);

}

void list_nodes(char* parent, lxb_dom_node_t *node) {
    while (node != NULL) {

        printf("%s-> %s (%s)\n", parent, get_tag_name(node), get_node_text(node));
        if (node->first_child != NULL) {
            list_nodes(get_tag_name(node), node->first_child);
        }
        node = node->next;
    }
}