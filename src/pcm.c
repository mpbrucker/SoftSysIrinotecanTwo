/*

This example reads standard from input and writes
to the default PCM device for 5 seconds of data.

*/

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#include "pcm.h"

void open_playback_device(snd_pcm_t **handle, snd_pcm_hw_params_t **params, snd_pcm_uframes_t * frame_size, char * dev_name) {
    unsigned int val = 44100;
    int dir;
    int res = snd_pcm_open(handle, dev_name,
                        SND_PCM_STREAM_PLAYBACK, 0);

    // Allocate hardware parameters object and set defaults.
    snd_pcm_hw_params_alloca(params);
    snd_pcm_hw_params_any(*handle, *params);
    // Set format and channels.
    snd_pcm_hw_params_set_format(*handle, *params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(*handle, *params, 2);
    // Set sample rate and period size (_near sets parameter space as close as possible)
    snd_pcm_hw_params_set_rate_near(*handle, *params, &val, &dir);
    snd_pcm_hw_params_set_period_size_near(*handle, *params, frame_size, &dir);

    res = snd_pcm_hw_params(*handle, *params);
    // Get period size and time
    snd_pcm_hw_params_get_period_size(*params, frame_size, &dir);
    snd_pcm_hw_params_get_period_time(*params, &val, &dir);

}


int main () {
    long loops;
    int size;
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    char *buffer;
    snd_pcm_uframes_t frames = 16;


    open_playback_device(&handle, &params, "default");


    size = frames * 4; /* 2 bytes/sample, 2 channels */
    buffer = (char *) malloc(size);

    /* 5 seconds in microseconds divided by
    * period time */
    loops = 5000000 / val;

    while (loops > 0) {
    loops--;
    rc = read(0, buffer, size);
    rc = snd_pcm_writei(handle, buffer, frames);
    }

    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    free(buffer);

    return 0;
}
