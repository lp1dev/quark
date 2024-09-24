#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "element.h"
#include "../config.h"


Element *Element_create() {
    Element *new;

    new = malloc(sizeof(Element));
    new->internal_id = rand();
    new->attributes.length = 0;
    new->attributes.first = NULL;
    new->style.length = 0;
    new->style.first = NULL;
    new->tag = NULL;
    new->id = NULL;
    new->children = NULL;
    new->parent = NULL;
    new->prev = NULL;
    new->next = NULL;
    new->type = "element";
    new->innerText = NULL;
    new->height = 0;
    new->width = 0;
    new->x = 0;
    new->y = 0;
    new->computed_height = 0;
    new->computed_width = 0;
    new->computed_x = 0;
    new->computed_y = 0;
    return new;
}

void    Element_append_child(Element *element, Element *child) {
    Element *last_child = element->children;

    if (last_child == NULL) {
        if (child != NULL) {
            element->children = child;
        }
        return;
    }
    while (last_child->children != NULL) {
        last_child = last_child->children;
    }
    if (child != NULL) {
        last_child->children = child;
    }
    free(last_child);
}

int     Element_children_length(Element *element) {
    Element *child;
    int i;
    
    i = 0;
    child = element->children;
    while (child != NULL) {
        child = child->children;
        i++;
    }
    //
    free(child);
    return i;
}

void    Element_add_attribute(Element *element, char *key, char *value) {
    NamedNodeMap_append(&element->attributes, key, value);
}

void    Element_add_style(Element *element, char *key, char *value) {
    NamedNodeMap_append(&element->style, key, value);
}

Node    *Element_get_last_style(Element *element) { // Required for the way lxb parses style w/ callbacks
    return NamedNodeMap_get_last(&element->style);
}

Node    *Element_get_style(Element *element, char *key) {
    return NamedNodeMap_get(&element->style, key);
}

void    Element_set_style(Element *element, char *key, char *value) {
    return NamedNodeMap_set(&element->style, key, value);
}

void    Element_set_attribute(Element *element, char *key, char *value) {
    return NamedNodeMap_set(&element->attributes, key, value);
}

Node     *Element_get_style_int(Element *element, char *name) {
    Node *new_node;
    Node *node;
    float value;

    node = Element_get_style(element, name);
    new_node = malloc(sizeof(Node));

    if (node != NULL) {
        process_style_numeric_value(node);

        if (strncmp(node->str_value, "px", 2) == 0) {
            return node;
        }
        else if (strncmp(node->str_value, "%", 1) == 0) {
            if (strncmp(name, "width", 5) == 0) {
                if (element->parent != NULL) {
                    value = (float) element->parent->width;
                } else {
                    value = (float) SCREEN_WIDTH;
                }
                new_node->int_value = (value / 100.0) * node->int_value;
            } else if (strncmp(name, "height", 6) == 0) {
                if (element->parent != NULL) {
                    value = (float) element->parent->height;
                } else {
                    value = (float) SCREEN_HEIGHT;
                }
                new_node->int_value = ((value / 100.0) * node->int_value);
            }
            new_node->key = name;
            new_node->str_value = node->str_value;
            return new_node;
        }
        else if (strncmp(node->str_value, "vh", 2) == 0) {
            node->int_value = (SCREEN_HEIGHT / 100) * node->int_value;
        }
        else if (strncmp(node->str_value, "vw", 2) == 0) {
            node->int_value = (SCREEN_WIDTH / 100) * node->int_value;
        }

    }
    //
    free(node);
    free(new_node);
    return NULL;
}

void    Element_draw_graph(Element *element, int depth) {
    char *tabs;
    Element *sibling;

    tabs = malloc(sizeof(char) * depth + 1);

   for (int i = 0; i < depth; i++) {
        tabs[i] = '\t';
    }

    tabs[depth] = '\0';
    sibling = element;

    while (sibling != NULL) {
        printf("%s----------\n", tabs);
        printf("%s[   %s   ]\n", tabs, sibling->tag);
        printf("%s[   %s   ]\n", tabs, sibling->id);
        printf("%s[   %s   ]\n", tabs, sibling->innerText);
        if (sibling->parent != NULL) {
            printf("%s[   %s   ]\n", tabs, sibling->parent->tag);
        }
        printf("%s----------\n", tabs);
        if (sibling->children != NULL) {
            Element_draw_graph(sibling->children, depth + 1);
        }
        sibling = sibling->next;
    }
    //
    free(sibling);
    free(tabs);
}

Element *Element_get_by_id(Element *element, char *id) {
    Element *tmp;
    Element *next;

    tmp = element;
    while (tmp != NULL) {
        if (tmp->id && strcmp(tmp->id, id) == 0) {
            return tmp;
        } else {
            next = tmp->next;
            while (next != NULL) {
                if (tmp->id && strcmp(tmp->id, id) == 0) { // Might be a bug, should maybe remplace tmp by next here!!
                    return tmp;// Or next ?
                }
                next = next->next;
            }
        }
        tmp = tmp->children;
    }
}

Element *Element_get_by_pos(Element *element, int x, int y) {
    Element *tmp;
    Element *next;
    Element *last_match;
    Element *children_match;

    next = element;
    last_match = NULL;
    while (next != NULL) {
        // printf("\tnext->tag %s\n", next->tag);
        if ((next->computed_x + next->computed_width) >= x && x > next->computed_x) {
            if ((next->computed_y + next->computed_height) >= y && y > next->computed_y) {
                last_match = next;
            }
        }
        if (next->children != NULL) {
            children_match = Element_get_by_pos(next->children, x, y);
            if (children_match != NULL) {
                return children_match;
            }
        }
        next = next->next;
    }
    return last_match;
}

void Element_delete(Element *element, int internal_id) {
    Element *tmp;
    Element *next;

    tmp = element;
    while (tmp != NULL) {
        if (tmp->internal_id == internal_id) {
            tmp->prev->next = tmp->next;
            free(tmp);
            return;
        }
        next = tmp->next;
        while (next != NULL) {
            if (next->internal_id == internal_id) {
                next->prev->next = next->next;
                free(next);
                return;
            }
            next = next->next;
        }
        tmp = tmp->children;
    }
}

Element *Element_get_by_internal_id(Element *element, int internal_id) {
    Element *tmp;
    Element *next;

    next = element;
    while (next != NULL) {
        if (next->internal_id == internal_id) {
            return next;
        } else if (next->children != NULL) {
            tmp = Element_get_by_internal_id(next->children, internal_id);
            if (tmp != NULL) {
                return tmp;
            }
        }
        next = next->next;
    }
    return NULL;
}

void process_style_numeric_value(Node *node) {
    char *buffer;
    int set_num_value;
    int j;

    if (node->int_value != -123456789) {
        return;
    }
    j = 0;
    set_num_value = 0;
    buffer = malloc(sizeof(char) * (strlen(node->str_value) + 1));
    for (int i = 0; node->str_value[i] != '\0'; i++) {
        if (node->str_value[i] < '0' || node->str_value[i] > '9') {
            if (!set_num_value) {
                buffer[i] = '\0';
                node->int_value = atoi(buffer);
                set_num_value = 1;
            }
            buffer[j] = node->str_value[i]; 
            j++;
        } else {
            buffer[i] = node->str_value[i];
        }
    }
    buffer[j] = '\0';
    node->str_value = buffer;
}

void    Element_print(Element *element) {
    printf("Element {\n");
    printf(" tag : %s,\n", element->tag);
    printf(" id : %s\n", element->id);
    printf(" innerText : %s\n", element->innerText);
    printf("}\n");
}
