#include <stdlib.h>
#include <stdio.h>
#include "node.h"

Node *Node_create(char *key, char* value) {
    Node *node;

    node = malloc(sizeof(Node));
    node->key = key;
    node->str_value = value;
    node->int_value = -1;
    node->next = NULL;
    return node;
}

void Node_print(Node *node) {
    Node *tmp;

    tmp = node;
    while (tmp != NULL) {
        printf("Node{\n");
        printf("\t%s : %s,\n", tmp->key, tmp->str_value);
        printf("\tnext : %p\n", tmp->next);
        printf("}\n");
        tmp = tmp->next;
    }
}