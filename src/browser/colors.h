#ifndef COLORS_H_
#define COLORS_H_

#include <SDL2/SDL.h>

SDL_Color Black = {0, 0, 0};
SDL_Color White = {255, 255, 255};
SDL_Color Blue = {0,0,255};
SDL_Color Red = {255,0,0};
SDL_Color Green = {0,255,0};
SDL_Color Default = { 255, 255, 255, 0};
SDL_Color *colors[] = {&White, &Blue, &Red, &Green};

#endif