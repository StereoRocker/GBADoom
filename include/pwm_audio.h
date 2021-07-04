#ifndef PWM_AUDIO_H
#define PWM_AUDIO_H

#define MAX_CHANNELS 8

typedef struct
{
    uint16_t lastAudioBufferIdx;
    uint16_t offset;
    uint8_t* lump;
    int size;
    int8_t volume;
} soundChannel_t;

#ifdef __cplusplus
extern "C" {
#endif

void updateSound();
void muteSound();

extern volatile soundChannel_t soundChannels[MAX_CHANNELS];

#ifdef __cplusplus
}
#endif


#endif