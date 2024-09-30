#include "cache.h"

Cached_Texture *Cached_Texture_Create(SDL_Texture *texture, SDL_Rect *rect, SDL_Color *color, int x, int y) {
    Cached_Texture *cached;

    cached = malloc(sizeof(Cached_Texture));
    cached->next = NULL;
    cached->texture = texture;
    cached->rect = rect;
    cached->color = color;
    cached->x = x;
    cached->y = y;
    return cached;
}

Cached_Texture *Cached_Texture_get(Cached_Texture *head, SDL_Texture *texture, SDL_Rect *rect, SDL_Color *color, int x, int y) {
    Cached_Texture *tmp;
    Cached_Texture *new;
    Cached_Texture *last;

    tmp = head;
    last = NULL;
    while (tmp != NULL) {
        last = tmp;
        if (tmp->texture == texture) {
            if (tmp->rect == rect) {
                if (tmp->color == color) {
                    if (tmp->x == x) {
                        if (tmp->y == y) {
                            return tmp;
                        }
                    }
                }
            }
        }
        tmp = tmp->next;
    }
    new = Cached_Texture_Create(texture, rect, color, x, y);
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
