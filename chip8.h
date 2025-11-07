#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

typedef enum { RUNNING, QUIT } chip8_states_t;

typedef struct {
    uint8_t memory[4096];
    uint8_t V[16];
    uint16_t opcode;
    uint16_t I;
    uint16_t pc;
    uint16_t stack[16];
    uint8_t sp;
    uint8_t display[64*32];
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t keys[16];
    uint8_t draw_flag;
    uint8_t key_pressed;
    chip8_states_t state;
} chip8_t;

// function **declarations**
void init_chip8(chip8_t *chip8);
void emulation_cicle(chip8_t *chip8);
void load_rom( chip8_t *chip8, const char *rom_filename);

#endif