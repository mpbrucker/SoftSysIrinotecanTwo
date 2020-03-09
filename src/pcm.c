#define ALSA_PCM_NEW_HW_PARAMS_API
#define TIME 2000000
#define FORMAT SND_PCM_FORMAT_S16_LE
#define CHANNELS 1 // number of audio channels

#include "pcm.h"

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
    int phys_bps = snd_pcm_format_physical_width(FORMAT) / 8;
    int big_endian = snd_pcm_format_big_endian(FORMAT) == 1;
    int to_unsigned = snd_pcm_format_unsigned(FORMAT) == 1;
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

int write_samples(snd_pcm_t *handle, signed short *samples, tone_params * params) {
    signed short *ptr;
    int bytes_written, remaining;
    sample_sine(params);
    ptr = samples;
    remaining = params->period_size;
    while (remaining > 0) {
        bytes_written = snd_pcm_writei(handle, ptr, remaining); // TODO: error check this
        if (bytes_written == -EAGAIN)
                continue;
            if (bytes_written < 0) {
                break;  /* skip one period */
            }
        ptr += bytes_written * CHANNELS;
        remaining -= bytes_written;
    }
}


// Opens a specified playback device and sets hardware settings.
void open_playback_device(snd_pcm_t **handle, snd_pcm_hw_params_t **params, tone_params * tone, unsigned int * period_time, char * dev_name) {
    int dir;
    int res = snd_pcm_open(handle, dev_name, SND_PCM_STREAM_PLAYBACK, SND_PCM_ASYNC);

    // Allocate hardware parameters object and set defaults.
    snd_pcm_hw_params_alloca(params);
    snd_pcm_hw_params_any(*handle, *params);
    // Set format and channels.
    snd_pcm_hw_params_set_format(*handle, *params, FORMAT);
    snd_pcm_hw_params_set_channels(*handle, *params, CHANNELS);
    // Set sample rate and period size (_near sets parameter space as close as possible)
    snd_pcm_hw_params_set_rate_near(*handle, *params, &(tone->sample_rate), &dir);
    snd_pcm_hw_params_set_period_size_near(*handle, *params, &(tone->period_size), &dir);

    res = snd_pcm_hw_params(*handle, *params);
    // Get period size and time
    snd_pcm_hw_params_get_period_size(*params, &(tone->period_size), &dir);
    snd_pcm_hw_params_get_period_time(*params, period_time, &dir);
}


int main () {
    long loops;
    int size;
    int res;
    double phase = 0;
    double freq = 440;
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;

    signed short * samples; // 2 byte data size, because each frame is 2 bytes
    snd_pcm_channel_area_t *areas;

    snd_pcm_uframes_t period_size = 16; // period size
    unsigned int sample_rate = 44100;
    unsigned int period_time;
    int dir;

    tone_params tone = {
        areas=areas,
        period_size=16,
        phase=0,
        sample_rate=44100,
        freq=440
    };

    open_playback_device(&handle, &params, &tone, &period_time, "default");


    // Allocate the sample and area buffers
    samples = malloc((period_size * CHANNELS * snd_pcm_format_physical_width(FORMAT)) / 8);
    areas = calloc(CHANNELS, sizeof(snd_pcm_channel_area_t));

    // set up the area buffer for each channel
    for (int chn = 0; chn < CHANNELS; chn++) {
        areas[chn].addr = samples;
        areas[chn].first = chn * snd_pcm_format_physical_width(FORMAT);
        areas[chn].step = CHANNELS * snd_pcm_format_physical_width(FORMAT);
    }

    /* 5 seconds in microseconds divided by
    * period time */
    loops = TIME / period_time;

    for (; loops > 0; loops--) {
        write_samples(handle, samples, &tone);
    }

    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    free(samples);
    free(areas);

    return 0;
}
