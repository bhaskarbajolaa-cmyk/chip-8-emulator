#include "chip8.h"
#include "renderer.h"
int main(int argc,char* argv[]){
    
    if(argc>1){
        chip8 cpu;
        srand(time(NULL));
        init_chip8(&cpu);
        chip8_load_rom(&cpu,argv[1]);
        init_renderer();
        double l_time = GetTime();
        while(!WindowShouldClose()){
            for(int i =0;i<10;i++){
            chip8_emulate_cycle(&cpu);}
            update_keys(&cpu);
            timer_ticks(&cpu, &l_time);
            draw_display(&cpu);
        }
    
        close_renderer();
    }
    
    return 0;
}
