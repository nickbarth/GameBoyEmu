#include <stdio.h>
#include <string.h>
#include "ppu.h"
#include "mmu.h"

void ppu_initialize(PPU* ppu, MMU* mmu) {
    memset(ppu->display, 0, sizeof(ppu->display));
    ppu->cycle_count = 0;
    ppu->scanline = 0;
    ppu->drawFlag = false;

    // set lcdc to enable lcd display
    mmu_write(mmu, 0xFF40, 0x91); // enable lcd and bg display
    mmu_write(mmu, 0xFF44, 0);    // initialize ly to 0
}

void render_scanline(PPU* ppu, MMU* mmu) {
    uint16_t map_offset = 0x9800;
    uint8_t scroll_y = mmu_read(mmu, 0xFF42);
    uint8_t scroll_x = mmu_read(mmu, 0xFF43);

    for (int x = 0; x < PPU_DISPLAY_WIDTH; x++) {
        uint8_t pixel_y = (ppu->scanline + scroll_y) % 256;
        uint8_t pixel_x = (x + scroll_x) % 256;
        uint16_t tile_index = (pixel_y / 8) * 32 + (pixel_x / 8);
        uint8_t tile_id = mmu_read(mmu, map_offset + tile_index);

        uint16_t tile_address = 0x8000 + tile_id * 16;
        uint8_t tile_row = pixel_y % 8;
        uint8_t tile_data1 = mmu_read(mmu, tile_address + tile_row * 2);
        uint8_t tile_data2 = mmu_read(mmu, tile_address + tile_row * 2 + 1);
        uint8_t color_bit = 7 - (pixel_x % 8);
        uint8_t color = ((tile_data2 >> color_bit) & 1) << 1 | ((tile_data1 >> color_bit) & 1);

        // map to grayscale
        uint32_t color_value = 0xFFFFFFFF;
        if (color == 0) color_value = 0xFFFFFFFF;      // white
        else if (color == 1) color_value = 0xAAAAAAFF; // light gray
        else if (color == 2) color_value = 0x555555FF; // dark gray
        else if (color == 3) color_value = 0x000000FF; // black

        ppu->display[ppu->scanline * PPU_DISPLAY_WIDTH + x] = color_value;
    }
}

void ppu_cycle(PPU* ppu, MMU* mmu) {
    ppu->cycle_count++;

    if (ppu->cycle_count >= 456) {
        ppu->cycle_count = 0;
        ppu->scanline++;

        if (ppu->scanline < 144) {
            // visible scanlines
            mmu_write(mmu, 0xFF44, ppu->scanline);
            render_scanline(ppu, mmu);
        } else if (ppu->scanline == 144) {
            // start of v-blank
            ppu->drawFlag = true;
            mmu_write(mmu, 0xFF44, 144);
        } else if (ppu->scanline > 153) {
            // end of v-blank
            ppu->scanline = 0;
            mmu_write(mmu, 0xFF44, ppu->scanline);
            ppu->drawFlag = false;
        } else {
            // v-blank period
            mmu_write(mmu, 0xFF44, ppu->scanline);
        }
    }
}
