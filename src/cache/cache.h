#ifndef QUARK_CACHE_H_
#define QUARK_CACHE_H_
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct Cached_Texture Cached_Texture;

struct Cached_Texture {
    Cached_Texture  *next;
    SDL_Texture     *texture;
    SDL_Rect        *rect;
    SDL_Color       *color;
    int             x;
    int             y;
};

typedef struct Cached_Font Cached_Font;

struct Cached_Font {
    TTF_Font    *font;
    char        *font_name;
    int         font_size;
    Cached_Font *next;
};

Cached_Texture *Cached_Texture_Create(SDL_Texture *texture, SDL_Rect *rect, SDL_Color *color, int x, int y);
Cached_Texture *Cached_Texture_get(Cached_Texture *head, SDL_Texture *texture, SDL_Rect *rect, SDL_Color *color, int x, int y);
Cached_Font    *Cached_Font_Create(char *font_name, int font_size);
Cached_Font    *Cached_Font_get(Cached_Font *head, char *font_name, int font_size);
#endif