#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "chip8.h"
#include "SDL.h"
#include "display.h"

bool init_sdl(sdl_t *sdl) {
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) {
        SDL_Log("Could not initialize SDL: %s", SDL_GetError());
        return false;
    }

    sdl->window = SDL_CreateWindow("CHIP8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 512, 0);
    if (!sdl->window) {
        SDL_Log("Could not create a window %s", SDL_GetError());
        return false;
    }

    sdl->renderer = SDL_CreateRenderer(sdl->window, -1, 0);
    if (!sdl->renderer) {
        SDL_Log("Could not create renderer %s", SDL_GetError());
        return false;
    }

    sdl->texture = SDL_CreateTexture(sdl->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    if (!sdl->texture) {
        SDL_Log("Could not create texture %s", SDL_GetError());
        return false;
    }
    return true;
}

void update_screen(const sdl_t sdl) {
    SDL_RenderCopy(sdl.renderer, sdl.texture, NULL, NULL);
    SDL_RenderPresent(sdl.renderer);
}

void final_cleanup(sdl_t sdl) {
    SDL_DestroyWindow(sdl.window);
    SDL_Quit();
}