#include <alsa/asoundlib.h>

void open_playback_device(snd_pcm_t **handle, snd_pcm_hw_params_t **params, snd_pcm_uframes_t * frame_size, unsigned int * sample_rate, char * dev_name);
int main();