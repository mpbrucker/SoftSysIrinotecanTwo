#define ALSA_PCM_NEW_HW_PARAMS_API
#define TIME 2000000
#define FORMAT SND_PCM_FORMAT_S16_LE
#define CHANNELS 1 // number of audio channels

#include "pcm.h"
#include <signal.h>
#include <stdio.h>
#include <stdio.h>

static volatile int running = 1;
tone_params * tone;

void killPCM(){
    running = 0;
}

void sample_sine(tone_params * params)
{
    static double max_phase = 2. * M_PI;
    double phase = params->phase;
    snd_pcm_uframes_t count = params->period_size;
    snd_pcm_channel_area_t * areas = params->areas;

    double step = (max_phase*(params->freq))/(params->sample_rate);
    unsigned char *samples[CHANNELS];
    int steps[CHANNELS];
    unsigned int chn;
    int format_bits = snd_pcm_format_width(FORMAT);
    unsigned int maxval = (1 << (format_bits - 1)) - 1;
    int bps = format_bits / 8;  /* bytes per sample */
    int res;


    // set the step size for each channel;
    for (chn = 0; chn < CHANNELS; chn++) {
        samples[chn] = (((unsigned char *)areas[chn].addr) + (areas[chn].first / 8));
        steps[chn] = areas[chn].step / 8; // Step size in bytes
    }

    /* fill the channel areas */
    while (count-- > 0) {
        res = sin(phase) * maxval;
        for (chn = 0; chn < CHANNELS; chn++) {
            /* Generate data in native endian format */
            for (int i = 0; i < bps; i++) {
                *(samples[chn] + i) = (res >>  i * 8) & 0xff; // TODO fix segfault here
            }
            samples[chn] += steps[chn];
        }
        phase += step;
        if (phase >= max_phase)
            phase -= max_phase;
    }
    params->phase = phase;
}

void write_samples(snd_pcm_t *handle, signed short *samples, tone_params * params, int *nframes) {
    signed short *ptr;
    int bytes_written, remaining;
    sample_sine(params);
    ptr = samples;
    remaining = tone->period_size;
    while (remaining > 0) {
        bytes_written = snd_pcm_writei(handle, ptr, remaining); // TODO: error check this
        if (bytes_written == -EAGAIN)
                continue;
            if (bytes_written < 0) {
                break;  /* skip one period */
            }
        if (bytes_written == -32){
            snd_pcm_prepare(handle);
        }
        ptr += bytes_written * CHANNELS;
        remaining -= bytes_written;
    }
    *nframes = (tone->period_size - remaining);
}


// Opens a specified playback device and sets hardware settings.
int open_playback_device(snd_pcm_t **handle, snd_pcm_hw_params_t **params, tone_params * tone, unsigned int * period_time, char * dev_name) {
    int dir;
    snd_pcm_open(handle, dev_name, SND_PCM_STREAM_PLAYBACK, SND_PCM_ASYNC);

    // Allocate hardware parameters object and set defaults.
    snd_pcm_hw_params_alloca(params);
    snd_pcm_hw_params_any(*handle, *params);
    // Set format and channels.
    snd_pcm_hw_params_set_format(*handle, *params, FORMAT);
    snd_pcm_hw_params_set_channels(*handle, *params, CHANNELS);
    // Set sample rate and period size (_near sets parameter space as close as possible)
    snd_pcm_hw_params_set_rate_near(*handle, *params, &(tone->sample_rate), &dir);
    snd_pcm_hw_params_set_period_size_near(*handle, *params, &(tone->period_size), &dir);

    snd_pcm_hw_params(*handle, *params);
    // Get period size and time
    if(snd_pcm_hw_params_set_period_size(*handle, *params, tone->period_size, dir)){
        fprintf(stderr, "Error setting period_size.\n");
        return -1;
    }

    if(snd_pcm_hw_params_set_periods_min(*handle, *params, &(tone->periods), &dir)){
        fprintf(stderr, "Error setting min #periods.\n");
        printf("min_periods: %u\n", (tone->periods));
        return -1;
    }
    printf("min_periods: %u\n", (tone->periods));

    if(snd_pcm_hw_params_set_periods(*handle, *params, tone->periods, dir)){
        fprintf(stderr, "Error setting #periods.\n");
        snd_pcm_hw_params_get_periods(*params, &(tone->periods), &dir);
        printf("periods: %u, %d", tone->periods, dir);
        return -1;
    }
    snd_pcm_hw_params_get_period_time(*params, period_time, &dir);
    unsigned int p;
    snd_pcm_hw_params_get_periods(*params, &p, &dir);
    printf("periods: %u\n", p);

    snd_output_t *output;

    snd_output_stdio_attach(&output, stdout, 0);
    snd_pcm_hw_params_dump(*params, output);
    snd_output_close(output);
    return 0;
}


tone_params *init_tone(){
    tone = malloc(sizeof(tone_params));
    snd_pcm_channel_area_t *areas = calloc(CHANNELS, sizeof(snd_pcm_channel_area_t));
    tone->areas = areas;
    tone->period_size = 16;
    tone->periods = 2;
    tone->phase=0;
    tone->sample_rate=44100;
    tone->freq=0;
    return tone;
}
void free_tone(tone_params *tone){
    free(tone->areas);
    free(tone);
}

void updatePCM(double v){
    tone->freq = v;
}

int runPCM () {
    signal(SIGINT, killPCM);

    long loops;
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    snd_pcm_uframes_t frames;

    signed short * samples; // 2 byte data size, because each frame is 2 bytes
    
    unsigned int period_time;

    tone = init_tone();

    if (open_playback_device(&handle, &params, tone, &period_time, "default")){
        return -1;
    }

    printf("period size: %lu\n",tone->period_size);
    printf("period time: %u\n",period_time);
    // Allocate the sample and area buffers
    samples = malloc((tone->period_size * CHANNELS * snd_pcm_format_physical_width(FORMAT)) / 8);

    // set up the area buffer for each channel
    for (int chn = 0; chn < CHANNELS; chn++) {
        tone->areas[chn].addr = samples;
        tone->areas[chn].first = chn * snd_pcm_format_physical_width(FORMAT);
        tone->areas[chn].step = CHANNELS * snd_pcm_format_physical_width(FORMAT);
    }
    /* 5 seconds in microseconds divided by
    * period time */
    //loops = TIME / period_time;

    //for (; loops > 0; loops--) {
    while(running){
        int err;
        if ((err = snd_pcm_wait (handle, 1000)) < 0) {
            fprintf (stderr, "poll failed (%s)\n", strerror (errno));
            break;
        }

        /* find out how much space is available for playback data */
        int frames_to_deliver;
        if ((frames_to_deliver = snd_pcm_avail_update (handle)) < 0) {
            if (frames_to_deliver == -EPIPE) {
                fprintf (stderr, "an xrun occured\n");
                break;
            } else {
                fprintf (stderr, "unknown ALSA avail update return value (%d)\n", 
                     frames_to_deliver);
                break;
            }
        }

        int dead_space = (tone->period_size * tone->periods - 8192);
        frames_to_deliver -= dead_space;
        if (frames_to_deliver < 0) continue;

        frames_to_deliver = (frames_to_deliver < tone->period_size) ? tone->period_size : frames_to_deliver;

        /* deliver the data */
        while (frames_to_deliver > tone->period_size){
            write_samples(handle, samples, tone, &frames_to_deliver);
        }
    }

    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    free(samples);
    free_tone(tone);
}

// int main(){
//     runPCM();
// }