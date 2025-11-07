#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "chip8.h"

void init_chip8(chip8_t *chip8) {
    const uint8_t chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

    chip8->pc = 0x200;
    chip8->opcode = 0;
    chip8->I = 0;
    chip8->sp = 0;

    for (int i = 0; i < (64*32); i++) {
        chip8->display[i] = 0;
    }

    for (int i = 0; i < 16; i++) {
        chip8->stack[i] = 0;
        chip8->V[i] = 0;
    }

    for (int i = 0; i < 4096; i++) {
        chip8->memory[i] = 0;
    }

    for (int i = 0; i < 80; i++) {
        chip8->memory[i] = chip8_fontset[i];
    }

    chip8->delay_timer = 0;
    chip8->sound_timer = 0;

    chip8->state = RUNNING;
    chip8->draw_flag = 0;
}

void emulation_cicle(chip8_t *chip8) {
    chip8->opcode = chip8->memory[chip8->pc] << 8 | chip8->memory[chip8->pc + 1];

    if (chip8->opcode == 0x00E0) {
        for (int i = 0; i < (64*32); i++) {
            chip8->display[i] = 0;
        }
        chip8->pc += 2;
        return;
    }

    if (chip8->opcode == 0x00EE) {
        chip8->sp -= 1;
        chip8->pc = chip8->stack[chip8->sp];
        chip8->pc += 2;
        return;
    }

    switch(chip8->opcode & 0xF000) {
        case 0x0000: {
            chip8->pc += 2;
            break;
        }
        case 0x1000: {
            chip8->pc = chip8->opcode & 0x0FFF;
            break;
        }
        case 0x2000: {
            chip8->stack[chip8->sp] = chip8->pc;
            chip8->sp += 1;
            chip8->pc = chip8->opcode & 0x0FFF;
            break;
        }
        case 0x3000: {
            uint8_t x = (chip8->opcode & 0x0F00) >> 8;
            if (chip8->V[x] == (chip8->opcode & 0x00FF)) {
                chip8->pc += 4;
            } else {
                chip8->pc += 2;
            }
            break;
        }
        case 0x4000: {
            uint8_t x = (chip8->opcode & 0x0F00) >> 8;
            if (chip8->V[x] != (chip8->opcode & 0x00FF)) {
                chip8->pc += 4;
            } else {
                chip8->pc += 2;
            }
            break;
        }
        case 0x5000: {
            uint8_t x = (chip8->opcode & 0x0F00) >> 8;
            uint8_t y = (chip8->opcode & 0x00F0) >> 4;
            if (chip8->V[x] == chip8->V[y]) {
                chip8->pc += 4;
            } else {
                chip8->pc += 2;
            }
            break;
        }
        case 0x6000: {
            uint8_t x = (chip8->opcode & 0x0F00) >> 8;
            chip8->V[x] = chip8->opcode & 0x00FF;
            chip8->pc += 2;
            break;
        }
        case 0x7000: {
            uint8_t x = (chip8->opcode & 0x0F00) >> 8;
            chip8->V[x] += chip8->opcode & 0x00FF;
            chip8->pc += 2;
            break;
        }
        case 0x8000: {
            uint8_t x = (chip8->opcode & 0x0F00) >> 8;
            uint8_t y = (chip8->opcode & 0x00F0) >> 4;
            uint8_t m = (chip8->opcode & 0x000F);

            switch(m) {
                case 0x0: chip8->V[x] = chip8->V[y]; break;
                case 0x1: chip8->V[x] |= chip8->V[y]; break;
                case 0x2: chip8->V[x] &= chip8->V[y]; break;
                case 0x3: chip8->V[x] ^= chip8->V[y]; break;
                case 0x4: {
                    uint16_t sum = chip8->V[x] + chip8->V[y];
                    chip8->V[0xF] = (sum > 255);
                    chip8->V[x] = sum & 0xFF;
                    break;
                }
                case 0x5:
                    chip8->V[0xF] = (chip8->V[x] > chip8->V[y]);
                    chip8->V[x] -= chip8->V[y];
                    break;
                case 0x6:
                    chip8->V[0xF] = chip8->V[x] & 1;
                    chip8->V[x] >>= 1;
                    break;
                case 0x7:
                    chip8->V[0xF] = (chip8->V[y] > chip8->V[x]);
                    chip8->V[x] = chip8->V[y] - chip8->V[x];
                    break;
                case 0xE:
                    chip8->V[0xF] = (chip8->V[x] & 0x80) >> 7;
                    chip8->V[x] <<= 1;
                    break;
            }
            chip8->pc += 2;
            break;
        }
        case 0x9000: {
            uint8_t x = (chip8->opcode & 0x0F00) >> 8;
            uint8_t y = (chip8->opcode & 0x00F0) >> 4;
            if (chip8->V[x] != chip8->V[y]) chip8->pc += 4;
            else chip8->pc += 2;
            break;
        }
        case 0xA000: {
            chip8->I = chip8->opcode & 0x0FFF;
            chip8->pc += 2;
            break;
        }
        case 0xB000: {
            uint16_t nnn = chip8->opcode & 0x0FFF;
            chip8->pc = nnn + chip8->V[0];
            break;
        }
        case 0xC000: {
            uint8_t x = (chip8->opcode & 0x0F00) >> 8;
            uint8_t kk = chip8->opcode & 0x00FF;
            uint8_t random_num = rand() % 256;
            chip8->V[x] = random_num & kk;
            chip8->pc += 2;
            break;
        }
        case 0xD000: {
            uint8_t x = chip8->V[(chip8->opcode & 0x0F00) >> 8];
            uint8_t y = chip8->V[(chip8->opcode & 0x00F0) >> 4];
            uint8_t height = chip8->opcode & 0x000F;

            chip8->V[0xF] = 0;

            for (int yline = 0; yline < height; yline++) {
                uint8_t pixel = chip8->memory[chip8->I + yline];
                for (int xline = 0; xline < 8; xline++) {
                    if ((pixel & (0x80 >> xline)) != 0) {
                        int idx = (x + xline + ((y + yline) * 64)) % (64 * 32);
                        if (chip8->display[idx] == 1)
                            chip8->V[0xF] = 1;
                        chip8->display[idx] ^= 1;
                    }
                }
            }
            chip8->draw_flag = 1;
            chip8->pc += 2;
            break;
        }

        case 0xE000: {
            uint8_t x = (chip8->opcode & 0x0F00) >> 8;
            uint8_t kk = chip8->opcode & 0x00FF;

            if (kk == 0x9E) {
                if (chip8->keys[chip8->V[x]] == 1)
                    chip8->pc += 4;
                else
                    chip8->pc += 2;
            } else if (kk == 0xA1) {
                if (chip8->keys[chip8->V[x]] != 1)
                    chip8->pc += 4;
                else
                    chip8->pc += 2;
            }
            break;
        }

        case 0xF000: {
            uint8_t x = (chip8->opcode & 0x0F00) >> 8;
            switch(chip8->opcode & 0x00FF) {
                case 0x07: chip8->V[x] = chip8->delay_timer; break;
                case 0x0A:
                    chip8->key_pressed = 0;
                    for (int i = 0; i < 16; i++) {
                        if (chip8->keys[i] != 0) {
                            chip8->V[x] = i;
                            chip8->key_pressed = 1;
                        }
                    }
                    if (!chip8->key_pressed) return;
                    break;
                case 0x15: chip8->delay_timer = chip8->V[x]; break;
                case 0x18: chip8->sound_timer = chip8->V[x]; break;
                case 0x1E: chip8->I += chip8->V[x]; break;
                case 0x29: chip8->I = chip8->V[x] * 5; break;
                case 0x33:
                    chip8->memory[chip8->I] = chip8->V[x] / 100;
                    chip8->memory[chip8->I + 1] = (chip8->V[x] / 10) % 10;
                    chip8->memory[chip8->I + 2] = chip8->V[x] % 10;
                    break;
                case 0x55:
                    for (int i = 0; i <= x; i++) chip8->memory[chip8->I + i] = chip8->V[i];
                    break;
                case 0x65:
                    for (int i = 0; i <= x; i++) chip8->V[i] = chip8->memory[chip8->I + i];
                    break;
            }
            chip8->pc += 2;
            break;
        }
    }
}

void load_rom(chip8_t *chip8, const char *rom_filename) {
    FILE *rom = fopen(rom_filename, "rb");
    if (rom == NULL) {
        printf("ERROR: ROM file does not exist\n");
        exit(EXIT_FAILURE);
    }

    fseek(rom, 0, SEEK_END);
    long rom_length = ftell(rom);
    rewind(rom);

    uint8_t *rom_buffer = malloc(rom_length);
    if (rom_buffer == NULL) {
        printf("ERROR: Out of memory\n");
        fclose(rom);
        exit(EXIT_FAILURE);
    }

    fread(rom_buffer, 1, rom_length, rom);
    fclose(rom);

    if (rom_length > (4096 - 512)) {
        printf("ERROR: ROM file too large\n");
        free(rom_buffer);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < rom_length; i++) {
        chip8->memory[i + 0x200] = rom_buffer[i];
    }

    free(rom_buffer);
}
