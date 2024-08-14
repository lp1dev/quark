#include <lexbor/html/interfaces/document.h>
#include <lexbor/html/interfaces/element.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include "helpers.h"
#include "browser/config.h"
#include "browser/colors.h"
#include "rendering/interfaces.h"

SDL_Renderer  * gRenderer = NULL;
TTF_Font *font = NULL;

static int SELECTED_COLOR = 0;
static future_render *item_buffer;

static int QUEUE_LENGTH = 0;
future_render *render_queue[255];

static lxb_status_t
callback(const lxb_char_t *data, size_t len, void *ctx)
{
    printf("IN CALLBACK %.*s\n", (int) len, (const char *) data);

    return LXB_STATUS_OK;
}


static lxb_status_t synchronous_serialize(const lxb_css_rule_declaration_t* declaration) {
        lxb_status_t status;
    const lxb_css_entry_data_t *data, *undata;
    const lxb_css_property__undef_t *undef;
    const lxb_css_property__custom_t *custom;

    static const lxb_char_t cl_str[] = ": ";
    static const lxb_char_t imp_str[] = " !important";

    data = lxb_css_property_by_id(declaration->type);
    if (data == NULL) {
        return LXB_STATUS_ERROR_NOT_EXISTS;
    }

    printf("Data name is %s, declaration type is %i\n", data->name, declaration->type);
    if (declaration->important) {
        printf("Declaration is important\n");
    } else {
        printf("Declaration is not important\n");
    }
    printf("Data contents are %s\n", declaration->u.user);

    if (declaration->type) { // 0 is LXB_CSS_PROPERTY__UNDEF

    }
    // return LXB_STATUS_OK;

    if (declaration->type == LXB_CSS_PROPERTY__UNDEF) {
        undef = declaration->u.undef;

        if (undef->type > LXB_CSS_PROPERTY__CUSTOM) {
            undata = lxb_css_property_by_id(undef->type);
            if (undata == NULL) {
                return LXB_STATUS_ERROR_NOT_EXISTS;
            }

            // printf("css attribute name[%s], len=[%i]\n", undata->name, undata->length);
        }
    }
    else if (declaration->type == LXB_CSS_PROPERTY__CUSTOM) {
        custom = declaration->u.custom;

        // printf("css attribute name[%s], len=[%i]\n", custom->name.data, custom->name.length);
    }
    else {
        // printf("css attribute name[%s], len=[%i]\n", data->name, data->length);
    }

    // status = data->serialize(declaration->u.user, cb, ctx);
    // if (status != LXB_STATUS_OK) {
        // return status;
    // }

    if (declaration->important && declaration->type != LXB_CSS_PROPERTY__UNDEF) {
        // printf("Val4[%s], len=[%i]\n", imp_str, (sizeof(imp_str) - 1));
    }

    return LXB_STATUS_OK;
}

static lxb_status_t
style_walk(lxb_html_element_t *element, const lxb_css_rule_declaration_t *declr,
        void *ctx, lxb_css_selector_specificity_t spec, bool is_weak) {
    static lxb_status_t status;

    status = synchronous_serialize(declr);
    // status = lxb_css_rule_declaration_serialize(declr, callback, NULL);
    if (status != LXB_STATUS_OK) {
        return EXIT_FAILURE;
    }

    printf("\n");

    printf("    Name: ");

    status = lxb_css_property_serialize_name(declr->u.user, declr->type,
                                             callback, NULL);
    if (status != LXB_STATUS_OK) {
        return EXIT_FAILURE;
    }

    printf("\n    Value: ");

    // Test 
    const lxb_css_entry_data_t *data;

    data = lxb_css_property_by_id(declr->type);
    if (data == NULL) {
        return LXB_STATUS_ERROR_NOT_EXISTS;
    }

    return data->serialize(declr->u.user, callback, NULL);
    // 

    printf("\n    Primary: %s\n", (is_weak) ? "false" : "true");
    printf("    Specificity (priority): %d %d %d %d %d\n",
           lxb_css_selector_sp_i(spec), lxb_css_selector_sp_s(spec),
           lxb_css_selector_sp_a(spec), lxb_css_selector_sp_b(spec),
           lxb_css_selector_sp_c(spec));

    printf("        Important: %d\n", lxb_css_selector_sp_i(spec));
    printf("        From Style Attribute: %d\n", lxb_css_selector_sp_s(spec));
    printf("        A: %d\n", lxb_css_selector_sp_a(spec));
    printf("        B: %d\n", lxb_css_selector_sp_b(spec));
    printf("        C: %d\n", lxb_css_selector_sp_c(spec));

    printf("\n");

    return LXB_STATUS_OK;
}

int graph_init()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
      printf("Failed to initialize the SDL2 library\n");
      return -1;
    }

    if(TTF_Init() < 0) {
        printf("TTF_Init error : %s\n", TTF_GetError());
        return -2;
    }

    font = TTF_OpenFont("/usr/share/fonts/opentype/Sans.ttf", DEFAULT_FONT_SIZE);

    if (font == NULL) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        return -3;
    }

    SDL_Window *window = SDL_CreateWindow("Browser",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          0);

    if(!window)
    {
      printf("Failed to create window");
        return -4;
    }

    if ((gRenderer = SDL_CreateRenderer( window, -1, 0)) == NULL)
      return -5;

    SDL_Surface *window_surface = SDL_GetWindowSurface(window);

    if(!window_surface)
    {
      printf("Failed to get the surface from the window\n");
      return -6;
    }

    SDL_UpdateWindowSurface(window);
}


