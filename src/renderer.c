#include <lexbor/html/interfaces/document.h>
#include <lexbor/html/interfaces/element.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <ctype.h>
#include "browser/classes/element.h"
#include "browser/classes/interval.h"
#include "helpers.h"
#include "browser/config.h"
#include "browser/colors.h"
#include "rendering/interfaces.h"
#include "renderer.h"
#include "browser/js.h"
#include "adapters/element.h"

SDL_Renderer *gRenderer = NULL;
Element *body;
TTF_Font *font;
Interval *intervals[LIST_SIZE];
Text_Texture *text_textures[LIST_SIZE];

/*

int graph_init()

    Initializing the SDL graphical environment

*/
int graph_init()
{
    SDL_Window *window;
    SDL_Surface *window_surface;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Failed to initialize the SDL2 library\n");
        exit(-1);
    }

    int status = TTF_Init();
    if (status < 0)
    {
        printf("TTF_Init error : %s\n", TTF_GetError());
        exit(-2);
    }

    window = SDL_CreateWindow("Quark", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    

    if (window == NULL)
    {
        printf("Failed to create window\n");
        exit(-4);
    }

    // Vita specific
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles2");
	gRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    //

    if (gRenderer == NULL)
    {
        exit(-5);
    }

    font = TTF_OpenFont("Sans.ttf", DEFAULT_FONT_SIZE);
    if (font == NULL) {
        printf("Failed to load font\n");
        exit(-7);
    }

    SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
    // SDL_UpdateWindowSurface(window);
    text_textures[0] = NULL;
}

/*
void render_text(char* text, SDL_Rect rect)

    Rendering text element
*/
SDL_Rect render_text(Element *el, char *text)
{
    SDL_Surface *surfaceMessage;
    SDL_Texture *Message;
    SDL_Color sdl_color;
    Node *element_color;
    Node *element_font_size;
    Node *node;
    Text_Texture *text_texture;
    char *text_align;
    css_color text_color = {0, 0, 0, 255};
    SDL_Rect text_rect;
    int font_size = DEFAULT_FONT_SIZE;
    int i;


    text_texture = NULL;
    i = 0;

    node = Element_get_style_int(el, "font-size");
    if (node != NULL) {
        font_size = node->int_value;
    }

    text_rect.x = el->computed_x;
    text_rect.y = el->computed_y;
    text_rect.w = font_size * strlen(text);
    text_rect.h = font_size * 3;

    //
    element_color = Element_get_style(el, "color");
    if (element_color != NULL) {
        text_color = parse_color(element_color->str_value);
    }
    sdl_color.r = text_color.r;
    sdl_color.b = text_color.b;
    sdl_color.g = text_color.g;
    sdl_color.a = text_color.a;

    surfaceMessage = TTF_RenderUTF8_Blended_Wrapped(font, text, sdl_color, el->width);

    text_rect.w = surfaceMessage->w;
    text_rect.h = surfaceMessage->h;

    node = Element_get_style(el, "text-align");
    if (node != NULL) {
        if (strncmp(node->str_value, "center", 6) == 0) {
            text_rect.x = el->computed_x + (el->computed_width / 2) - (text_rect.w / 2);
        } else if (strncmp(node->str_value, "right", 5) == 0) {
            text_rect.x = el->computed_width - text_rect.w;
        } else if (strncmp(node->str_value, "left", 4) == 0) {
            text_rect.x = el->computed_x;
        }
        text_align = node->str_value;
    }
    //
    node = Element_get_style(el, "vertical-align");
    if (node != NULL) {
        if (strncmp(node->str_value, "middle", 6) == 0) {
            text_rect.y = el->computed_y + (el->height / 2) - (text_rect.h / 2);
        } else if (strncmp(node->str_value, "top", 5) == 0) {
            // This is the default behaviour
        } else if (strncmp(node->str_value, "bottom", 4) == 0) {
            text_rect.y = el->computed_y + el->computed_height - text_rect.h;
        }
    }
    //
    node = Element_get_style_int(el, "padding");
    if (node != NULL) {
        if (text_align != NULL) {
            if (strncmp(text_align, "center", 6) == 0) {
                text_rect.y += node->int_value;
            } else if (strncmp(text_align, "right", 5) == 0) {
                text_rect.x -= node->int_value;
            } else if (strncmp(text_align, "left", 4) == 0) {
                text_rect.x += node->int_value;
            }
        } else {
            text_rect.x += node->int_value;
            text_rect.y += node->int_value;
        }
    }
    //
    node = Element_get_style_int(el, "line-height");
    if (node != NULL) {
        text_rect.h = node->int_value;
    }

    text_texture = Text_Texture_Create(font, text, surfaceMessage, &text_rect, sdl_color, font_size, el->x, el->y, gRenderer);
    SDL_RenderCopy(gRenderer, text_texture->texture, NULL, &text_rect);
    SDL_FreeSurface(surfaceMessage);
    return text_rect;
}

