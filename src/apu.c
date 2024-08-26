#include "apu.h"
#include "mmu.h"
#include <string.h>

void apu_initialize(APU* apu) {
    memset(apu, 0, sizeof(APU));
    apu->nr52 = 0xF1;
}

void apu_cycle(APU* apu, MMU* mmu) {
    static int phase = 0;

    // check sound enabled
    if (!(mmu_read(mmu, 0xFF26) & 0x80)) {
        // output silence
        memset(apu->buffer, 0, sizeof(apu->buffer));
        apu->buffer_position = 0;
        return;
    }

    // check DAC enabled
    uint8_t nr12 = mmu_read(mmu, 0xFF12);
    if ((nr12 & 0xF8) == 0) {
        // output silence
        memset(apu->buffer, 0, sizeof(apu->buffer));
        apu->buffer_position = 0;
        return;
    }

    // check channel enabled
    uint8_t nr14 = mmu_read(mmu, 0xFF14);
    if (!(nr14 & 0x80)) {
        // output silence
        memset(apu->buffer, 0, sizeof(apu->buffer));
        apu->buffer_position = 0;
        return;
    }

    // square 1 - frequency low and high
    uint8_t nr13 = mmu_read(mmu, 0xFF13);
    int frequency = nr13 | ((nr14 & 0x07) << 8);
    int wave_length = 2048 - frequency;

    // duty cycle
    uint8_t nr11 = mmu_read(mmu, 0xFF11);
    int duty_cycle = (nr11 >> 6) & 0x03;

    if (apu->buffer_position < sizeof(apu->buffer) / sizeof(apu->buffer[0])) {
        int amplitude = 0;

        int duty_pos = phase / (wave_length / 8);

        // square wave
        switch (duty_cycle) {
            case 0: amplitude = (duty_pos < 1) ? 3000 : -3000; break; // 12.5% duty cycle
            case 1: amplitude = (duty_pos < 2) ? 3000 : -3000; break; // 25% duty cycle
            case 2: amplitude = (duty_pos < 4) ? 3000 : -3000; break; // 50% duty cycle
            case 3: amplitude = (duty_pos < 6) ? 3000 : -3000; break; // 75% duty cycle
        }

        // fill buffer and increment phase
        apu->buffer[apu->buffer_position++] = amplitude;
        phase = (phase + 1) % wave_length;
    }
}

void audio_callback(void* userdata, uint8_t* stream, int len) {
    APU* apu = (APU*)userdata;
    int16_t* audio_buffer = (int16_t*)stream;

    // no sound
    if (apu->buffer_position == 0) {
        memset(stream, 0, len);
        return;
    }

    for (int i = 0; i < len / 2; i++) {
        if (i < apu->buffer_position) {
            audio_buffer[i] = apu->buffer[i];
        } else {
            audio_buffer[i] = 0;
        }
    }

    apu->buffer_position = 0;
}

