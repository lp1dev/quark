#ifndef QUARK_ELEMENT_H_
#define QUARK_ELEMENT_H_
#include "node.h"
#include "css_properties.h"

typedef struct Element Element;

struct Element {
    int  internal_id;
    NamedNodeMap attributes;
    NamedNodeMap style;
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
void    Element_add_style(Element *element, char *key, char *value);
Node    *Element_get_last_style(Element *element);

#endif /* QUARK_ELEMENT_H_ */