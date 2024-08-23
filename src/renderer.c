#include <lexbor/html/interfaces/document.h>
#include <lexbor/html/interfaces/element.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <ctype.h>
#include "browser/classes/element.h"
#include "helpers.h"
#include "browser/config.h"
#include "browser/colors.h"
#include "rendering/interfaces.h"
#include "renderer.h"
#include "browser/js.h"
#include "adapters/element.h"

SDL_Renderer *gRenderer = NULL;
Element *body;

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

    window = SDL_CreateWindow("Quark",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT,
                              0);

    if (window == NULL)
    {
        printf("Failed to create window\n");
        exit(-4);
    }

    if ((gRenderer = SDL_CreateRenderer(window, -1, 0)) == NULL)
    {
        exit(-5);
    }

    window_surface = SDL_GetWindowSurface(window);

    if (window_surface == NULL)
    {
        printf("Failed to get the surface from the window\n");
        exit(-6);
    }

    SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
    SDL_UpdateWindowSurface(window);
}

/*
void render_text(char* text, SDL_Rect rect)

    Rendering text element
*/
void render_text(Element *el, char *text)
{
    SDL_Surface *surfaceMessage;
    SDL_Texture *Message;
    SDL_Color sdl_color;
    Node *element_color;
    css_color text_color = {0, 0, 0, 255};
    SDL_Rect text_rect;
    TTF_Font *font;
    int font_size = DEFAULT_FONT_SIZE;


    printf("Rendering text '%s' with font size %i\n", el->innerText, font_size);

    font = TTF_OpenFont("/usr/share/fonts/opentype/Sans.ttf", font_size);
    if (font == NULL)
    {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        exit(-3);
    }

    text_rect.x = el->x;
    text_rect.y = el->y;
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
    //
    surfaceMessage = TTF_RenderText_Solid(font, text, sdl_color);
    Message = SDL_CreateTextureFromSurface(gRenderer, surfaceMessage);

    SDL_RenderCopy(gRenderer, Message, NULL, &text_rect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
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
    // duk_push_string(ctx, color_to_string(item->render_properties.color));
    // duk_put_prop_string(ctx, -2, "color");
    // duk_push_string(ctx, color_to_string(item->render_properties.background_color));
    // duk_put_prop_string(ctx, -2, "backgroundColor");
}

/*  */
static duk_ret_t get_element_style(duk_context *ctx) {
    Element *el;
    Node *node;
    int internal_id;

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
        duk_push_string(ctx, node->str_value);
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

    printf("In getElementById\n");
    id = (char *) duk_get_string(ctx, 0);
    element = Element_get_by_id(body, id);
    if (element != NULL) {
        printf("Found element %s with id %s\n", element->tag, id);
    } else {
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
    update_key = duk_get_string(ctx, 2);
    update_value = duk_get_string(ctx, 3);
    printf("update_element(%i, %i, %s, %s)\n", internal_id, update_type, update_key, update_value);

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
    }

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
}


void calculate_element_dimentions(Element *el) {
    Element *parent;
    Element *tmp;
    int siblings;
    int position;

    //
    parent = el->parent;
    tmp = NULL;
    siblings = 0;
    position = 0;
    //
    if (parent == NULL) {
        return;
    }

    tmp = parent->children;
    while (tmp != NULL) {
        if (tmp->internal_id != el->internal_id) {
            siblings++;
        } else {
            position = siblings;
        }
        tmp = tmp->next;
    }
    printf("Element has %i siblings and position %i\n", siblings, position);

    el->width = parent->width; // We take all of the available width by default;
    el->height = (parent->height / (siblings + 1));
    el->x = 0;
    el->y = parent->y + (parent->height / (siblings + 1)) * position;
}

void draw_element(Element *el) {
    SDL_Rect rect;
    css_color background_color = {255, 255, 255, 0};
    Node *node;

    calculate_element_dimentions(el);
    node = Element_get_style(el, "background-color");
    if (node != NULL) {
        printf("Background color is %s\n", node->str_value);
        background_color = parse_color(node->str_value);
    }
    SDL_SetRenderDrawColor(gRenderer, \
    background_color.r, \
    background_color.g, \
    background_color.b, \
    background_color.a);
    rect.x = el->x;
    rect.y = el->y;
    rect.w = el->width;
    rect.h = el->height;
    print_rect(rect);
    SDL_RenderFillRect(gRenderer, &rect);
    if (el->innerText && strlen(el->innerText) > 0) {
        render_text(el, el->innerText);
    }
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
        }
        if (tmp->children != NULL) {
            render(tmp->children, depth + 1, ctx);
        }
        tmp = tmp->next;
    }
}

/*
void render_document(html_document *document)

    Rendering the DOM document item
*/
void render_document(lxb_html_document_t *document) {
    //
    body = parse_lxb_body((lxb_dom_node_t *) document->body);

    body->height = SCREEN_HEIGHT;
    body->width = SCREEN_WIDTH;
    body->parent = NULL;

    duk_context *ctx = js_init();

    init_dom(ctx);

    Element_draw_graph(body, 0);

    render(body, 0, ctx);
    SDL_RenderPresent(gRenderer);
    SDL_Delay(5000);

    Element_draw_graph(body, 0);

    render(body, 0, ctx);
    SDL_RenderPresent(gRenderer);
    SDL_Delay(5000);
    // TODO fix JS and re-render after JS changes
    duk_destroy_heap(ctx);
}
