/*

This example reads standard from input and writes
to the default PCM device for 5 seconds of data.

*/

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API
#define TIME 5000000

#include "pcm.h"

// Opens a specified playback device and sets hardware settings.
void open_playback_device(snd_pcm_t **handle, snd_pcm_hw_params_t **params, snd_pcm_uframes_t * frame_size, unsigned int * sample_rate, char * dev_name) {
    int dir;
    int res = snd_pcm_open(handle, dev_name, SND_PCM_STREAM_PLAYBACK, SND_PCM_ASYNC);

    // Allocate hardware parameters object and set defaults.
    snd_pcm_hw_params_alloca(params);
    snd_pcm_hw_params_any(*handle, *params);
    // Set format and channels.
    snd_pcm_hw_params_set_format(*handle, *params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(*handle, *params, 2);
    // Set sample rate and period size (_near sets parameter space as close as possible)
    snd_pcm_hw_params_set_rate_near(*handle, *params, sample_rate, &dir);
    snd_pcm_hw_params_set_period_size_near(*handle, *params, frame_size, &dir);

    res = snd_pcm_hw_params(*handle, *params);
    // Get period size and time
    snd_pcm_hw_params_get_period_size(*params, frame_size, &dir);
    snd_pcm_hw_params_get_period_time(*params, sample_rate, &dir);
}

void get_sample(char * buffer, int sample_size) {
    int res = read(0, buffer, sample_size);
}

int main () {
    long loops;
    int size;
    int res;
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    char *buffer;
    snd_pcm_uframes_t frames = 16;
    unsigned int sample_rate = 44100;

    open_playback_device(&handle, &params, &frames, &sample_rate, "default");
    size = frames * 4; /* 2 bytes/sample, 2 channels */
    buffer = (char *) malloc(size);

    /* 5 seconds in microseconds divided by
    * period time */
    loops = TIME / sample_rate;

    for (; loops > 0; loops--) {
        get_sample(buffer, size);
        res = snd_pcm_writei(handle, buffer, frames);
    }

    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    free(buffer);

    return 0;
}