/* 

void serialize_future_render(duk_context *ctx, future_render *item)

    Serialize a future_render object into a JS object for a duktape context

*/
void serialize_element(duk_context *ctx, Element *element) {
    duk_push_object(ctx);
    // Setting tagName
    duk_push_string(ctx, element->tag); // I'm conflicted between doing a upper_str here or in JS
    duk_put_prop_string(ctx, -2, "tagName");
    duk_push_string(ctx, element->id);
    duk_put_prop_string(ctx, -2, "id");
    duk_push_string(ctx, element->innerText);
    duk_put_prop_string(ctx, -2, "innerText");
    duk_push_int(ctx, element->internal_id);
    duk_put_prop_string(ctx, -2, "internalId");
    duk_push_int(ctx, element->x);
    duk_put_prop_string(ctx, -2, "x");
    duk_push_int(ctx, element->y);
    duk_put_prop_string(ctx, -2, "y");
    duk_push_int(ctx, element->width);
    duk_put_prop_string(ctx, -2, "width");
    duk_push_int(ctx, element->height);
    duk_put_prop_string(ctx, -2, "height");
}


static duk_ret_t get_element_attributes(duk_context *ctx) {
    Element *el;
    Node    *node;
    int     internal_id;

    internal_id = (int) duk_get_int(ctx, 0);
    el = Element_get_by_internal_id(body, internal_id);
    if (el == NULL) {
        printf("Invalid element retrieval by internal id %i\n", internal_id);
        exit(-1);
    }
    duk_push_object(ctx);
    node = el->attributes.first;
    duk_push_number(ctx, el->internal_id);
    duk_put_prop_string(ctx, -2, "internalId");
    while (node != NULL) {
        duk_push_string(ctx, node->str_value);
        duk_put_prop_string(ctx, -2, node->key);
        node = node->next;
    }
    return (duk_ret_t) 1;
}

/*  */
static duk_ret_t get_element_style(duk_context *ctx) {
    Element *el;
    Node    *node;
    char    *tmp;
    int     internal_id;
    int     size;

    internal_id = (int) duk_get_int(ctx, 0);
    el = Element_get_by_internal_id(body, internal_id);
    if (el == NULL) {
        printf("Invalid element retrieval by internal id %i\n", internal_id);
        exit(-1);
    }
    duk_push_object(ctx);
    node = el->style.first;
    duk_push_number(ctx, el->internal_id);
    duk_put_prop_string(ctx, -2, "internalId");
    while (node != NULL) {
        if (node->int_value != -123456789) { // This is the uninitialized value
            size = snprintf(NULL, 0 ,"%i%s\n", node->int_value, node->str_value);
            tmp = malloc((size + 1) * sizeof(char));
            snprintf(tmp, size + 1 ,"%i %s\n", node->int_value, node->str_value);
            duk_push_string(ctx, tmp);
            free(tmp);
        } else {
            duk_push_string(ctx, node->str_value);
        }
        duk_put_prop_string(ctx, -2, node->key);
        node = node->next;
    }
    return (duk_ret_t) 1;
}

/*

static duk_ret_t get_element_by_id(duk_context *ctx)

    Returns a JS parsed element

*/
static duk_ret_t get_element_by_id(duk_context *ctx)
{
    Element *element;
    lxb_dom_attr_t *attr;
    const lxb_char_t *tmp;
    size_t value_len, tmp_len;
    char *id;

    // printf("In getElementById\n");
    id = (char *) duk_get_string(ctx, 0);
    element = Element_get_by_id(body, id);
    if (element == NULL) {
        printf("#%s not found\n", id);
        return (duk_ret_t)0;
    }
    serialize_element(ctx, element);
    return (duk_ret_t)1;
}

