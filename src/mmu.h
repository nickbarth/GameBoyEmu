#ifndef MMU_H
#define MMU_H

#include <stdint.h>
#include <stdbool.h>
#include "cart.h"

#define ROM_SIZE 0xFFFF

typedef struct mmu {
    uint8_t data[ROM_SIZE];
} MMU;

void mmu_initialize(MMU* mmu);
void mmu_write(MMU* mmu, uint16_t address, uint8_t value);
void mmu_write16(MMU* mmu, uint16_t address, uint16_t value);
uint8_t mmu_read(MMU* mmu, uint16_t address);
uint16_t mmu_read16(MMU* mmu, uint16_t address);
void mmu_load_bios(MMU* mmu, const char* filename);
void mmu_load_cart(MMU* mmu, Cart* cart);

#endif
