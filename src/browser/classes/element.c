#include <stdlib.h>
#include <stdio.h>
#include "element.h"


Element *Element_create() {
    Element *new;

    new = malloc(sizeof(Element));
    new->internal_id = rand();
    new->attributes.length = 0;
    new->attributes.first = NULL;
    new->style_size = 0;
    new->tag = NULL;
    new->id = NULL;
    new->children = NULL;
    new->parent = NULL;
    new->type = "element";
    new->innerText = NULL;
    return new;
}

void    Element_append_child(Element *element, Element *child) {
    Element *last_child = element->children;

    while (last_child != NULL) {
        last_child = last_child->children;
    }
    last_child = child;
}

void    Element_add_attribute(Element *element, char *key, char *value) {
    Node *tmp;
    Node *last;

    if (element->attributes.length == 0) {
        element->attributes.first = Node_create(key, value);
        element->attributes.length = 1;
    } else {
        tmp = element->attributes.first;
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        last = tmp;
        tmp = Node_create(key, value);
        last->next = tmp;
        element->attributes.length++;
    }
}

void    Element_print(Element *element) {
    printf("Element {\n");
    printf(" tag : %s,\n", element->tag);
    printf(" id : %s\n", element->id);
    printf(" innerText : %s\n", element->innerText);
    printf("}\n");
}