/* 

static duk_ret_t update_element(duk_context *ctx)

    Update a JS future_render element according to JS changes

*/
static duk_ret_t update_element(duk_context *ctx) {
    Element *element;
    static int internal_id;
    SDL_Color color_buffer;
    int update_type;
    char *update_key;
    char *update_value;

    internal_id = duk_get_number(ctx, 0);
    element = Element_get_by_internal_id(body, internal_id);
    if (element == NULL) {
        printf("Error, element is NULL! %i\n", internal_id);
        return (duk_ret_t) 0;
    }
    update_type = duk_get_number(ctx, 1);
    update_key = (char *) duk_get_string(ctx, 2);
    update_value = (char *) duk_get_string(ctx, 3);

    switch (update_type) {
        case ID:
            element->id = update_value;
            break;
        case STYLE:
            Element_set_style(element, update_key, update_value);
            break;
        case INNER_TEXT:
            element->innerText = update_value;
            break;
        case ATTRIBUTES:
            Element_set_attribute(element, update_key, update_value);
            break;
    }

    return (duk_ret_t) 0;
}

void init_intervals() {
    memset(intervals, 0, LIST_SIZE);
    return;
}

void check_intervals(duk_context *ctx) {
    Interval *interval;
    int current_time;
    int i;

    current_time = SDL_GetTicks();
    i = 0;
    while (intervals[i] != NULL) {
        if (intervals[i]->timeout != -123456789) {
            if (current_time > (intervals[i]->start_time + intervals[i]->timeout)) {
                duk_get_global_string(ctx, "quark_executeInterval");
                duk_push_number(ctx, intervals[i]->id);
                duk_call(ctx, 1);
                intervals[i]->timeout = -123456789; // Temporary replacement for an actual delete of the interval
                free(intervals[i]);
            }
        } else if (intervals[i]->interval != -123456789) {
            if (current_time > (intervals[i]->start_time + intervals[i]->interval)) {
                duk_get_global_string(ctx, "quark_executeInterval");
                duk_push_number(ctx, intervals[i]->id);
                duk_call(ctx, 1);
                intervals[i]->start_time = current_time;
            }
        }
        i++;
    }
}

static duk_ret_t set_interval(duk_context *ctx) {
    Interval *interval_obj;
    int i;
    int id;
    int interval;
    int timeout;

    i  = 0;
    id = duk_get_number(ctx, 0);
    interval = duk_get_number(ctx, 1);
    timeout = duk_get_number(ctx, 2);
    interval_obj = Interval_create(id, interval, timeout, SDL_GetTicks());
    while (intervals[i] != NULL) {
        i++;
    }
    if (i >= LIST_SIZE) {
        printf("Too many intervals created, exiting\n");
        exit(-1);
    }
    intervals[i] = interval_obj;
    return (duk_ret_t) 0;
}

/* void init_dom

    Initializing dom objects in the JS context
*/
void init_dom(duk_context *ctx) {
    duk_push_global_object(ctx);
    duk_put_global_string(ctx, "quark"); // Creating a "document" global
    duk_push_c_function(ctx, get_element_by_id, 1 /*nargs*/);
    duk_put_global_string(ctx, "c_getElementById");
    duk_push_c_function(ctx, update_element, 4 /*nargs*/);
    duk_put_global_string(ctx, "c_updateElement");
    duk_push_c_function(ctx, get_element_style, 1);
    duk_put_global_string(ctx, "c_getElementStyle");
    duk_push_c_function(ctx, set_interval, 3);
    duk_put_global_string(ctx, "c_setInterval");
    duk_push_c_function(ctx, get_element_attributes, 1);
    duk_put_global_string(ctx, "c_getElementAttributes");

}

void compute_margin_padding(Element *el) {
    Node *node;

    node = Element_get_style_int(el, "margin");
    if (node != NULL) {
        el->computed_x += node->int_value;
        el->computed_y += node->int_value;
    }

    node = Element_get_style_int(el->parent, "padding");
    if (node != NULL) {
        el->computed_x += node->int_value;
        el->computed_y += node->int_value;
        el->computed_height -= (node->int_value * 2);
        el->computed_width -= (node->int_value * 2);
    }
}

