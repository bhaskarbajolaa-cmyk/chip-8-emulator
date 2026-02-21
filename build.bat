@echo off

gcc main.c chip8.c renderer.c -o main1.exe ^
-I. ^
-I"raylib-5.5\src" ^
-L. ^
-lraylib -lopengl32 -lgdi32 -lwinmm ^
-std=c99 -Wall -DPLATFORM_DESKTOP

pause
