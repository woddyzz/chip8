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
void handle_input(chip8_t *cpu) {
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

    // initialize the chip8
    init_chip8(&cpu);

    // initialize sdl
    if (!init_sdl(&sdl)) {
        SDL_Log("Could not initialize SDL properly %s", SDL_GetError());
        return;
    }

    // main loop
    while(cpu.state != QUIT) {
        SDL_Delay(16);
        handle_input(&cpu);
        if (cpu.draw_flag == 1){
            update_screen(sdl);
        }
    }
}