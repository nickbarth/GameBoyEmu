#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "cart.h"
#include "cpu.h"
#include "mmu.h"
#include "ppu.h"

#define SCALE_FACTOR 3

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file.gb>\n", argv[0]);
        return 1;
    }

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

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Game Boy Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, PPU_DISPLAY_WIDTH * SCALE_FACTOR, PPU_DISPLAY_HEIGHT * SCALE_FACTOR, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, PPU_DISPLAY_WIDTH, PPU_DISPLAY_HEIGHT);

    uint32_t pixels[PPU_DISPLAY_WIDTH * PPU_DISPLAY_HEIGHT];

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                quit = true;
            }
        }

        // CPU cycle
        int cycles = cpu_cycle(&cpu, &mmu, &ppu);

        for (int i = 0; i < cycles; i++) {
            ppu_cycle(&ppu, &mmu);
        }
        // printf("PC: %04X, SP: %04X, A: %02X, B: %02X, C: %02X, D: %02X, E: %02X, H: %02X, L: %02X, F: %02X\n", cpu.pc, cpu.sp, cpu.a, cpu.b, cpu.c, cpu.d, cpu.e, cpu.h, cpu.l, cpu.f);

        // PPU signal
        if (ppu.drawFlag) {
            for (int i = 0; i < PPU_DISPLAY_WIDTH * PPU_DISPLAY_HEIGHT; ++i) {
                pixels[i] = ppu.display[i];
            }
            SDL_UpdateTexture(texture, NULL, pixels, PPU_DISPLAY_WIDTH * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
            ppu.drawFlag = false;
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
