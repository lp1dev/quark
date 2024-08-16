#include <ctype.h> 
#include <stdio.h>
#include <stdlib.h>
#include "css.h"

css_property *create_css_property(css_property *prev, char *name, char *str_value, int important, int primary) {
        css_property *prop = (css_property *) malloc(sizeof(css_property));
        prop->prev = prev;
        prop->name = name;
        prop->str_value = str_value;
        prop->important = important;
        prop->primary = primary;
        prop->next = NULL;
        prop->value_length = 0;
        return prop;
}