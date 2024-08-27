#ifndef APU_H
#define APU_H

#include <stdint.h>
#include "mmu.h"

typedef struct {
    // audio registers
    uint8_t nr10, nr11, nr12, nr13, nr14; // 1 - square 1
    uint8_t nr21, nr22, nr23, nr24;       // 2 - square 2
    uint8_t nr30, nr31, nr32, nr33, nr34; // 3 - wave
    uint8_t nr41, nr42, nr43, nr44;       // 4 - noise
    uint8_t nr50, nr51, nr52;             // control registers

    // audio buffer
    int16_t buffer[735 * 2]; // one frame at 44100Hz
    int buffer_position;
} APU;

void apu_initialize(APU* apu);
void apu_cycle(APU* apu, MMU* mmu);
void audio_callback(void* userdata, uint8_t* stream, int len);

#endif
