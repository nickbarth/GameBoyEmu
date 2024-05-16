#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>
#include "mmu.h"
#include "ppu.h"

typedef struct cpu {
    // 8-bit registers
    uint8_t a, f;
    uint8_t b, c;
    uint8_t d, e;
    uint8_t h, l;

    // 16-bit registers
    uint16_t pc; // program counter
    uint16_t sp; // stack pointer

    // clocks
    uint64_t m, t;

    // cycles
    unsigned int cycles;

    // debug
    bool debug;
} CPU;

void cpu_initialize(CPU* cpu);
void cpu_cycle(CPU* cpu, MMU* mmu, PPU* ppu);

#endif
