#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <lexbor/html/interfaces/element.h>
#include "css.h"
#include "config.h"

css_property *create_css_property(css_property *prev, char *name, char *str_value, int important, int primary)
{
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

int unit_to_px(css_property *style)
{
        if (style->unit == NULL)
        {
                return atoi(style->str_value);
        }
        else if (strcmp(style->unit, "px") == 0)
        {
                return atoi(style->str_value);
        }
        else if (strcmp(style->unit, "em") == 0)
        { // TEMP values, to replace with real el, rem
                return atoi(style->str_value) * DEFAULT_FONT_SIZE;
        }
        else if (strcmp(style->unit, "rem") == 0)
        {
                return atoi(style->str_value) * DEFAULT_FONT_SIZE;
        }
}

css_color parse_color(char *color)
{
        css_color output = {255, 255, 255, 255};
        char *buffer;

        if (color[0] == '#')
        {
                if (strlen(color) < 7 || strlen(color) > 9)
                {
                        printf("Invalid hex color [%s]\n", color);
                        return output;
                }
                //
                buffer = malloc(sizeof(char) * 3);
                buffer[0] = color[1];
                buffer[1] = color[2];
                buffer[2] = '\0';
                //
                output.r = strtol(buffer, NULL, 16);
                buffer[0] = color[3];
                buffer[1] = color[4];
                output.g = strtol(buffer, NULL, 16);
                buffer[0] = color[5];
                buffer[1] = color[6];
                output.b = strtol(buffer, NULL, 16);
                if (strlen(color) == 9) {
                        buffer[0] = color[7];
                        buffer[1] = color[8];
                        output.a = strtol(buffer, NULL, 16);
                }
        }
        return output;
}

css_color apply_style(SDL_Rect *rect, SDL_Rect *root_rect, lxb_html_element_t *el, css_property *style, render_properties *render_properties)
{
        css_color color = {255, 255, 255, 0};

        while (style != NULL)
        {
                printf("item->style->NAME IS %s\n", style->name);
                if (!style->primary && !style->important) {
                        style = style->next;
                        continue;
                }
                if (strcmp(style->name, "background-color") == 0)
                {
                        render_properties->background_color = parse_color(style->str_value);
                }
                else if (strcmp(style->name, "color") == 0) 
                {
                        printf("Applying color %s\n", style->str_value);
                        render_properties->color = parse_color(style->str_value);
                }
                else if (strcmp(style->name, "margin") == 0)
                {    
                        style->int_value = unit_to_px(style);
                        // printf("MARGIN IS %i\n", style->int_value);
                }
                else if (strcmp(style->name, "font-size") == 0)
                {
                        style->int_value = unit_to_px(style);
                        render_properties->font_size = style->int_value;
                        printf("GOT FONT-SIZE %i\n", style->int_value);
                }
                style = style->next;
        }
        return color;
}