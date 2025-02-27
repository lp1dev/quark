#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "node.h"

Node *Node_create(char *key, char* value) {
    Node *node;

    node = malloc(sizeof(Node));
    node->key = malloc(sizeof(char) * (strlen(key) + 1));
    node->str_value = malloc(sizeof(char) * (strlen(value) + 1));
    // strncpy(node->key, key, strlen(key));
    // strncpy(node->str_value, value, strlen(value));
    node->key = key;
    node->str_value = value;
    node->int_value = -123456789;
    node->next = NULL;
    return node;
}

Node *Node_create_empty() {
    Node *node;

    node = malloc(sizeof(Node));
    node->key = NULL;
    node->str_value = NULL;
    node->int_value = -123456789;
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

void NamedNodeMap_append(NamedNodeMap *map, char* key, char *value) {
    Node *tmp;
    Node *last;

    // We seem to have a segfault when appending a new item

    if (map->length == 0) {
        map->first = Node_create(key, value);
        map->length = 1;
    } else {
        tmp = map->first;
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        last = tmp;
        tmp = Node_create(key, value);
        last->next = tmp;
        map->length++;
    }
}

void NamedNodeMap_delete(NamedNodeMap *map, char *key) {
    Node *tmp;

    tmp = map->first;
    if (strcmp(tmp->key, key) == 0) {
        free(map->first);
        map->first = NULL;
        map->length = 0;
    }
    while (tmp->next != NULL) {
        if (strcmp(tmp->next->key, key) == 0) {
            free(tmp->next);
            tmp->next = tmp->next->next;
            return;
        }
        tmp = tmp->next;
    }
}

void NamedNodeMap_append_node(NamedNodeMap *map, Node *new_node) {
    Node *tmp;
    Node *last;

    if (map->length == 0) {
        map->first = new_node;
        map->length = 1;
    } else {
        tmp = map->first;
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        last = tmp;
        tmp = new_node;
        last->next = tmp;
        map->length++;
    }
}

Node *NamedNodeMap_get_last(NamedNodeMap *map) {
    Node *tmp;

    if (map->length == 0) {
        return NULL;
    }
    tmp = map->first;
    while (tmp->next != NULL) {
        tmp = tmp->next;
    }
    return tmp;
}

Node *NamedNodeMap_get(NamedNodeMap *map, char *key) {
    Node *node;

    node = map->first;

    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

void NamedNodeMap_set(NamedNodeMap *map, char *key, char *value) {
    Node *node;

    node = map->first;

    while (node != NULL) {
        // printf("\tNode %s=%s\n", node->key, node->str_value);
        // printf("\tSearched key %s\n", key);

        if (strncmp(node->key, key, strlen(key)) == 0) {
            node->key = key;
            node->str_value = value;
            return;
        }
        node = node->next;
    }
    NamedNodeMap_append(map, key, value);
}