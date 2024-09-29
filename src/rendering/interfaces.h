#ifndef _RENDERING_INTERFACES_H_
#define _RENDERING_INTERFACES_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct Text_Texture {
  SDL_Texture *texture;
  SDL_Surface *surface;
  SDL_Color   color;
  SDL_Rect    *rect;
  char        *text;
  int         size;
  int         x;
  int         y;
} Text_Texture;

Text_Texture *Text_Texture_Create(TTF_Font *font, char *text, SDL_Surface *surface, SDL_Rect *rect, SDL_Color color, int font_size, int x, int y, SDL_Renderer *renderer);
int Text_Texture_Compare(Text_Texture *texture, TTF_Font *font, char *text, SDL_Color color, int font_size, int x, int y);

#endif