void compute_element_dimensions_inline(Element *el) {
    Element *parent;
    Element *tmp;
    Node *node;
    int siblings;
    int vertical_space_left;
    int horizontal_space_left;
    int position;

    //
    parent = el->parent;
    tmp = NULL;
    siblings = 0;
    position = 0;
    //
    if (parent == NULL) {
        el->width = SCREEN_WIDTH;
        el->height = SCREEN_HEIGHT;
        return;
    }

    tmp = parent->children;
    while (tmp != NULL) {
        if (tmp->internal_id == el->internal_id) {
            position = siblings;
        }
        siblings++;
        tmp = tmp->next;
    }

    if (el->prev == NULL) {
        el->width = (parent->computed_width / (siblings));
        el->height = parent->computed_height;
        el->x = ((parent->computed_width / (siblings)) * position) + parent->computed_x;
        el->y = parent->computed_y;
    } else {
        horizontal_space_left = SCREEN_WIDTH - (el->prev->computed_x + el->prev->computed_width);
        vertical_space_left = SCREEN_HEIGHT - (el->prev->computed_y + el->prev->computed_height);
        el->width = horizontal_space_left / (siblings - 1) * position;
        el->height = parent->computed_height;
        el->x = el->prev->computed_x + el->prev->computed_width;
        el->y = parent->computed_y;
        if ((parent->computed_x + el->x + el->width) > parent->computed_width) {
            parent->computed_width = parent->computed_x + el->x + el->width;
        }
        // printf("%s{%i, %i, %i, %i}\n", el->tag, el->x, el->y, el->width, el->height);
    }

    node = Element_get_style_int(el, "height");
    if (node != NULL) {
        el->height = node->int_value;
    }
    node = Element_get_style_int(el, "width");
    if (node != NULL) {
        el->width = node->int_value;
    }

    // Applying computed properties
    el->computed_x = el->x;
    el->computed_y = el->y;
    el->computed_height = el->height;
    el->computed_width = el->width;
    //
    
    compute_margin_padding(el);
    free(tmp);
}

SDL_Rect compute_smallest_element_size(Element *el) {
    Element *child;
    Element *prev;
    Node *font_size;
    Node *node;
    int font_size_value;
    SDL_Rect smallest = {0, 0, 0, 0};
    SDL_Rect text_size = {0, 0, 0, 0};
    SDL_Rect children_size = {0, 0, 0, 0};
    
    //
    smallest.w = el->width;
    smallest.h = el->height;

    child = el->children;
    while (child != NULL) {
        children_size = compute_smallest_element_size(child);
        smallest.w = children_size.w;
        smallest.h = children_size.h;

        child = child->children;
    }

    font_size = Element_get_style_int(el, "font-size");
    if (font_size != NULL) {
        font_size_value = font_size->int_value;
    } else {
        font_size_value = DEFAULT_FONT_SIZE;
    }
    if (el->innerText != NULL) {
        text_size.w = (strlen(el->innerText) * font_size_value);
        text_size.w = font_size_value;
    }

    if (smallest.h > text_size.h && text_size.h > 0) {
        smallest.h = text_size.h;
    }
    if (smallest.w > text_size.w && text_size.w > 0) {
        smallest.w = text_size.w;
    }
    // node = Element_get_style_int(el, "padding");
    // if (node != NULL) {
    //     // There seems to be a bug in applying padding.
    //     smallest.w += node->int_value;
    //     smallest.h += node->int_value;
    // }
    return smallest;
}

void compute_element_dimensions(Element *el) {
    Element *parent;
    Element *tmp;
    Node *node;
    SDL_Rect smallest_size;
    int siblings;
    int vertical_space_left;
    int position;

    //
    parent = el->parent;
    tmp = NULL;
    siblings = 0;
    position = 0;
    smallest_size = compute_smallest_element_size(el);

    //
    if (parent == NULL) {
        el->width = SCREEN_WIDTH;
        el->height = SCREEN_HEIGHT;
        return;
    }

    tmp = parent->children;
    while (tmp != NULL) {
        if (tmp->internal_id == el->internal_id) {
            position = siblings;
        }
        siblings++;
        tmp = tmp->next;
    }

    if (el->prev == NULL) {    
        el->width = parent->computed_width; // By default an element will take all of the available width
        el->height = (parent->computed_height / siblings);

        if (smallest_size.w != 0) {
            el->width = smallest_size.w;
            // I should add padding here
        }
        if (smallest_size.h != 0) {
            el->height = smallest_size.h;
            // Here too
        }
        el->y = ((parent->computed_height / siblings) * position) + parent->computed_y;
        el->x = parent->computed_x;
    } else {
        vertical_space_left = SCREEN_HEIGHT - (el->prev->computed_y + el->prev->computed_height);
        el->width = parent->computed_width; // We take all of the available width by default;
        el->height = vertical_space_left / (siblings - 1) * position;
        if (smallest_size.w != 0) {
            el->width = smallest_size.w;
        }
        if (smallest_size.h != 0) {
            // Here too
            el->height = smallest_size.h;
        }

        el->x = parent->computed_x;
        el->y = el->prev->computed_y + el->prev->computed_height;
        if ((parent->computed_y + el->y + el->height) > parent->computed_height) {
            parent->computed_height = parent->computed_y + el->y + el->height;
        }
    }

    node = Element_get_style_int(el, "height");
    if (node != NULL) {
        el->height = node->int_value;
    }
    node = Element_get_style_int(el, "width");
    if (node != NULL) {
        el->width = node->int_value;
    }

    // Applying computed properties
    el->computed_x = el->x;
    el->computed_y = el->y;
    el->computed_height = el->height;
    el->computed_width = el->width;
    //
    compute_margin_padding(el);
    free(tmp);
}

