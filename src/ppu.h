#ifndef PPU_H
#define PPU_H

#include <stdint.h>
#include <stdbool.h>
#include "mmu.h"

#define PPU_DISPLAY_WIDTH 160
#define PPU_DISPLAY_HEIGHT 144
#define PPU_DISPLAY_SIZE (PPU_DISPLAY_WIDTH * PPU_DISPLAY_HEIGHT)

typedef struct {
    uint32_t display[PPU_DISPLAY_SIZE];
    int cycle_count;
    int scanline;
    bool drawFlag;
    int mode;
} PPU;

void ppu_initialize(PPU* ppu, MMU* mmu);
void ppu_cycle(PPU* ppu, MMU* mmu);

#endif
