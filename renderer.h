#ifndef RENDERER_H
#define RENDERER_H


#include "raylib.h"
#include "chip8.h"




#define WINDOW_SCALE 10
#define WINDOW_WIDTH 64*WINDOW_SCALE 
#define WINDOW_HEIGHT 32*WINDOW_SCALE

extern int keymap[16];


void init_renderer(void);
void draw_display(chip8* c8);
void update_keys(chip8* c8);
void close_renderer(void);

#endif