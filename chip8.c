#include "chip8.h"
#include "renderer.h"

void init_chip8(chip8* c8){
    init_zero(c8->ram);
    init_zero(c8->V);
    init_zero(c8->display);
    init_zero(c8->keys);
    init_zero(c8->stack);
    const uint8_t font[] = {
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

    for(int i=0;i<80;i++){
        c8->ram[0x50+i] = font[i];
    }
    c8->delay_t = 0;
    c8->sound_t = 0;
    c8->stack_p = 0;
    c8->prog_counter = 0x200;
    c8->state = OFF;
    c8->I = 0;
}

void chip8_load_rom(chip8* c8, const char* rom_name){
    FILE* rom = fopen(rom_name,"rb");
    if(rom==NULL){ 
        printf("rom file does not exists");
        exit(EXIT_FAILURE);}
    fseek(rom,0,SEEK_END);
    int rom_size = ftell(rom);
    rewind(rom);
    uint8_t *rom_buffer;
    rom_buffer = (uint8_t*) malloc(sizeof(uint8_t) * rom_size);
        if (rom_buffer == NULL) {
            printf("ERROR: Out of memory\n");
            exit(EXIT_FAILURE);
        }

        // TODO: handle error if not read into memory?
        fread(rom_buffer, sizeof(uint8_t), rom_size, rom); 

        // Check that the rom is not too large for the region in memory it is placed in
        if ((0xFFF-0x200) >= rom_size) {
            for(int i = 0; i < rom_size; i++) {
                c8->ram[i + 0x200] = rom_buffer[i];
            }
        }
        else {
            printf("ERROR: ROM file too large\n");
            exit(EXIT_FAILURE);
        }

}

void chip8_emulate_cycle(chip8* c8) {
    
    uint16_t opcode = (c8->ram[c8->prog_counter] << 8) | c8->ram[c8->prog_counter + 1];
    c8->prog_counter+=2;
    int temp1=0,temp2=0;
    uint8_t x,y;
    switch(opcode & 0xF000){
        case 0x0000:
            switch(opcode & 0x0FFF){
                case 0x00E0:
                    clr_screen(c8);
                    break;
                case 0x00EE:
                    c8->prog_counter = c8->stack[c8->stack_p];
                    c8->stack_p--;
                    break;
            }
            break;   
        case 0x1000:
            c8->prog_counter = opcode & 0x0FFF;
            break;
        case 0x2000:
            c8->stack_p++;
            c8->stack[c8->stack_p]=c8->prog_counter;
            c8->prog_counter = opcode & 0x0FFF;
            break;
        case 0x3000:
            if(c8->V[(opcode & 0x0F00)>>8] == (opcode & 0x00FF))
                c8->prog_counter+=2;
            break;
        case 0x4000:
            if(c8->V[(opcode & 0x0F00)>>8] != (opcode & 0x00FF))
                c8->prog_counter+=2;
            break;
        case 0x5000:
            if(c8->V[(opcode & 0x0F00)>>8] == c8->V[(opcode & 0x00F0)>>4])
                c8->prog_counter+=2;
            break;
        case 0x6000:
            c8->V[(opcode & 0x0F00)>>8] = opcode & 0x00FF;
            break;
        case 0x7000:
            c8->V[(opcode & 0x0F00)>>8] += opcode & 0x00FF;
            break;
        case 0x8000:
            switch(opcode & 0x000F){
                case 0x0000:
                    c8->V[(opcode & 0x0F00)>>8] = c8->V[(opcode & 0x00F0)>>4];
                    break;
                case 0x0001:
                    c8->V[(opcode & 0x0F00)>>8] = c8->V[(opcode & 0x00F0)>>4] | c8->V[(opcode & 0x0F00)>>8];
                    break;
                case 0x0002:
                    c8->V[(opcode & 0x0F00)>>8] = c8->V[(opcode & 0x00F0)>>4] & c8->V[(opcode & 0x0F00)>>8];
                    break;
                case 0x0003:
                    c8->V[(opcode & 0x0F00)>>8] = c8->V[(opcode & 0x00F0)>>4] ^ c8->V[(opcode & 0x0F00)>>8];
                    break;
                case 0x0004:
                    temp1= c8->V[(opcode & 0x0F00)>>8]+c8->V[(opcode & 0x00F0)>>4];
                    temp2=(temp1)/256;
                    if(temp2){
                        c8->V[0xF] = 1;
                    }
                    else c8->V[0xF] = 0;
                    c8->V[(opcode & 0x0F00)>>8] = temp1 & 0xFF;
                    break;
                case 0x0005:
                    x = (opcode & 0x0F00) >> 8;
                    y = (opcode & 0x00F0) >> 4;
                    c8->V[0xF] = (c8->V[x] >= c8->V[y]) ? 1 : 0;
                    c8->V[x] -= c8->V[y];
                    break;
                case 0x0006:
                    temp1 = c8->V[(opcode & 0x0F00)>>8];
                    temp2 = temp1 & 1;
                    if(temp2){
                        c8->V[0xF] = 1;
                    }
                    else c8->V[0xF] = 0;
                    c8->V[(opcode & 0x0F00)>>8] = temp1>>1;
                    break;
                case 0x0007:
                    x = (opcode & 0x0F00) >> 8; 
                    y = (opcode & 0x00F0) >> 4;
                    c8->V[0xF] = (c8->V[y] >= c8->V[x]) ? 1 : 0;
                    c8->V[x] = c8->V[y] - c8->V[x];
                    break;
                case 0x000E:
                    temp1 = c8->V[(opcode & 0x0F00)>>8];
                    temp2 = temp1 & 0x80;
                    if(temp2){
                        c8->V[0xF] = 1;
                    }
                    else c8->V[0xF] = 0;
                    c8->V[(opcode & 0x0F00)>>8] = temp1<<1;
                    break;
            }
            break;
        case 0x9000:
            if(c8->V[(opcode & 0x0F00)>>8] != c8->V[(opcode & 0x00F0)>>4])
                c8->prog_counter+=2;
            break;
        case 0xA000:
            c8->I = opcode & 0x0FFF;
            break;
        case 0xB000:
            c8->prog_counter = c8->V[0] + (opcode & 0x0FFF);
            break;
        case 0xC000:
            c8->V[(opcode & 0x0F00)>>8] = (opcode & 0x00FF) & (rand()%256);
            break;
        case 0xD000:{
            uint8_t x = c8->V[(opcode & 0x0F00) >> 8];
            uint8_t y = c8->V[(opcode & 0x00F0) >> 4];
            uint8_t height = opcode & 0x000F;
            c8->V[0xF] = 0;

            for(int row = 0 ; row < height; row++){
                uint8_t sprite_byte = c8->ram[c8->I+row];

                for(int col=0;col<8;col++){
                    if ((sprite_byte & (0x80 >> col))!=0){
                        int pixel_x = (x + col) % 64;
                        int pixel_y = (y+row) % 32;
                        int index = pixel_y*64 + pixel_x;

                        if(c8->display[index]==1)
                            c8->V[0xF] = 1;
                        c8->display[index]^=1;
                    }
                }
            }
            
            }
            break;
        case 0xE000:
            switch(opcode & 0x00FF){
                case 0x009E:
                    if(c8->keys[c8->V[(opcode & 0x0F00)>>8]])
                        c8->prog_counter+=2;
                    break;
                case 0x00A1:
                    if(!(c8->keys[c8->V[(opcode & 0x0F00)>>8]]))
                        c8->prog_counter+=2;
                    break;
                default: printf("undefined opcode of type (0xE000)");
                exit(1);
            }
            break;
        case 0xF000:
            switch(opcode & 0x00FF){
                case 0x0007:  
                    c8->V[(opcode & 0x0F00)>>8] = c8->delay_t;
                    break;
                case 0x000A:
                    for(int i =0 ;i<16;i++){
                        if(IsKeyDown(keymap[i])){
                            c8->prog_counter+=2;
                            c8->V[(opcode & 0x0F00)>>8] = i;
                            break;

                        }
                    }
                    c8->prog_counter-=2;
                    break;
                case 0x0015:
                    c8->delay_t = c8->V[(opcode & 0x0F00)>>8];
                    break;
                case 0x0018:
                    c8->sound_t = c8->V[(opcode & 0x0F00)>>8];
                    break;
                case 0x001E:
                    c8->I += c8->V[(opcode & 0x0F00)>>8];
                    break;
                case 0x0029:
                    c8->I = 0x50 + 5*(c8->V[(opcode & 0x0F00)>>8]);
                    break;
                case 0x0033:
                    temp1 =c8->V[(opcode & 0x0F00)>>8];
                    for(int i =2;i>-1;i--){
                        c8->ram[c8->I + i] = temp1%10;
                        temp1=temp1/10;
                    }
                    break;
                case 0x0055:
                    for(int i = 0 ; i<16;i++){
                        c8->ram[c8->I +i] = c8->V[i];
                    }
                    break;
                case 0x0065:
                    for(int i = 0 ; i<16;i++){
                        c8->V[i] = c8->ram[c8->I +i];
                    }
                    break;
                default: printf("undefined opcode of type (0xF000)");
                exit(1);
            }
            break;
    }
    
}

void clr_screen(chip8* c8) {
    init_zero(c8->display);
}

void timer_ticks(chip8 *c8 , double* l_time){
    double now =GetTime();
    if(now - *l_time >=1.0/60.0){
        if (c8->delay_t > 0) c8->delay_t--;
        if (c8->delay_t > 0) c8->delay_t--;
        *l_time = now;
    }
}
