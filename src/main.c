#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>
#include "cart.h"
#include "cpu.h"
#include "mmu.h"
#include "ppu.h"
#include "apu.h"

#define SCALE_FACTOR 3
#define GB_CPU_FREQ 4194304  // Game Boy CPU frequency in Hz
#define FRAME_CYCLES (GB_CPU_FREQ / 60)  // Cycles per frame at 60 FPS

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file.gb>\n", argv[0]);
        return 1;
    }

    // Initialize Cart, MMU, CPU, PPU, and APU
    Cart cart;
    cart_initialize(&cart);
    cart_load(&cart, argv[1]);

    MMU mmu;
    mmu_initialize(&mmu);
    mmu_load_cart(&mmu, &cart);
    mmu_load_bios(&mmu, "roms/gb_bios.bin");

    CPU cpu;
    cpu_initialize(&cpu);

    PPU ppu;
    ppu_initialize(&ppu, &mmu);

    APU apu;
    apu_initialize(&apu);

    // Initialize Raylib
    InitWindow(PPU_DISPLAY_WIDTH * SCALE_FACTOR, PPU_DISPLAY_HEIGHT * SCALE_FACTOR, "Game Boy Emulator");
    InitAudioDevice();
    AudioStream audioStream = LoadAudioStream(44100, 16, 1);
    PlayAudioStream(audioStream);

    uint32_t pixels[PPU_DISPLAY_WIDTH * PPU_DISPLAY_HEIGHT];
    Image screenImage = {
        .data = pixels,
        .width = PPU_DISPLAY_WIDTH,
        .height = PPU_DISPLAY_HEIGHT,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };
    Texture2D texture = LoadTextureFromImage(screenImage);

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        int cycles_executed = 0;

        while (cycles_executed < FRAME_CYCLES) {
            // CPU cycle
            int cycles = cpu_cycle(&cpu, &mmu, &ppu);
            cycles_executed += cycles;

            for (int i = 0; i < cycles; i++) {
                ppu_cycle(&ppu, &mmu);
                apu_cycle(&apu, &mmu);
            }

            // Audio processing
            if (apu.buffer_position > 0 && IsAudioStreamProcessed(audioStream)) {
                UpdateAudioStream(audioStream, apu.buffer, apu.buffer_position);
                apu.buffer_position = 0;
            }
        }

        // PPU signal
        if (ppu.drawFlag) {
            for (int i = 0; i < PPU_DISPLAY_WIDTH * PPU_DISPLAY_HEIGHT; ++i) {
                pixels[i] = ppu.display[i];
            }
            UpdateTexture(texture, pixels);
            ppu.drawFlag = false;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTextureEx(texture, (Vector2){0, 0}, 0.0f, SCALE_FACTOR, WHITE);
        EndDrawing();
    }

    UnloadTexture(texture);
    UnloadAudioStream(audioStream);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
