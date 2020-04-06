#include <alsa/asoundlib.h>
#include <math.h>
#include "globals.h"

#define MAXNOTES 8

typedef struct {
    double phase;
    double freq;
} tone_params;

void sample_sine(tone_params * params);
void write_samples(snd_pcm_t *handle, signed short *samples, int * nframes);
int open_playback_device(snd_pcm_t **handle, snd_pcm_hw_params_t **params, unsigned int * period_time, char * dev_name);

tone_params *init_tone();
void free_tone();
void updatePCM(double v, int i);
int runPCM();

//void killPCM();
//int main();