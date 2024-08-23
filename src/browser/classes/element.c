#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "element.h"


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
    new->next = NULL;
    new->type = "element";
    new->innerText = NULL;
    new->height = 0;
    new->width = 0;
    new->x = 0;
    new->y = 0;
    return new;
}

void    Element_append_child(Element *element, Element *child) {
    Element *last_child = element->children;

    if (last_child == NULL) {
        element->children = child;
        return;
    }
    while (last_child->children != NULL) {
        last_child = last_child->children;
    }
    last_child->children = child;
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
        printf("%s----------\n", tabs);
        if (sibling->children != NULL) {
            Element_draw_graph(sibling->children, depth + 1);
        } else {
            return;
        }
        sibling = sibling->next;
    }
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
                if (tmp->id && strcmp(tmp->id, id) == 0) {
                    return tmp;
                }
                next = next->next;
            }
        }
        tmp = tmp->children;
    }
}

Element *Element_get_by_internal_id(Element *element, int internal_id) {
    Element *tmp;
    Element *next;

    tmp = element;
    while (tmp != NULL) {
        if (tmp->internal_id == internal_id) {
            return tmp;
        } else {
            next = tmp->next;
            while (next != NULL) {
                if (tmp->internal_id == internal_id) {
                    return tmp;
                }
                next = next->next;
            }
        }
        tmp = tmp->children;
    }
    return NULL;
}

void process_style_numeric_value(Node *node) {
    char *buffer;
    int set_num_value;
    int j;

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