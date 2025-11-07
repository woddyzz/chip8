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

// map keyboard keys to CHIP-8 keys
static const SDL_Keycode keymap[16] = {
    SDLK_x, SDLK_1, SDLK_2, SDLK_3,
    SDLK_q, SDLK_w, SDLK_e, SDLK_a,
    SDLK_s, SDLK_d, SDLK_z, SDLK_c,
    SDLK_4, SDLK_r, SDLK_f, SDLK_v
};

void handle_input(chip8_t *cpu) {
    SDL_Event event;
    // Reset keys each frame (you can keep this behavior or manage key-up events)
    for (int i = 0; i < 16; i++) cpu->keys[i] = 0;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                cpu->state = QUIT;
                return;
            case SDL_KEYDOWN: {
                SDL_Keycode kc = event.key.keysym.sym;
                if (kc == SDLK_ESCAPE) {
                    cpu->state = QUIT;
                    return;
                }
                for (int i = 0; i < 16; i++) {
                    if (kc == keymap[i]) {
                        cpu->keys[i] = 1;
                        break;
                    }
                }
                break;
            }
            case SDL_KEYUP: {
                SDL_Keycode kc = event.key.keysym.sym;
                for (int i = 0; i < 16; i++) {
                    if (kc == keymap[i]) {
                        cpu->keys[i] = 0;
                        break;
                    }
                }
                break;
            }
            default:
                break;
        }
    }
}


int main(int argc, char **argv) {
    // initialize the chip8
    init_chip8(&cpu);

    // initialize sdl
    if (!init_sdl(&sdl)) {
        SDL_Log("Could not initialize SDL properly %s", SDL_GetError());
        return 1;
    }

    load_rom(&cpu, "IBM Logo.ch8");

    // main loop
    while (cpu.state != QUIT) {
        emulation_cicle(&cpu);
        if (cpu.draw_flag) {
            update_screen(&sdl, &cpu);
            cpu.draw_flag = 0;
        }
        handle_input(&cpu);
        SDL_Delay(16); // ~60 FPS
    }

    final_cleanup(sdl);
    return 0;
}