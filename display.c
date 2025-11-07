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

void update_screen(const sdl_t sdl, const chip8_t chip8) {
    void *pixels;
    int pitch;

    // Lock the texture to get direct pixel access
    SDL_LockTexture(sdl.texture, NULL, &pixels, &pitch);

    uint32_t *dst = (uint32_t *)pixels;
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            int i = y * 64 + x;
            uint32_t color = (chip8.display[i]) ? 0xFFFFFFFF : 0x000000FF;
            dst[y * (pitch / 4) + x] = color;
        }
    }

    SDL_UnlockTexture(sdl.texture);

    SDL_RenderCopy(sdl.renderer, sdl.texture, NULL, NULL);
    SDL_RenderPresent(sdl.renderer);
}

void final_cleanup(sdl_t sdl) {
    SDL_DestroyTexture(sdl.texture);
    SDL_DestroyRenderer(sdl.renderer);
    SDL_DestroyWindow(sdl.window);
    SDL_Quit();
}