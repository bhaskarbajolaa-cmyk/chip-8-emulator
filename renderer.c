
#include "renderer.h"

int keymap[16] = {
    KEY_X,      // 0
    KEY_ONE,    // 1
    KEY_TWO,    // 2
    KEY_THREE,  // 3
    KEY_Q,      // 4
    KEY_W,      // 5
    KEY_E,      // 6
    KEY_A,      // 7
    KEY_S,      // 8
    KEY_D,      // 9
    KEY_Z,      // A
    KEY_C,      // B
    KEY_FOUR,   // C
    KEY_R,      // D
    KEY_F,      // E
    KEY_V       // F
};

void init_renderer(void) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "CHIP-8 Emulator");
    SetTargetFPS(60);
}

void draw_display(chip8* c8) {
    BeginDrawing();
    ClearBackground(BLACK);

    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            if (c8->display[y * 64 + x]) {
                DrawRectangle(x * WINDOW_SCALE, y * WINDOW_SCALE, WINDOW_SCALE, WINDOW_SCALE, WHITE);
            }
        }
    }

    EndDrawing();
}

void update_keys(chip8* c8){
    for(int i =0 ;i<16;i++){
        c8->keys[i] = IsKeyDown(keymap[i]);
    }
}

void close_renderer(void) {
    CloseWindow();
}
