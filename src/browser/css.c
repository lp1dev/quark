#include <ctype.h> 
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <lexbor/html/interfaces/element.h>
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

SDL_Color apply_style(SDL_Rect *rect, SDL_Rect *root_rect, lxb_html_element_t *el, future_render *item, css_property *style) {
        SDL_Color color = { 255, 255, 255 };

        while (style != NULL) {
                if (strcmp(style->name, "background-color") == 0) {
                        printf("BACKGROUND COLOR IS %s\n", style->str_value);
                } else if (strcmp(style->name, "margin") == 0) {
                        style->int_value = atoi(style->str_value);
                        printf("MARGIN IS %i\n", style->int_value);
                }
                style = style->next;
        }
        return color;
}