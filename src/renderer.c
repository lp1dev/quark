#include <lexbor/html/interfaces/document.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include "helpers.h"

SDL_Renderer  * gRenderer = NULL;
TTF_Font *font = NULL;
SDL_Color Black = {0, 0, 0};
SDL_Color White = {255, 255, 255};
SDL_Color Blue = {0,0,255};
SDL_Color Red = {255,0,0};
SDL_Color Green = {0,255,0};
SDL_Color *colors[] = {&White, &Blue, &Red, &Green};
int SELECTED_COLOR = 0;

enum {
  SCREEN_WIDTH  = 960,
  SCREEN_HEIGHT = 544,
  DEFAULT_FONT_SIZE=24
};

struct {
    SDL_Rect rect;

} future_render;

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

    SDL_Window *window = SDL_CreateWindow("SDL2 Window",
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
    rect.h = DEFAULT_FONT_SIZE * 2; // TODO update

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

SDL_Rect render_single_node(lxb_dom_node_t* node, SDL_Rect root_rect, int num_element, int max_elements, int depth) {
    printf("Rendering a single node (depth:%i) [%i/%i] %s\n", depth, num_element, max_elements, get_tag_name(node));

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
    SDL_SetRenderDrawColor(gRenderer, chosen_color->r, chosen_color->g, chosen_color->b, chosen_color->a);
    SDL_RenderFillRect(gRenderer, &elem_rect);
    return elem_rect;
}

void render_node_subnodes(lxb_dom_node_t* node, SDL_Rect rect, int depth) {
    int containers = 0;
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
            char * node_text = get_node_text(node);
            if (strlen(node_text) > 0) {
                render_text(node_text, rect);
            }
        }
        node = node->next;
    }
}

void render_body(lxb_dom_node_t *body) {
    SDL_Rect rect = { 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20};
    SDL_RenderFillRect( gRenderer, &rect);
    render_node_subnodes(body, rect, 0);
}

void render_document(lxb_html_document_t *document) {
    render_body((lxb_dom_node_t*)document->body);
    SDL_RenderPresent( gRenderer );
    SDL_Delay(5000);
}