void draw_element(Element *el) {
    SDL_Rect rect;
    css_color background_color = {255, 255, 255, 0};
    Node *node;
    int is_inline;

    node = NULL;
    is_inline = 0;
    if (el->parent != NULL) {
        node = Element_get_style(el->parent, "display");
    }
    if (node != NULL && \
        strncmp(node->str_value, "inline-block", 12) == 0) {
        compute_element_dimensions_inline(el);
    } else {
        compute_element_dimensions(el);
    }
    node = Element_get_style(el, "background-color");
    if (node != NULL) {
        background_color = parse_color(node->str_value);
    }
    node = Element_get_style(el, "border");
    if (node != NULL) {
        // TODO handle borders
    }

    SDL_SetRenderDrawColor(gRenderer, \
    background_color.r, \
    background_color.g, \
    background_color.b, \
    background_color.a);

    rect.x = el->computed_x;
    rect.y = el->computed_y;
    rect.w = el->computed_width;
    rect.h = el->computed_height;
    SDL_RenderFillRect(gRenderer, &rect);

    if (el->innerText && strlen(el->innerText) > 0) {
        render_text(el, el->innerText);
    }
    free(node);
}

/* void render (Element *body)

    Rendering the Element body
*/
void render(Element *parent, int depth, duk_context *ctx) {
    Element *tmp;
    int position;
    int siblings;

    tmp = parent;

    while (tmp != NULL) {
        draw_element(tmp);
        if (strncmp(tmp->tag, "script", 6) == 0) {
            duk_eval_string(ctx, tmp->innerText);
            Element_delete(parent, tmp->internal_id);
        }
        if (tmp->children != NULL) {
            render(tmp->children, depth + 1, ctx);
        }
        tmp = tmp->next;
    }
    free(tmp);
}

void handle_click(duk_context *ctx, int x, int y) {
    Element *el;

    // printf("User click on %i, %i\n", x, y);
    el = Element_get_by_pos(body, x, y);
    if (el != NULL) {
        duk_get_global_string(ctx, "quark_onClick");
        // duk_push_int(ctx, el->internal_id);
        serialize_element(ctx, el);
        duk_call(ctx, 1);
    } else {
        printf("Warning : Invalid element clicked? x=%i y=%i\n", x, y);
    }
    return;
}


/* 
void render_loop(dux_context *ctx)

    The main rendering loop

*/
void render_loop(duk_context *ctx) {
    SDL_Event event;
    int go_on;
    int timer;
    int i = 0;

    go_on = 1;
    timer = 0;

    while (go_on) {
        // Element_draw_graph(body, 0);
        render(body, 0, ctx);
        SDL_RenderPresent(gRenderer);
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                go_on = 0;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                handle_click(ctx, event.button.x, event.button.y);
            }
        }
        check_intervals(ctx);
        i++;
        // printf("TIMER %i\n", timer);
        timer += SDL_GetTicks();
        // SDL_Delay(1000);
    }
    return;
}

/*
void render_document(html_document *document)

    Rendering the DOM document item
*/
void render_document(lxb_html_document_t *document) {
    duk_context *ctx;
    //
    body = parse_lxb_body((lxb_dom_node_t *) document->body);
    body->height = SCREEN_HEIGHT;
    body->width = SCREEN_WIDTH;
    body->computed_height = SCREEN_HEIGHT;
    body->computed_width = SCREEN_WIDTH;
    body->parent = NULL;

    init_intervals();
    ctx = js_init();
    init_dom(ctx);
    render_loop(ctx);
    duk_destroy_heap(ctx);
    SDL_DestroyRenderer(gRenderer);
    SDL_Quit();
}
