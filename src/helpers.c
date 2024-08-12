#include <lexbor/html/interfaces/document.h>

struct item {
  int type;
  char *contents;
};

void list_nodes(char* parent, lxb_dom_node_t *node) {
    while (node != NULL) {
        char *tag_name = lxb_dom_element_qualified_name(lxb_dom_interface_element(node), NULL);
        printf("%s-> %s\n", parent, tag_name);
        if (node->first_child != NULL) {
            list_nodes(tag_name, node->first_child);
        }
        node = node->next;
    }
}