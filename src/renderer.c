#include <lexbor/html/interfaces/document.h>
#include <lexbor/html/interfaces/element.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <ctype.h>
#include "helpers.h"
#include "browser/config.h"
#include "browser/colors.h"
#include "rendering/interfaces.h"

static int QUEUE_LENGTH = 0;
static future_render *item_buffer;

SDL_Renderer *gRenderer = NULL;
TTF_Font *font = NULL;
future_render *render_queue[255];

/*
int style_callback(char *data, void *ctx)

    callback used by data->serialize for each item.
    I would have prefered an iterative process, but that's what
    lexbor does internally
*/
static lxb_status_t
style_callback(const lxb_char_t *data, size_t len, void *ctx)
{
    lxb_status_t status;
    size_t c_length;
    css_property *last;

    last = get_last_css_property(item_buffer->style);

    char *data_str = (char *)data;
    printf("data_string = [%.*s]\n", (int)len, data_str);

    if (is_empty(data_str))
    {
        return LXB_STATUS_OK;
    }

    if (last->value_length == 0)
    {
        if (strncmp(last->name, data, len) != 0)
        {
            last->str_value = malloc(sizeof(char) * (len + 1));
            strncpy(last->str_value, data, len);
            last->str_value[len] = '\0';
            last->value_length = len;
            last->unit = NULL;
        }
    }
    else if (is_unit(data_str, len))
    {
        printf("\t[%.*s] is a unit\n", (int)len, data_str);
        last->unit = malloc(sizeof(char) * (len + 1));
        strncpy(last->unit, data_str, len);
        last->unit[len] = '\0';
    }
    else if (is_numeric(data_str, len))
    {
        add_value_str(last, data, len);
        printf("\t[%.*s] is a number\n", (int)len, data_str);
    }
    else
    {
        add_value_str(last, data, len);
        printf("\t[%.*s] was not identified\n", (int)len, data_str);
    }
    return LXB_STATUS_OK;
}

/*

int synchronous_serialize(const lxb_css_rule_declaration_t* declaration)

    Stores the data contained in the document's style in qwark's custom css_property structs.

*/
static lxb_status_t synchronous_serialize(const lxb_css_rule_declaration_t *declaration, bool is_weak)
{
    lxb_status_t status;
    css_property *last;
    const lxb_css_entry_data_t *data;

    data = lxb_css_property_by_id(declaration->type);
    if (data == NULL)
    {
        return LXB_STATUS_ERROR_NOT_EXISTS;
    }
    last = get_last_css_property(item_buffer->style);
    if (last == NULL)
    {
        item_buffer->style = create_css_property(NULL, data->name, NULL, declaration->important, !is_weak);
        item_buffer->style_size++;
    }
    else
    {
        last->next = create_css_property(last, data->name, NULL, declaration->important, !is_weak);
        item_buffer->style_size++;
    }
    return LXB_STATUS_OK;
}

