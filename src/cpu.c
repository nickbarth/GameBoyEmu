#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"
#include "ppu.h"

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(x) if (cpu->debug) printf x
#else
#define DEBUG_PRINT(x) do {} while (0)
#endif

void cpu_initialize(CPU* cpu) {
    cpu->a = 0x00;
    cpu->f = 0x00;

    cpu->b = 0x00;
    cpu->c = 0x00;

    cpu->d = 0x00;
    cpu->e = 0x00;

    cpu->h = 0x00;
    cpu->l = 0x00;

    cpu->pc = 0x0000;
    cpu->sp = 0x0000;

    cpu->cycles = 0;
    cpu->debug = false;
}

void cpu_debug(CPU* cpu) {
    DEBUG_PRINT(("\n======\n\n"));

    // cycle
    DEBUG_PRINT(("Cycle:\n"));
    DEBUG_PRINT(("%d\n\n", cpu->cycles));

    // state
    DEBUG_PRINT(("State:\n"));
    DEBUG_PRINT(("PC: 0x%04X\n", cpu->pc));
    DEBUG_PRINT(("SP: 0x%04X\n\n", cpu->sp));

    // registers
    DEBUG_PRINT(("Registers:\n"));
    DEBUG_PRINT(("AF: 0x%02X%02X\n", cpu->a, cpu->f));
    DEBUG_PRINT(("BC: 0x%02X%02X\n", cpu->b, cpu->c));
    DEBUG_PRINT(("DE: 0x%02X%02X\n", cpu->d, cpu->e));
    DEBUG_PRINT(("HL: 0x%02X%02X\n\n", cpu->h, cpu->l));

    // flags
    DEBUG_PRINT(("Flags:\n"));
    DEBUG_PRINT(("Z: %d\n", (cpu->f & 0x80) >> 7));
    DEBUG_PRINT(("N: %d\n", (cpu->f & 0x40) >> 6));
    DEBUG_PRINT(("H: %d\n", (cpu->f & 0x20) >> 5));
    DEBUG_PRINT(("C: %d\n\n", (cpu->f & 0x10) >> 4));
}

void set_af(CPU* cpu, uint16_t value) {
    cpu->a = value >> 8;
    cpu->f = value & 0x00FF;
}

uint16_t get_af(CPU* cpu) {
    return (cpu->a << 8) | cpu->f;
}

void set_bc(CPU* cpu, uint16_t value) {
    cpu->b = value >> 8;
    cpu->c = value & 0x00FF;
}

uint16_t get_bc(CPU* cpu) {
    return (cpu->b << 8) | cpu->c;
}

void set_de(CPU* cpu, uint16_t value) {
    cpu->d = value >> 8;
    cpu->e = value & 0x00FF;
}

uint16_t get_de(CPU* cpu) {
    return (cpu->d << 8) | cpu->e;
}

void set_hl(CPU* cpu, uint16_t value) {
    cpu->h = value >> 8;
    cpu->l = value & 0x00FF;
}

uint16_t get_hl(CPU* cpu) {
    return (cpu->h << 8) | cpu->l;
}

// zero flag
void set_z(CPU* cpu, bool value) {
    if (value) {
        cpu->f |= 0x80;
    } else {
        cpu->f &= ~0x80;
    }
}

// subtract flag
void set_n(CPU* cpu, bool value) {
    if (value) {
        cpu->f |= 0x40;
    } else {
        cpu->f &= ~0x40;
    }
}

// half carry flag
void set_h(CPU* cpu, bool value) {
    if (value) {
        cpu->f |= 0x20;
    } else {
        cpu->f &= ~0x20;
    }
}

// carry flag
void set_c(CPU* cpu, bool value) {
    if (value) {
        cpu->f |= 0x10;
    } else {
        cpu->f &= ~0x10;
    }
}

