#include <ctype.h> 
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <lexbor/html/interfaces/element.h>
#include "css.h"
#include "config.h"

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

int unit_to_px(css_property *style) {
  if (style->unit == NULL) {
    return atoi(style->str_value);
  } else if (strcmp(style->unit, "px") == 0) {
    return atoi(style->str_value);
  } else if (strcmp(style->unit, "em") == 0) { // TEMP values, to replace with real el, rem
    return atoi(style->str_value) * DEFAULT_FONT_SIZE;
  } else if (strcmp(style->unit, "rem") == 0) {
    return atoi(style->str_value) * DEFAULT_FONT_SIZE;
  }
}

SDL_Color apply_style(SDL_Rect *rect, SDL_Rect *root_rect, lxb_html_element_t *el, future_render *item, css_property *style) {
        SDL_Color color = { 255, 255, 255 };

        while (style != NULL) {
                printf("STYLE->NAME IS %s\n", style->name);
                if (strcmp(style->name, "background-color") == 0) {
                        printf("BACKGROUND COLOR IS %s\n", style->str_value);
                } else if (strcmp(style->name, "margin") == 0) {
                        style->int_value = unit_to_px(style);
                        printf("MARGIN IS %i\n", style->int_value);
                } else if (strcmp(style->name, "font-size") == 0) {
                        style->int_value = unit_to_px(style);
                        item->font_size = style->int_value;
                        printf("GOT FONT-SIZE %i\n", style->int_value);
                }
                style = style->next;
        }
        return color;
}