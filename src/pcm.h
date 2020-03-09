#include <alsa/asoundlib.h>
#include <math.h>

typedef struct {
    snd_pcm_channel_area_t *areas; 
    snd_pcm_uframes_t period_size; 
    double phase; 
    unsigned int sample_rate; 
    double freq;
} tone_params;

static void sample_sine(const snd_pcm_channel_area_t *areas, int count, double *_phase, unsigned int sample_rate, double freq);
int write_samples(snd_pcm_t *handle, signed short *samples, snd_pcm_channel_area_t *areas, snd_pcm_uframes_t period_size, double * phase, unsigned int sample_rate, double freq);
void open_playback_device(snd_pcm_t **handle, snd_pcm_hw_params_t **params, tone_params * tone, unsigned int * period_time, char * dev_name);

int main();