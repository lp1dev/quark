#ifndef QUARK_ELEMENT_H_
#define QUARK_ELEMENT_H_
#include "node.h"

typedef struct Element Element;

struct Element {
    NamedNodeMap attributes;
    int  internal_id;
    NamedNodeMap style;
    int style_size;
    char *tag;
    char *id;
    char *type;
    Element *children;
    Element *parent;
    char *innerText;
};

Element *Element_create(void);
void    Element_print(Element *element);
void    Element_append_child(Element *element, Element *child);
void    Element_add_attribute(Element *element, char *key, char *value);

#endif /* QUARK_ELEMENT_H_ */