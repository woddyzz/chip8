#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "display.h"
#include "chip8.h"
#include "SDL.h"

chip8_t cpu;
sdl_t sdl;

// just closes the window
void check_game_status(chip8_t *cpu) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                cpu->state = QUIT;
                return;
            default:
                break;
        }
    }
}

void main(int agrc, char **argv){

    init_sdl(&sdl);
    init_chip8(&cpu);

    while(cpu.state != QUIT) {
        SDL_Delay(16);
        check_game_status(&cpu);
        update_screen(sdl);
        SDL_RenderClear(sdl.renderer);
    }
}