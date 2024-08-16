#include <lexbor/html/interfaces/document.h>
#include <SDL2/SDL.h>
#include "rendering/interfaces.h"

static char *UNITS[] = {
  "pt",
  "px",
  "%",
  "vh",
  "vw"
};

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

bool is_numeric(char *string, int length) {
  for (int i = 0; i < length;i++) {
    if (string[i] < '0' || string[i] > '9') {
      return false;
    }
  }
  return true;
}

bool is_unit(char *string, int length) {
  for (int i = 0; UNITS[i]; i++) {
    if (strcmp(string, UNITS[i]) == 0) {
      return true;
    }
  }
  return false;
}

void add_value_str(css_property *property, const unsigned char * value, int len) {
  int i = 0;

  if (property->str_value == NULL) {
    return;
  }
  if (property->value_length <= 0) {
    property->str_value = (char *)value;
    property->str_value[len - 1] = '\0';
    property->value_length = len;

  } else {

    int total_length = property->value_length + len;
    char *new_str = malloc(sizeof(char) * (total_length + 1));
    
    while (i < property->value_length + len) {
      if (i < property->value_length) {
        new_str[i] = property->str_value[i]; 
      } else {
        new_str[i] = value[i - property->value_length];
      }
      i++;
    }
    new_str[i] = '\0';
    property->str_value = new_str;
    property->value_length += len;
  }
}

void print_style(css_property *style) {
  while (style != NULL) {
    printf("\nproperty {");
    printf("\n\tname:%s,", style->name);
    printf("\n\tvalue:%s,", style->str_value);
    if (style->unit != NULL) {
      printf("\n\tunit:%s,", style->unit);
    }
    printf("\n\tvalue_length: %i", style->value_length);
    printf("\n}\n");
    style = style->next;
  }
}

bool is_empty(char *string) {
  for (int i = 0; string[i] != NULL;i++) {
    if (string[i] != ' ' && string[i] != '\t' \
       && string[i] != '\n' && string[i] != '\r') {
        return false;
    }
  }
  return true;
}