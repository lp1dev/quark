#ifndef QUARK_ADAPTERS_ELEMENT_H_
#define QUARK_ADAPTERS_ELEMENT_H_
#include <lexbor/html/interfaces/element.h>
#include "../browser/classes/element.h"

Element *parse_lxb_body(lxb_dom_node_t *body);

#endif