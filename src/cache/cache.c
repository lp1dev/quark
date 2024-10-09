#include "cache.h"

Cached_Texture *Cached_Texture_Create(char *name, SDL_Renderer *renderer, SDL_Surface *surface, SDL_Rect *rect, SDL_Color *color, int x, int y) {
    Cached_Texture *cached;

    printf("DEBUG:: Created new texture\n");

    cached = malloc(sizeof(Cached_Texture));
    cached->next = NULL;
    cached->name = name;
    cached->texture = SDL_CreateTextureFromSurface(renderer, surface);
    cached->rect = rect;
    cached->surface = surface;
    cached->color = color;
    cached->x = x;
    cached->y = y;
    return cached;
}

Cached_Texture *Cached_Texture_get(char *name, Cached_Texture *head, SDL_Renderer *renderer, SDL_Surface *surface, SDL_Rect *rect, SDL_Color *color, int x, int y) {
    Cached_Texture *tmp;
    Cached_Texture *new;
    Cached_Texture *last;

    tmp = head;
    last = NULL;
    while (tmp != NULL) {
        last = tmp;
        if (tmp->rect->w == rect->w) {
            if (tmp->rect->h == rect->h) {
                if (tmp->rect->x == rect->x) {    
                    if (tmp->x == x) {
                        if (tmp->y == y) {
                            if (tmp->name == name) {
                                return tmp;
                            }
                        }
                    }
                }
            }
        }
        tmp = tmp->next;
    }
    new = Cached_Texture_Create(name, renderer, surface, rect, color, x, y);
    if (last != NULL) {
        last->next = new;        
    }
    return new;
}

Cached_Font    *Cached_Font_Create(char *font_name, int font_size) {
    Cached_Font *cache;

    printf("DEBUG:::Creating new font! %s\n", font_name);
    cache = malloc(sizeof(Cached_Font));
    cache->next = NULL;
    cache->font = TTF_OpenFont(font_name, font_size);
    cache->font_name = font_name;
    cache->font_size = font_size;
    if (cache->font == NULL) {
        printf("Error: Loading font file %s failed\n", font_name);
    }
    return cache;
}

Cached_Font    *Cached_Font_get(Cached_Font *head, char *font_name, int font_size) {
    Cached_Font *tmp;
    Cached_Font *last;
    Cached_Font *new;

    tmp = head;
    last = NULL;
    while (tmp != NULL) {
        last = tmp;
        if (tmp->font_name == font_name) {
            if (tmp->font_size == font_size) {
                return tmp;
            }
        }
        tmp = tmp->next;
    }

    new = Cached_Font_Create(font_name, font_size);
    if (last != NULL) {
        last->next = new;
    }
    return new;
}
