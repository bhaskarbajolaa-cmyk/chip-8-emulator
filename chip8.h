#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define RAM_SIZE 4096
#define REGISTER 16
#define KEYS 16
#define DISPLAY (64 * 32)
#define STACK 16
#define init_zero(arr) memset((arr), 0, sizeof(arr))

typedef enum {
    OFF,
    RUNNING,
    PAUSE
} state_type;

typedef struct {
    uint8_t ram[RAM_SIZE];
    uint8_t V[REGISTER];
    uint8_t delay_t;
    uint8_t sound_t;
    uint8_t stack_p;
    uint8_t display[DISPLAY];
    uint8_t keys[KEYS];
    state_type state;
    uint16_t I;
    uint16_t prog_counter;
    uint16_t stack[STACK];
} chip8;

// Function declarations
void init_chip8(chip8* c8);
void chip8_load_rom(chip8* c8, const char* rom_name);
void chip8_emulate_cycle(chip8* c8);
void clr_screen(chip8* c8); 
void timer_ticks(chip8 *c8 , double* l_time);

#endif
