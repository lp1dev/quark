#include "element.h"
#include "../helpers.h"

void parse_attributes(Element *element, lxb_dom_element_t *lxb_element) {
    lxb_dom_attr_t *attr;
    lxb_char_t *tmp_key, *tmp_val;
    size_t tmp_len;

    attr = lxb_dom_element_first_attribute(lxb_element);
        while (attr != NULL)
        {
            tmp_key = (lxb_char_t *) lxb_dom_attr_qualified_name(attr, &tmp_len); // Attribute name
            tmp_val = (lxb_char_t *) lxb_dom_attr_value(attr, &tmp_len); // Attribute value
            printf("Attribute %s: %s\n", tmp_key, tmp_val);
            Element_add_attribute(element, tmp_key, tmp_val);
            attr = lxb_dom_element_next_attribute(attr);
        }
}

void walk_and_create_elements(Element *parent, lxb_dom_node_t *root_node) {
    lxb_dom_node_t *node;
    lxb_html_element_t *html_element;
    Element *el_buffer;
    char *str_buffer;

    node = root_node;
    el_buffer = NULL;
    str_buffer = NULL;

    while (node != NULL) {
        // We get the lexbor element from the current node
        html_element = lxb_html_interface_element(node);
        // DEBUG
        printf("Node type is %i\n", node->type);

        if (node->type == LXB_DOM_NODE_TYPE_ELEMENT) {
            // If the current node is a lexbor Element type
            el_buffer = Element_create();
            el_buffer->tag = get_tag_name(node);
            parse_attributes(el_buffer, (lxb_dom_element_t *) html_element);
            Node_print(el_buffer->attributes.first);
            Element_append_child(parent, el_buffer);
            Element_print(el_buffer);
        }
        else if (node->type == LXB_DOM_NODE_TYPE_TEXT) {
            str_buffer = get_node_text(node);
            if (el_buffer != NULL && str_buffer != NULL && !is_empty(str_buffer)) {
                el_buffer->innerText = str_buffer;
            }
        }
        if (node->first_child != NULL && el_buffer != NULL) {
            walk_and_create_elements(el_buffer, node->first_child);
        }
        node = node->next;
    }
}

Element *parse_lxb_body(lxb_dom_node_t *body) {
    Element *body_element = Element_create();

    body_element->tag = "body";

    walk_and_create_elements(body_element, body);
}