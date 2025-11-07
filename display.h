#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include "chip8.h"
#include "SDL.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} sdl_t;

// Function declarations only
bool init_sdl(sdl_t *sdl);
void update_screen(const sdl_t *sdl, const chip8_t *chip8);
void final_cleanup(sdl_t sdl);

#endif // DISPLAY_H