void cpu_cycle(CPU* cpu, MMU* mmu, PPU* ppu) {
    cpu_debug(cpu);

    uint8_t opcode = mmu->data[cpu->pc];

    DEBUG_PRINT(("Opcode:\n0x%02X\n\n", opcode));
    DEBUG_PRINT(("$%04X: ", cpu->pc));

    cpu->pc += 1;
    int cycles = 0;

    switch (opcode) {
        case 0x00: // NOP
            DEBUG_PRINT(("NOP"));
            cycles = 4;
            break;
        case 0x04: // INC B
            DEBUG_PRINT(("INC B"));
            {
                cpu->b += 1;
                set_z(cpu, cpu->b == 0);
                set_n(cpu, 0);
                set_h(cpu, (cpu->b & 0x0F) == 0);
            }
            cycles = 4;
            break;
        case 0x05: // DEC B
            DEBUG_PRINT(("DEC B"));
            {
                set_h(cpu, (cpu->b & 0x0F) == 0);
                cpu->b -= 1;
                set_z(cpu, cpu->b == 0);
                set_n(cpu, 1);
            }
            cycles = 4;
            break;
        case 0x06: // LD B,n
            {
                uint8_t n = mmu_read(mmu, cpu->pc);
                DEBUG_PRINT(("LD B,$%02X", n));
                cpu->b = n;
                cpu->pc += 1;
                cycles = 8;
            }
            break;
        case 0x0C: // INC C
            DEBUG_PRINT(("INC C"));
            {
                cpu->c += 1;
                set_z(cpu, cpu->c == 0);
                set_n(cpu, 0);
                set_h(cpu, (cpu->c & 0x0F) == 0);
            }
            cycles = 4;
            break;
        case 0x0D: // DEC C
            DEBUG_PRINT(("DEC C"));
            {
                set_h(cpu, (cpu->c & 0x0F) == 0);
                cpu->c -= 1;
                set_z(cpu, cpu->c == 0);
                set_n(cpu, 1);
            }
            cycles = 4;
            break;
        case 0x0E: // LD C,n
            {
                uint8_t n = mmu_read(mmu, cpu->pc);
                DEBUG_PRINT(("LD C,$%02X", n));
                cpu->c = n;
                cpu->pc += 1;
                cycles = 8;
            }
            break;
        case 0x11: // LD DE,nn
            {
                uint16_t nn = mmu_read16(mmu, cpu->pc);
                DEBUG_PRINT(("LD DE,$%04X", nn));
                set_de(cpu, nn);
                cpu->pc += 2;
                cycles = 12;
            }
            break;
        case 0x13: // INC DE
            {
                DEBUG_PRINT(("INC DE"));
                uint16_t de = get_de(cpu) + 1;
                set_de(cpu, de);
                cycles = 8;
            }
            break;
        case 0x15: // DEC D
            DEBUG_PRINT(("DEC D"));
            {
                set_h(cpu, (cpu->d & 0x0F) == 0);
                cpu->d -= 1;
                set_z(cpu, cpu->d == 0);
                set_n(cpu, 1);
            }
            cycles = 4;
            break;
        case 0x16: // LD D,n
            {
                uint8_t n = mmu_read(mmu, cpu->pc);
                DEBUG_PRINT(("LD D,$%02X", n));
                cpu->d = n;
                cpu->pc += 1;
                cycles = 8;
            }
            break;
        case 0x17: // RLA
            DEBUG_PRINT(("RLA"));
            {
                uint8_t carry = (cpu->f & 0x10) >> 4;
                set_c(cpu, (cpu->a & 0x80) >> 7);
                cpu->a = (cpu->a << 1) | carry;
                set_z(cpu, 0);
                set_n(cpu, 0);
                set_h(cpu, 0);
            }
            cycles = 4;
            break;
        case 0x18: // JR n
            {
                int8_t n = mmu_read(mmu, cpu->pc);
                DEBUG_PRINT(("JR %d", n));
                cpu->pc += 1;
                cpu->pc += n;
                cycles = 12;
            }
            break;
        case 0x1A: // LD A,(DE)
            DEBUG_PRINT(("LD A,(DE)"));
            cpu->a = mmu_read(mmu, get_de(cpu));
            cycles = 8;
            break;
        case 0x1D: // DEC E
            DEBUG_PRINT(("DEC E"));
            {
                set_h(cpu, (cpu->e & 0x0F) == 0);
                cpu->e -= 1;
                set_z(cpu, cpu->e == 0);
                set_n(cpu, 1);
            }
            cycles = 4;
            break;
        case 0x1E: // LD E,n
            {
                uint8_t n = mmu_read(mmu, cpu->pc);
                DEBUG_PRINT(("LD E,$%02X", n));
                cpu->e = n;
                cpu->pc += 1;
                cycles = 8;
            }
            break;
        case 0x20: // JR NZ,n
            {
                int8_t n = mmu_read(mmu, cpu->pc);
                DEBUG_PRINT(("JR NZ,%d", n));
                cpu->pc += 1;
                cycles = 8;
                if (!(cpu->f & 0x80)) {
                    cpu->pc += n;
                    cycles = 12;
                }
            }
            break;
        case 0x21: // LD HL,nn
            {
                uint16_t nn = mmu_read16(mmu, cpu->pc);
                DEBUG_PRINT(("LD HL,$%04X", nn));
                set_hl(cpu, nn);
                cpu->pc += 2;
                cycles = 12;
            }
            break;
        case 0x22: // LD (HL+),A
            DEBUG_PRINT(("LD (HL+),A"));
            {
                uint16_t hl = get_hl(cpu);
                mmu_write(mmu, hl, cpu->a);
                set_hl(cpu, hl + 1);
                cycles = 8;
            }
            break;
        case 0x23: // INC HL
            DEBUG_PRINT(("INC HL"));
            set_hl(cpu, get_hl(cpu) + 1);
            cycles = 8;
            break;
        case 0x24: // INC H
            DEBUG_PRINT(("INC H"));
            {
                cpu->h += 1;
                set_z(cpu, cpu->h == 0);
                set_n(cpu, 0);
                set_h(cpu, (cpu->h & 0x0F) == 0);
            }
            cycles = 4;
            break;
        case 0x28: // JR Z,n
            {
                int8_t n = mmu_read(mmu, cpu->pc);
                DEBUG_PRINT(("JR Z,%d", n));
                cpu->pc += 1;
                cycles = 8;
                if (cpu->f & 0x80) {
                    cpu->pc += n;
                    cycles = 12;
                }
            }
            break;
        case 0x2E: // LD L,n
            {
                uint8_t n = mmu_read(mmu, cpu->pc);
                DEBUG_PRINT(("LD L,$%02X", n));
                cpu->l = n;
                cpu->pc += 1;
                cycles = 8;
            }
            break;
        case 0x31: // LD SP,nn
            {
                uint16_t nn = mmu_read16(mmu, cpu->pc);
                DEBUG_PRINT(("LD SP,$%04X", nn));
                cpu->sp = nn;
                cpu->pc += 2;
                cycles = 12;
            }
            break;
        case 0x32: // LDD (HL),A
            DEBUG_PRINT(("LD (HL-),A"));
            {
                uint16_t hl = get_hl(cpu);
                mmu_write(mmu, hl, cpu->a);
                set_hl(cpu, hl - 1);
                cycles = 8;
            }
            break;
        case 0x3D: // DEC A
            DEBUG_PRINT(("DEC A"));
            {
                set_h(cpu, (cpu->a & 0x0F) == 0);
                cpu->a -= 1;
                set_z(cpu, cpu->a == 0);
                set_n(cpu, 1);
            }
            cycles = 4;
            break;
        case 0x3E: // LD A,n
            {
                uint8_t n = mmu_read(mmu, cpu->pc);
                DEBUG_PRINT(("LD A,$%02X", n));
                cpu->a = n;
                cpu->pc += 1;
                cycles = 8;
            }
            break;
        case 0x4F: // LD C,A
            DEBUG_PRINT(("LD C,A"));
            cpu->c = cpu->a;
            cycles = 4;
            break;
        case 0x57: // LD D,A
            DEBUG_PRINT(("LD D,A"));
            cpu->d = cpu->a;
            cycles = 4;
            break;
        case 0x5F: // LD E,A
            DEBUG_PRINT(("LD E,A"));
            cpu->e = cpu->a;
            cycles = 4;
            break;
        case 0x67: // LD H,A
            DEBUG_PRINT(("LD H,A"));
            cpu->h = cpu->a;
            cycles = 4;
            break;
        case 0x77: // LD (HL),A
            DEBUG_PRINT(("LD (HL),A"));
            mmu->data[get_hl(cpu)] = cpu->a;
            cycles = 8;
            break;
        case 0x78: // LD A,B
            DEBUG_PRINT(("LD A,B"));
            cpu->a = cpu->b;
            cycles = 4;
            break;
        case 0x7B: // LD A,E
            DEBUG_PRINT(("LD A,E"));
            cpu->a = cpu->e;
            cycles = 4;
            break;
        case 0x7C: // LD A,H
            DEBUG_PRINT(("LD A,H"));
            cpu->a = cpu->h;
            cycles = 4;
            break;
        case 0x7D: // LD A,L
            DEBUG_PRINT(("LD A,L"));
            cpu->a = cpu->l;
            cycles = 4;
            break;
        case 0x86: // ADD A,(HL)
            DEBUG_PRINT(("ADD A,(HL)"));
            {
                uint8_t n = mmu_read(mmu, get_hl(cpu));
                uint8_t result = cpu->a + n;
                set_z(cpu, result == 0);
                set_n(cpu, 0);
                set_h(cpu, (cpu->a & 0x0F) + (n & 0x0F) > 0x0F);
                set_c(cpu, cpu->a + n > 0xFF);
                cpu->a = result;
            }
            cycles = 8;
            break;
        case 0x90: // SUB B
            DEBUG_PRINT(("SUB B"));
            {
                uint8_t result = cpu->a - cpu->b;
                set_z(cpu, result == 0);
                set_n(cpu, 1);
                set_h(cpu, (cpu->a & 0x0F) < (cpu->b & 0x0F));
                set_c(cpu, cpu->a < cpu->b);
                cpu->a = result;
            }
            cycles = 4;
            break;
        case 0xAF: // XOR A
            DEBUG_PRINT(("XOR A"));
            cpu->a ^= cpu->a;
            set_z(cpu, 1);
            set_n(cpu, 0);
            set_h(cpu, 0);
            set_c(cpu, 0);
            cycles = 4;
            break;
        case 0xBE: // CP (HL)
            DEBUG_PRINT(("CP (HL)"));
            {
                uint8_t n = mmu_read(mmu, get_hl(cpu));
                uint8_t result = cpu->a - n;
                set_z(cpu, result == 0);
                set_n(cpu, 1);
                set_h(cpu, (cpu->a & 0x0F) < (n & 0x0F));
                set_c(cpu, cpu->a < n);
            }
            cycles = 8;
            break;
        case 0xC1: // POP BC
            DEBUG_PRINT(("POP BC"));
            set_bc(cpu, mmu_read16(mmu, cpu->sp));
            cpu->sp += 2;
            cycles = 12;
            break;
        case 0xC5: // PUSH BC
            DEBUG_PRINT(("PUSH BC"));
            cpu->sp -= 2;
            mmu_write16(mmu, cpu->sp, get_bc(cpu));
            cycles = 16;
            break;
        case 0xC9: // RET
            DEBUG_PRINT(("RET"));
            cpu->pc = mmu_read16(mmu, cpu->sp);
            cpu->sp += 2;
            cycles = 16;
            break;
        case 0xCB: // Bit Instructions
            opcode = mmu->data[cpu->pc];
            cpu->pc += 1;
            switch (opcode) {
                case 0x11: // RL C
                    DEBUG_PRINT(("RL C"));
                    {
                        uint8_t carry = (cpu->f & 0x10) >> 4;
                        set_c(cpu, (cpu->c & 0x80) >> 7);
                        cpu->c = (cpu->c << 1) | carry;
                        set_z(cpu, cpu->c == 0);
                        set_n(cpu, 0);
                        set_h(cpu, 0);
                    }
                    cycles = 8;
                    break;
                case 0x7C: // BIT 7,H
                    DEBUG_PRINT(("BIT 7,H"));
                    {
                        set_z(cpu, (cpu->h & 0x80) == 0);
                        set_n(cpu, 0);
                        set_h(cpu, 1);
                    }
                    cycles = 8;
                    break;
                default:
                    fprintf(stderr, "Unknown opcode: 0xCB%X\n", opcode);
                    exit(EXIT_FAILURE);
            }
            break;
        case 0xCD: // CALL nn
            {
                uint16_t address = mmu_read16(mmu, cpu->pc);
                DEBUG_PRINT(("CALL $%04X", address));
                cpu->pc += 2;
                cpu->sp -= 2;
                mmu_write16(mmu, cpu->sp, cpu->pc);
                cpu->pc = address;
                cycles = 24;
            }
            break;
        case 0xE0: // LD (n),A
            {
                uint8_t n = mmu_read(mmu, cpu->pc);
                DEBUG_PRINT(("LD ($FF%02X),A", n));
                mmu_write(mmu, 0xFF00 + n, cpu->a);
                cpu->pc += 1;
                cycles = 12;
            }
            break;
        case 0xE2: // LD (C),A
            DEBUG_PRINT(("LD ($FF%02X),A", cpu->c));
            mmu_write(mmu, 0xFF00 + cpu->c, cpu->a);
            cycles = 8;
            break;
        case 0xEA: // LD (nn),A
            {
                uint16_t nn = mmu_read16(mmu, cpu->pc);
                DEBUG_PRINT(("LD ($%04X),A", nn));
                mmu_write(mmu, nn, cpu->a);
                cpu->pc += 2;
                cycles = 16;
            }
            break;
        case 0xF0: // LD A,(n)
            {
                uint8_t n = mmu_read(mmu, cpu->pc);
                DEBUG_PRINT(("LD A,($FF%02X)", n));
                DEBUG_PRINT(("\nAT $%04X = $%02X\n", 0xFF00 + n, mmu_read(mmu, 0xFF00 + n)));
                cpu->a = mmu_read(mmu, 0xFF00 + n);
                cpu->pc += 1;
                cycles = 12;
            }
            break;
        case 0xFE: // CP n
            {
                uint8_t n = mmu_read(mmu, cpu->pc);
                DEBUG_PRINT(("CP $%02X", n));
                uint8_t a = cpu->a;
                set_z(cpu, a == n);
                set_n(cpu, 1);
                set_h(cpu, (a & 0x0F) < (n & 0x0F));
                set_c(cpu, a < n);
                cpu->pc += 1;
                cycles = 8;
            }
            break;
        default:
            fprintf(stderr, "Unknown opcode: 0x%X\n", opcode);
            exit(EXIT_FAILURE);
    }

    cpu->cycles += cycles;

    for (int i = 0; i < cycles; i++) {
        ppu_cycle(ppu, mmu);
    }
}
