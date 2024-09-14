#include "gba.h"
#include "sound.h"

void setupSounds() {

    REG_SOUNDCNT_X = SND_ENABLED;

	REG_SOUNDCNT_H = SND_OUTPUT_RATIO_100 |
                     DSA_OUTPUT_RATIO_100 |
                     DSA_OUTPUT_TO_BOTH |
                     DSA_TIMER0 |
                     DSA_FIFO_RESET |
                     DSB_OUTPUT_RATIO_100 |
                     DSB_OUTPUT_TO_BOTH |
                     DSB_TIMER1 |
                     DSB_FIFO_RESET;

	REG_SOUNDCNT_L = 0;

}

void playSoundA(const signed char* data, int dataLength, int looping) {

    int cyclesPerSample = PROCESSOR_CYCLES_PER_SECOND / SOUND_FREQ;
    
    dma[1].cnt = 0;
    // Set DMA channel to 1
    DMANow(1, data, REG_FIFO_A, DMA_DESTINATION_FIXED | DMA_AT_REFRESH | DMA_REPEAT | DMA_32);

    // Set up timer 0

    REG_TM0D = 65535 - cyclesPerSample;
    REG_TM0CNT = TIMER_ON;
    
    // Initialize struct members of soundA
    soundA.data = data;
    soundA.dataLength = dataLength;
    soundA.looping = looping;
    soundA.vBlankCount = 0;
    soundA.durationInVBlanks = (VBLANK_FREQ * dataLength) / SOUND_FREQ;
    soundA.isPlaying = 1;
}

void playSoundB(const signed char* data, int dataLength, int looping) {

    dma[2].cnt = 0;

    int cyclesPerSample = PROCESSOR_CYCLES_PER_SECOND / SOUND_FREQ;
    // Set DMA channel to 2
    DMANow(2, data, REG_FIFO_B, DMA_DESTINATION_FIXED | DMA_AT_REFRESH | DMA_REPEAT | DMA_32);

    // Set up timer 1
    REG_TM1CNT = 0;
	
    REG_TM1D = 65535 - cyclesPerSample;
    REG_TM1CNT = TIMER_ON;

    // Initialize struct members of soundB
    soundB.data = data;
    soundB.dataLength = dataLength;
    soundB.looping = looping;
    soundB.vBlankCount = 0;
    soundB.durationInVBlanks = VBLANK_FREQ * dataLength / SOUND_FREQ;
    soundB.isPlaying = 1;

}

void pauseSounds() {
	soundA.isPlaying = 0;
	REG_TM0CNT = 0;
	soundB.isPlaying = 0;
	REG_TM1CNT = 0;

}

void unpauseSounds() {
	soundA.isPlaying = 1;
	REG_TM0CNT = TIMER_ON;
	soundB.isPlaying = 1;
	REG_TM1CNT = TIMER_ON;
}

void stopSounds() {
	dma[1].cnt = 0;
	dma[2].cnt = 0;
	soundA.isPlaying = 0;
	REG_TM0CNT = 0;
	soundB.isPlaying = 0;
	REG_TM1CNT = 0;	

}