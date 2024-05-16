#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mmu.h"

void mmu_initialize(MMU* mmu) {
    memset(mmu->data, 0, ROM_SIZE);
}

void mmu_write(MMU* mmu, uint16_t address, uint8_t value) {
    if (address < 0x8000) {
        return; // ROM
    }
    mmu->data[address] = value;
}

void mmu_write16(MMU* mmu, uint16_t address, uint16_t value) {
    mmu_write(mmu, address, value & 0xFF);
    mmu_write(mmu, address + 1, value >> 8);
}

uint8_t mmu_read(MMU* mmu, uint16_t address) {
    return mmu->data[address];
}

uint16_t mmu_read16(MMU* mmu, uint16_t address) {
    return mmu_read(mmu, address) | (mmu_read(mmu, address + 1) << 8);
}

void mmu_load_bios(MMU* mmu, const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open BIOS %s\n", path);
        exit(1);
    }

    fread(mmu->data, 1, 0x100, file);
    fclose(file);
}

void mmu_load_cart(MMU* mmu, Cart* cart) {
    memcpy(mmu->data, cart->data, cart->size);
}
