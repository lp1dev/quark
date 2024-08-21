#include <stdlib.h>
#include <stdio.h>
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



void    Element_add_attribute(Element *element, char *key, char *value) {
    NamedNodeMap_append(&element->attributes, key, value);
}

void    Element_add_style(Element *element, char *key, char *value) {
    NamedNodeMap_append(&element->style, key, value);
}

Node    *Element_get_last_style(Element *element) { // Required for the way lxb parses style w/ callbacks
    return NamedNodeMap_get_last(&element->style);
}

void    Element_print(Element *element) {
    printf("Element {\n");
    printf(" tag : %s,\n", element->tag);
    printf(" id : %s\n", element->id);
    printf(" innerText : %s\n", element->innerText);
    printf("}\n");
}