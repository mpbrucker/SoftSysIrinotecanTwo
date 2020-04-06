#include <alsa/asoundlib.h>
#include <math.h>
#include "globals.h"

typedef struct {
    snd_pcm_channel_area_t *areas; 
    snd_pcm_uframes_t period_size; 
    unsigned int periods;
    double phase; 
    unsigned int sample_rate; 
    double freq;
} tone_params;

void sample_sine(tone_params * params);
void write_samples(snd_pcm_t *handle, signed short *samples, tone_params * params);
int open_playback_device(snd_pcm_t **handle, snd_pcm_hw_params_t **params, tone_params * tone, unsigned int * period_time, char * dev_name);

tone_params *init_tone();
void free_tone();
void updatePCM(double v);
int runPCM();

//void killPCM();
//int main();