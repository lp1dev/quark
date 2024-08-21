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
void Node_print(Node *node);

#endif /* NODES_H_ */