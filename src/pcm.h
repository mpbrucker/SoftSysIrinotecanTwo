#include <alsa/asoundlib.h>
#include <math.h>

static void sample_sine(const snd_pcm_channel_area_t *areas, int count, double *_phase, unsigned int sample_rate);
int write_samples(snd_pcm_t *handle, signed short *samples, snd_pcm_channel_area_t *areas, snd_pcm_uframes_t period_size, double * phase, unsigned int sample_rate);
void open_playback_device(snd_pcm_t **handle, snd_pcm_hw_params_t **params, snd_pcm_uframes_t * period_size, unsigned int * sample_rate, unsigned int * period_time, char * dev_name);

int main();