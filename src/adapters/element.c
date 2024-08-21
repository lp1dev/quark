#include "element.h"
#include "../helpers.h"

/*
int style_callback(char *data, void *ctx)

    callback used by data->serialize for each item.
    I would have prefered an iterative process, but that's what
    lexbor does internally
*/
static lxb_status_t
style_callback(const lxb_char_t *data, size_t len, void *ctx)
{
    Node *node;
    int value_length;
    char *data_str;
    char *tmp;

    node = (Node *) ctx;
    data_str = (char *) data;

    if (is_empty(data_str)) {
        return LXB_STATUS_OK;
    }

    tmp = malloc(sizeof(char) * (len + value_length + 1));
    snprintf(tmp, len + value_length, "%s%.*s", node->str_value, (int) len, data_str);
    node->str_value = tmp;
    return LXB_STATUS_OK;

}

static lxb_status_t style_walk(lxb_html_element_t *html_element, const lxb_css_rule_declaration_t *declr,
           void *ctx, lxb_css_selector_specificity_t spec, bool is_weak) {

    const lxb_css_entry_data_t *data;
    Node *new_node;
    Element *element = (Element *) ctx;

    data = lxb_css_property_by_id(declr->type);
    new_node = Node_create(data->name, ""); // We set the str_value to ""
    data->serialize(declr->u.user, style_callback, (void *) new_node);
    //TODO also add the important bool
    //     declr->important
    // And those :
    //     printf("    Specificity (priority): %d %d %d %d %d\n",
    //        lxb_css_selector_sp_i(spec), lxb_css_selector_sp_s(spec),
    //        lxb_css_selector_sp_a(spec), lxb_css_selector_sp_b(spec),
    //        lxb_css_selector_sp_c(spec));

    //      printf("        Important: %d\n", lxb_css_selector_sp_i(spec));
    //      printf("        From Style Attribute: %d\n", lxb_css_selector_sp_s(spec));
    //      printf("        A: %d\n", lxb_css_selector_sp_a(spec));
    //      printf("        B: %d\n", lxb_css_selector_sp_b(spec));
    //      printf("        C: %d\n", lxb_css_selector_sp_c(spec));
    NamedNodeMap_append_node(&element->style, new_node);
    return LXB_STATUS_OK;

}

void parse_style(Element *element, lxb_html_element_t *html_element) {
    lxb_status_t status;

    if (html_element->style != NULL) {
        status = lxb_html_element_style_walk(html_element, style_walk, element, true);
    }
}

void parse_attributes(Element *element, lxb_dom_element_t *lxb_element) {
    lxb_dom_attr_t *attr;
    lxb_char_t *tmp_key, *tmp_val;
    size_t tmp_len;

    attr = lxb_dom_element_first_attribute(lxb_element);
        while (attr != NULL)
        {
            tmp_key = (lxb_char_t *) lxb_dom_attr_qualified_name(attr, &tmp_len); // Attribute name
            tmp_val = (lxb_char_t *) lxb_dom_attr_value(attr, &tmp_len); // Attribute value
            // printf("Attribute %s: %s\n", tmp_key, tmp_val);
            Element_add_attribute(element, tmp_key, tmp_val);
            attr = lxb_dom_element_next_attribute(attr);
        }
}

Element *walk_and_create_elements(Element *parent, lxb_dom_node_t *root_node) {
    lxb_dom_node_t *node;
    lxb_html_element_t *html_element;
    Element *first_element;
    Element *el_buffer;
    char *str_buffer;

    node = root_node;
    el_buffer = NULL;
    str_buffer = NULL;

    // printf("Parent is :\n");
    // Element_print(parent);

    while (node != NULL) {
        // We get the lexbor element from the current node
        html_element = lxb_html_interface_element(node);
        if (node->type == LXB_DOM_NODE_TYPE_ELEMENT) {
            printf("Element Node is %s\n", get_tag_name(node));

            if (el_buffer != NULL) {
                el_buffer->next = Element_create();
                el_buffer = el_buffer->next;
            } else {
                el_buffer = Element_create();
                first_element = el_buffer;
            }
            el_buffer->tag = get_tag_name(node);
            parse_attributes(el_buffer, (lxb_dom_element_t *) html_element);
            parse_style(el_buffer, html_element);
            Element_append_child(parent, el_buffer);
            // Element_print(el_buffer);
            // Node_print(el_buffer->attributes.first);
            // Node_print(el_buffer->style.first);
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
    return first_element;
}

Element *parse_lxb_body(lxb_dom_node_t *body) {
    Element *document;
    document = Element_create();
    document->tag = "document";
    return walk_and_create_elements(document, body);
}