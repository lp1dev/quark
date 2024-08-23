#ifndef NODES_H_
#define NODES_H_

typedef struct Node Node;

struct Node {
    Node *next;
    Node *prev;
    char *key;
    char *str_value;
    int int_value;
};

typedef struct NamedNodeMap NamedNodeMap;

struct NamedNodeMap {
    Node *first;
    int length;
};

Node *Node_create(char *key, char* value);
Node *Node_create_empty();
void Node_print(Node *node);
void NamedNodeMap_append(NamedNodeMap *map, char* key, char *value);
void NamedNodeMap_append_node(NamedNodeMap *map, Node *new_node);
Node *NamedNodeMap_get_last(NamedNodeMap *map);
Node *NamedNodeMap_get(NamedNodeMap *map, char *key);
void NamedNodeMap_set(NamedNodeMap *map, char *key, char *value);

#endif /* NODES_H_ */