#ifndef QUARK_ELEMENT_H_
#define QUARK_ELEMENT_H_
#include "node.h"
#include "css_properties.h"

enum update_types {
    RECT,
    SURFACE,
    RENDER_PROPERTIES,
    ATTRIBUTES,
    STYLE,
    EL,
    INTERNAL_ID,
    TAG,
    ID,
    INNER_TEXT
};

typedef struct Element Element;

struct Element {
    int     internal_id;
    int     height;
    int     width;
    int     x;
    int     y;
    NamedNodeMap attributes;
    NamedNodeMap style;
    char    *tag;
    char    *id;
    char    *type;
    Element *prev;
    Element *next;
    Element *children;
    Element *parent;
    char    *innerText;
};

Element *Element_create(void);
void    Element_print(Element *element);
void    Element_append_child(Element *element, Element *child);
void    Element_add_attribute(Element *element, char *key, char *value);
void    Element_add_style(Element *element, char *key, char *value);
Node    *Element_get_last_style(Element *element);
Node    *Element_get_style(Element *element, char *key);
void    Element_set_style(Element *element, char *key, char *value);
void    Element_set_attribute(Element *element, char *key, char *value);
int     Element_children_length(Element *element);
void    Element_draw_graph(Element *element, int depth);
Element *Element_get_by_id(Element *element, char *id);
Element *Element_get_by_internal_id(Element *element, int internal_id);
Element *Element_get_by_pos(Element *element, int x, int y);
void    Element_delete(Element *element, int internal_id);
void    process_style_numeric_value(Node *node);

#endif /* QUARK_ELEMENT_H_ */