/*
int style_walk()

    Iterating over style items and adding them to the element

*/
static lxb_status_t
style_walk(lxb_html_element_t *element, const lxb_css_rule_declaration_t *declr,
           void *ctx, lxb_css_selector_specificity_t spec, bool is_weak)
{
    static lxb_status_t status;
    const lxb_css_entry_data_t *data;

    status = synchronous_serialize(declr, is_weak);
    if (status != LXB_STATUS_OK)
    {
        return EXIT_FAILURE;
    }

    data = lxb_css_property_by_id(declr->type);
    if (data == NULL)
    {
        return LXB_STATUS_ERROR_NOT_EXISTS;
    }

    data->serialize(declr->u.user, style_callback, NULL);
    return LXB_STATUS_OK;

    // TODO add those params to my css_property as well
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
    font = TTF_OpenFont("/usr/share/fonts/opentype/Sans.ttf", DEFAULT_FONT_SIZE);

    if (font == NULL)
    {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        exit(-3);
    }

    window = SDL_CreateWindow("Qwark",
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

    SDL_UpdateWindowSurface(window);
}

/*
void render_text(char* text, SDL_Rect rect)

    Rendering text element
*/
void render_text(future_render *item)
{
    SDL_Surface *surfaceMessage;
    SDL_Texture *Message;
    SDL_Rect text_rect;

    print_style(item->style);

    printf("Rendering text '%s' with font size %i\n", item->innerText, item->font_size);
    // print_rect(rect);

    font = TTF_OpenFont("/usr/share/fonts/opentype/Sans.ttf", item->font_size);
    text_rect.x = item->rect.x;
    text_rect.y = item->rect.y;
    text_rect.w = item->font_size * strlen(item->innerText);
    text_rect.h = item->font_size * 3;

    surfaceMessage = TTF_RenderText_Solid(font, item->innerText, Black);
    Message = SDL_CreateTextureFromSurface(gRenderer, surfaceMessage);

    SDL_RenderCopy(gRenderer, Message, NULL, &text_rect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}

/*
void create_future_render_item()

    Create an item to add to the renderer_queue buffer
*/
void create_future_render_item(char *tag, SDL_Rect rect, int num_element, int max_elements, int depth)
{
    if (QUEUE_LENGTH >= 255)
    {
        printf("Rendering too many items! Exiting.\n");
        exit(-1);
    }
    item_buffer = malloc(sizeof(future_render));
    item_buffer->tag = tag;
    item_buffer->rect = rect;
    item_buffer->properties = malloc(sizeof(future_render_properties));
    item_buffer->properties->depth = depth;
    item_buffer->properties->num_element = num_element;
    item_buffer->properties->max_elements = max_elements;
    item_buffer->color = Default;
    item_buffer->innerText = NULL;
    item_buffer->style_size = 0;
    item_buffer->style = NULL;
    item_buffer->font_size = DEFAULT_FONT_SIZE;
    render_queue[QUEUE_LENGTH++] = item_buffer;
}

/*
SDL_Rect render_single_node(dom_node *node, SDL_Rect root_rect, int num_element, int max_elements, int depth)

    Render a single node.
*/
SDL_Rect render_single_node(lxb_dom_node_t *node, SDL_Rect root_rect, int num_element, int max_elements, int depth)
{
    lxb_html_element_t *el;
    SDL_Rect elem_rect;
    const lxb_css_rule_declaration_t *background_color;
    lxb_status_t status;

    printf("Rendering a single node (depth:%i) [%i/%i] %s\n", depth, num_element + 1, max_elements, get_tag_name(node));

    el = lxb_html_interface_element(node);
    printf("\tCreated html element %p\n", el);
    //

    printf("\tEl style is at %p\n", el->style);

    // Defining the Rect properties
    elem_rect.x = root_rect.x;
    elem_rect.y = ((root_rect.h / max_elements) * num_element) + root_rect.y;
    elem_rect.w = root_rect.w; // Elements take all available width by default
    elem_rect.h = (root_rect.h / max_elements);

    create_future_render_item(get_tag_name(node), elem_rect, num_element, max_elements, depth);

    if (el != NULL && el->style != NULL && el->style->value != NULL)
    {
        lxb_status_t status = lxb_html_element_style_walk(el, style_walk, NULL, true);
    }

    // apply_style(&elem_rect, &root_rect, el, item_buffer, item_buffer->style);

    return elem_rect;
}

/*
void render_node_subnodes(dom_node *node, SDL_Rect rect, int depth)

    Render a node and its subnodes recursively
*/

void render_node_subnodes(lxb_dom_node_t *node, SDL_Rect rect, int depth)
{
    int i;
    int elements;
    char *text_buffer;
    lxb_dom_node_t *root_node;
    SDL_Rect last_rect;

    i = 0;
    elements = 0;
    root_node = node;
    last_rect = rect;

    // Counting the number of elements in the current depth
    while (node != NULL)
    {
        if (node->type == LXB_DOM_NODE_TYPE_ELEMENT)
        {
            elements++;
        }
        node = node->next;
    }
    node = root_node;

    while (node != NULL)
    {
        lxb_html_element_t *html_element;

        html_element = lxb_html_interface_element(node);
        printf("Element qualified name %i\n", (int)html_element->element.qualified_name);
        printf("Node type is %i\n", node->type);
        printf("Tag name is %s\n", get_tag_name(node));
        printf("Number of elements = [%i/%i]\n", i, elements);

        // If it's an element, we add it to the render queue
        if (html_element->element.qualified_name == LXB_DOM_ATTR__UNDEF)
        { // ATTR undef aren't attributes but tags
            if (node->type == LXB_DOM_NODE_TYPE_ELEMENT)
            {
                last_rect = render_single_node(node, rect, i++, elements, depth + 1);
            }
        }
        // If it's a text, we add it to the current item's innerText
        else if (node->type == LXB_DOM_NODE_TYPE_TEXT)
        {
            printf("WE HAVE A TEXT ATTRIBUTE %s -> %s\n", get_tag_name(node), get_node_text(node));
            text_buffer = get_node_text(node);
            if (!is_empty(text_buffer))
            {
                printf("AND IT'S NOT EMPTY\n");
                item_buffer->innerText = text_buffer;
            }

        }
        else if (node->type == LXB_DOM_NODE_TYPE_ATTRIBUTE)
        {
            printf("WE HAVE AN ATTRIBUTE %s\n", get_tag_name(node));
        }
        // If our node has children, we render them
        if (node->first_child != NULL)
        {
            render_node_subnodes(node->first_child, last_rect, depth + 1);
        }
        node = node->next;
    }
}

/*
void render_body(dom_node *body)

    Rendering the document's body.
*/
void render_body(lxb_dom_node_t *body)
{
    SDL_Rect rect = {10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20};
    future_render *item;

    SDL_RenderFillRect(gRenderer, &rect);

    render_node_subnodes(body, rect, 0);

    item = render_queue[0];
    // Rendering all future_render items
    for (int i = 0; item != NULL; i++)
    {
        if (i >= 255)
        {
            printf("ERROR: TOO MANY ITEMS TO RENDER");
            exit(1);
        }
        if (item->tag == NULL) // Empty tags seem to happen sometimes
        {
            continue;
        }
        printf("Rendering %s\n", item->tag);
        print_rect(item->rect);
        SDL_SetRenderDrawColor(gRenderer, item->color.r, item->color.g, item->color.b, item->color.a);
        SDL_RenderFillRect(gRenderer, &item->rect);
        if (item->innerText != NULL)
        {
            printf("innerText is %p %s\n", item->innerText, item->innerText);
            render_text(item);
        }
        item = render_queue[i];
    }
}

/*
void render_document(html_document *document)

    Rendering the DOM document item
*/
void render_document(lxb_html_document_t *document)
{
    render_body((lxb_dom_node_t *)document->body);
    SDL_RenderPresent(gRenderer);
    SDL_Delay(5000);
}
