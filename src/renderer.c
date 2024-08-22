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
    SDL_Color text_color = {0, 0, 0};
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

    surfaceMessage = TTF_RenderText_Solid(font, text, text_color);
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
    }
    serialize_element(ctx, element);
    /* 
        I should definitely do attributes parsing *BEFORE* someone does a getElementById, todo fix
    // */
    // for (int i = 0; render_queue[i] != NULL; i++)
    // {
    //     attr = lxb_dom_element_first_attribute(&render_queue[i]->el->element);

    //     while (attr != NULL)
    //     {
    //         tmp = lxb_dom_attr_qualified_name(attr, &tmp_len); // Attribute name
    //         if (strcmp(tmp, "id") == 0) {
    //             tmp = lxb_dom_attr_value(attr, &tmp_len); // Attribute value
    //             render_queue[i]->id = (char *) tmp; // Setting up the ID since this is the first time we encounter it
    //             if (tmp != NULL && strcmp(tmp, id) == 0)
    //             {
    //                 duk_pop(ctx);
    //                 serialize_future_render(ctx, render_queue[i]);
    //                 // duk_push_string(ctx, render_queue[i]->tag);
    //             }
    //         }

    //         attr = lxb_dom_element_next_attribute(attr);
    //     }
    // }
    return (duk_ret_t)1;
}

/* 

static duk_ret_t update_element(duk_context *ctx)

    Update a JS future_render element according to JS changes

*/
static duk_ret_t update_element(duk_context *ctx) {
    future_render *item;
    static int internalId;
    SDL_Color color_buffer;
    int update_type;
    char *new_value_str;

    internalId = duk_get_number(ctx, 0);
    update_type = duk_get_number(ctx, 1);

    // for (int i = 0; render_queue[i] != NULL; i++) {
    //     if (render_queue[i]->internal_id == internalId) {
    //         item = render_queue[i];
    //     }
    // }
    // if (item != NULL) {
    //     switch (update_type) {
    //         case ID:
    //             new_value_str = (char *) duk_get_string(ctx, 2);
    //             item->id = new_value_str;
    //             break;
    //         case INNER_TEXT:
    //             new_value_str = (char *) duk_get_string(ctx, 2);
    //             item->innerText = new_value_str;
    //             break;
    //         case COLOR:
    //             new_value_str = (char *) duk_get_string(ctx, 2);
    //             item->render_properties.color = parse_color(new_value_str);
    //             printf("COLOR IS NOW r %i g %i b %i\n", item->render_properties.color.r, item->render_properties.color.g, item->render_properties.color.b);
    //     }
    // }
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
    duk_push_c_function(ctx, update_element, 3 /*nargs*/);
    duk_put_global_string(ctx, "c_updateElement");
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
    SDL_Delay(10000);
    // TODO fix JS and re-render after JS changes
    duk_destroy_heap(ctx);
}
