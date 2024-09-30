#include "interfaces.h"

Text_Texture *Text_Texture_Create(TTF_Font *font, char *text, SDL_Surface *surface, SDL_Rect *rect, SDL_Color color, int font_size, int x, int y, SDL_Renderer *renderer) {
    Text_Texture *texture;

    texture = malloc(sizeof(Text_Texture));

    texture->text = text;
    texture->size = font_size;
    texture->surface = surface;
    texture->texture = SDL_CreateTextureFromSurface(renderer, surface);
    texture->color = color;
    texture->rect = rect;
    texture->x = x;
    texture->y = y;
    return texture;
}

int Text_Texture_Compare(Text_Texture *texture, TTF_Font *font, char *text, SDL_Color color, int font_size, int x, int y) {
    if (texture->text != text) {
        return 0;
    } if (texture->size != font_size) {
        return 0;
    } if (texture->x != x) {
        return 0;
    } if (texture->y != y) {
        return 0;
    } if (texture->color.r != color.r) {
        return 0;
    } if (texture->color.g != color.g) {
        return 0;
    } if (texture->color.b != color.b) {
        return 0;
    } if (texture->color.a != color.a) {
        return 0;
    }
    return 1;
}