void render_text(char *text, SDL_Rect rect) {
    printf("Rendering text '%s'\n", text);
    print_rect(rect);
    rect.w = DEFAULT_FONT_SIZE * strlen(text);
    rect.h = DEFAULT_FONT_SIZE * 3;

    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text, Black); 

    SDL_Texture* Message = SDL_CreateTextureFromSurface(gRenderer, surfaceMessage);

    SDL_RenderCopy(gRenderer, Message, NULL, &rect);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}


void render_element(lxb_dom_node_t *node, SDL_Rect root_rect, SDL_Color *color, int num_element, int max_elements) {
    SDL_Rect elem_rect;
    elem_rect.x = root_rect.x;
    elem_rect.y = ((root_rect.h / max_elements) * num_element) + root_rect.y;
    elem_rect.w = root_rect.w; // Elements take all available width by default
    elem_rect.h = (root_rect.h / max_elements);
    printf("\t->Rect {%i, %i, %i, %i}\n", elem_rect.x, elem_rect.y, elem_rect.w, elem_rect.h);
    SDL_SetRenderDrawColor(gRenderer, color->r, color->g, color->b, color->a);
    SDL_RenderFillRect(gRenderer, &elem_rect);
}


void create_future_render_item(char *tag, SDL_Rect rect, int num_element, int max_elements, int depth, SDL_Color color, char * text) {
    future_render *item_buffer;
    item_buffer = malloc(sizeof(future_render));
    item_buffer->tag = tag;
    item_buffer->rect = rect;
    item_buffer->properties = malloc(sizeof(future_render_properties));
    item_buffer->properties->depth = depth;
    item_buffer->properties->num_element = num_element;
    item_buffer->properties->max_elements = max_elements;
    item_buffer->color = color;
    item_buffer->innerText = text;
    render_queue[QUEUE_LENGTH++] = item_buffer;
}

SDL_Rect render_single_node(lxb_dom_node_t* node, SDL_Rect root_rect, int num_element, int max_elements, int depth) {
    lxb_html_element_t *el;

    printf("Rendering a single node (depth:%i) [%i/%i] %s\n", depth, num_element, max_elements, get_tag_name(node));



    el = lxb_html_interface_element(node);
    printf("Created html element %p\n", el);
    //
    printf("El style is at %p\n", el->style);

    if (el != NULL && el->style != NULL && el->style->value != NULL) {
        lxb_status_t status = lxb_html_element_style_walk(el, style_walk, NULL, true);
    }
    //

    SDL_Rect elem_rect;
    elem_rect.x = root_rect.x;
    elem_rect.y = ((root_rect.h / max_elements) * num_element) + root_rect.y;
    elem_rect.w = root_rect.w; // Elements take all available width by default
    elem_rect.h = (root_rect.h / max_elements);

    SDL_Color *chosen_color = colors[SELECTED_COLOR++];

    if (SELECTED_COLOR > 3) {
        SELECTED_COLOR = 0;
    }

    printf("\t->Rect {%i, %i, %i, %i}\n", elem_rect.x, elem_rect.y, elem_rect.w, elem_rect.h);

    char * node_text = get_node_text(node);
    // Update for future rendering fix
    create_future_render_item(get_tag_name(node), elem_rect, num_element, max_elements, depth, *chosen_color, node_text);
    //

    // Actual rendering
    // SDL_SetRenderDrawColor(gRenderer, chosen_color->r, chosen_color->g, chosen_color->b, chosen_color->a);
    // SDL_RenderFillRect(gRenderer, &elem_rect);
    //

    return elem_rect;
}

void render_node_subnodes(lxb_dom_node_t* node, SDL_Rect rect, int depth) {
    int i = 0;
    int elements = 0;
    SDL_Rect last_rendered_rect;

    lxb_dom_node_t* root_node = node;

    while (node != NULL) {
        if (node->type == LXB_DOM_NODE_TYPE_ELEMENT) {
            elements++;
        }
        node = node->next;
    }

    node = root_node;

    while (node != NULL) {
        printf("Handling node %s\n", get_tag_name(node));


        if (node->type == LXB_DOM_NODE_TYPE_ELEMENT) {
            last_rendered_rect = render_single_node(node, rect, i, elements, depth);
            i++;
        }
        if (node->first_child != NULL) {
            render_node_subnodes(node->first_child, last_rendered_rect, depth + 1);
        }
        else {
            // char * node_text = get_node_text(node);
            // if (strlen(node_text) > 0) {
            //     render_text(node_text, rect);
            // }
        }
        node = node->next;
    }
}

void render_body(lxb_dom_node_t *body) {
    SDL_Rect rect = { 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20};
    SDL_RenderFillRect( gRenderer, &rect);
    render_node_subnodes(body, rect, 0);
    future_render * item = render_queue[0];
    // Rendering future_render items
    int i = 0;
    while (item != NULL) {
        printf("item->tag = %s\n", item->tag);
        printf("pointer to rect %p\n", item->rect);
        SDL_SetRenderDrawColor(gRenderer, item->color.r, item->color.g, item->color.b, item->color.a);
        SDL_RenderFillRect(gRenderer, &item->rect);
        if (item->innerText != NULL) {
            render_text(item->innerText, item->rect);
        }
        free(item);
        item = render_queue[++i];

    }
}

void render_document(lxb_html_document_t *document) {
    // get_css_selectors(document);
    render_body((lxb_dom_node_t*)document->body);

    SDL_RenderPresent( gRenderer );
    SDL_Delay(5000);
}
