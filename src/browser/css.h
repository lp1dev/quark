#ifndef _CSS_H_
#define _CSS_H_


/*

css_property is a linked list

 */

typedef struct css_property css_property;

struct css_property {
    char *str_value;
    int int_value;
    float float_value;
    int value_length;
    char *unit;
    char *name;
    int important;
    int primary;
    css_property *next;
    css_property *prev;
};

css_property *create_css_property(css_property *prev, char *name, char *str_value, int important, int primary);


#endif