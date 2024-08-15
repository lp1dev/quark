#include <lexbor/html/interfaces/document.h>
#include <SDL2/SDL.h>
#include "rendering/interfaces.h"

struct item {
  int type;
  char *contents;
};

char *get_node_text(lxb_dom_node_t* node) {
    size_t len;
    return lxb_dom_node_text_content(node, &len);
}

const char *get_tag_name(lxb_dom_node_t *node) {
    return lxb_dom_element_qualified_name(lxb_dom_interface_element(node), NULL);

}

void print_rect(SDL_Rect rect) {
    printf("\t->Rect {%i, %i, %i, %i}\n", rect.x, rect.y, rect.w, rect.h);

}

void list_nodes(const char* parent, lxb_dom_node_t *node) {
  const char * tag_name;
  while (node != NULL) {
    
    printf("%s-> %s (%s)\n", parent, get_tag_name(node), get_node_text(node));
    if (node->first_child != NULL) {
      tag_name = get_tag_name(node);
      list_nodes(tag_name, node->first_child);
    }
    node = node->next;
  }
}

css_property *get_last_css_property(css_property *property) {
  if (property == NULL) {
    return NULL;
  }
  while (property->next != NULL) {
    property = property->next;
  }
  return property;
}

void print_property(css_property *property) {
  printf("p->Name: %s\n", property->name);
  printf("p->str_value: %s\n", property->str_value);
  printf("p->int_value: %i\n", property->int_value);
  printf("p->float_value: %f\n", property->float_value);
}
