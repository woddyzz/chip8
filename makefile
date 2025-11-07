
CLFAGS=-std=c17 -Wall -Wextra -Werror

all:
	gcc main.c display.c chip8.c -o chip8 $(CFLAGS) `sdl2-config --cflags --libs`