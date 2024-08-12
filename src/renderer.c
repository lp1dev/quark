#include <lexbor/html/interfaces/document.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include "helpers.h"

SDL_Renderer  * gRenderer = NULL;
TTF_Font *font = NULL;
SDL_Color Black = {0, 0, 0};
SDL_Color Blue = {0,0,255};
SDL_Color Red = {255,0,0};
SDL_Color Green = {0,255,0};
SDL_Color *colors[] = {&Blue, &Red, &Green};

enum {
  SCREEN_WIDTH  = 960,
  SCREEN_HEIGHT = 544
};

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

    font = TTF_OpenFont("/usr/share/fonts/opentype/Sans.ttf", 24);

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


void render_text(char *text) {
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text, Black); 

    SDL_Texture* Message = SDL_CreateTextureFromSurface(gRenderer, surfaceMessage);
    SDL_Rect Message_rect; //create a rect
    Message_rect.x = 0;  //controls the rect's x coordinate     
    Message_rect.y = 0; // controls the rect's y coordinte
    Message_rect.w = 100; // controls the width of the rect
    Message_rect.h = 100; // controls the height of the rect

    SDL_RenderCopy(gRenderer, Message, NULL, &Message_rect);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}

void render_element(lxb_dom_node_t *node, SDL_Rect root_rect, SDL_Color *color, int num_element, int max_elements) {
    printf("Rendering an element\n");
    SDL_Rect elem_rect;
    elem_rect.x = root_rect.x;
    elem_rect.y = ((root_rect.h / max_elements) * num_element) + root_rect.y;
    elem_rect.w = root_rect.w; // Elements take all available width by default
    elem_rect.h = (root_rect.h / max_elements);
    printf("Rect {%i, %i, %i, %i}", elem_rect.x, elem_rect.y, elem_rect.w, elem_rect.h);
    SDL_SetRenderDrawColor(gRenderer, color->r, color->g, color->b, color->a);
    SDL_RenderFillRect(gRenderer, &elem_rect);
}

void render_body(lxb_dom_node_t *body) {
    lxb_dom_node_t *node = body->first_child;
    lxb_dom_node_t ** rootNodes;
    int containers = 0;
    int i = 0;
    SDL_SetRenderDrawColor( gRenderer, 255,255,255,255);
    SDL_Rect rect = { 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20};
    SDL_RenderFillRect( gRenderer, &rect);
    // Rendering white background

    while (node != NULL) {
        char *tag_name = lxb_dom_element_qualified_name(lxb_dom_interface_element(node), NULL);
        printf("- Root node [%i] %i : %s\n", node->type, i, tag_name);
        if (node->type == LXB_DOM_NODE_TYPE_ELEMENT) {
            containers++;
        }
        node = node->next;
        i++;
    }
    i = 0;
    printf("There is %i containers\n", containers);
    while (i < containers) {
        render_element(node, rect, colors[i], i, containers);
        i+= 1;
    }

}

void render_document(lxb_html_document_t *document) {
    render_body((lxb_dom_node_t*)document->body);
    list_nodes("document", (lxb_dom_node_t*)document->body);
    render_text("TESTTEXT");
    //
    SDL_RenderPresent( gRenderer );
    SDL_Delay(4000);
}
