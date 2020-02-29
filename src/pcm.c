#define ALSA_PCM_NEW_HW_PARAMS_API
#define TIME 5000000
#define FORMAT SND_PCM_FORMAT_S16_LE
#define CHANNELS 1 // number of audio channels

#include "pcm.h"

static void sample_sine(const snd_pcm_channel_area_t *areas, int count, double *_phase)
{
    static double max_phase = 2. * M_PI;
    double phase = *_phase;
    double step = max_phase*freq/(double)rate;
    unsigned char *samples[channels];
    int steps[channels];
    unsigned int chn;
    int format_bits = snd_pcm_format_width(format);
    unsigned int maxval = (1 << (format_bits - 1)) - 1;
    int bps = format_bits / 8;  /* bytes per sample */
    int phys_bps = snd_pcm_format_physical_width(format) / 8;
    int big_endian = snd_pcm_format_big_endian(format) == 1;
    int to_unsigned = snd_pcm_format_unsigned(format) == 1;
    int is_float = (format == SND_PCM_FORMAT_FLOAT_LE ||
            format == SND_PCM_FORMAT_FLOAT_BE);
    /* verify and prepare the contents of areas */
    for (chn = 0; chn < channels; chn++) {
        if ((areas[chn].first % 8) != 0) {
            printf("areas[%u].first == %u, aborting...\n", chn, areas[chn].first);
            exit(EXIT_FAILURE);
        }
        samples[chn] = /*(signed short *)*/(((unsigned char *)areas[chn].addr) + (areas[chn].first / 8));
        if ((areas[chn].step % 16) != 0) {
            printf("areas[%u].step == %u, aborting...\n", chn, areas[chn].step);
            exit(EXIT_FAILURE);
        }
        steps[chn] = areas[chn].step / 8;
        samples[chn] += offset * steps[chn];
    }
    /* fill the channel areas */
    while (count-- > 0) {
        union {
            float f;
            int i;
        } fval;
        int res, i;
        if (is_float) {
            fval.f = sin(phase);
            res = fval.i;
        } else
            res = sin(phase) * maxval;
        if (to_unsigned)
            res ^= 1U << (format_bits - 1);
        for (chn = 0; chn < channels; chn++) {
            /* Generate data in native endian format */
            if (big_endian) {
                for (i = 0; i < bps; i++)
                    *(samples[chn] + phys_bps - 1 - i) = (res >> i * 8) & 0xff;
            } else {
                for (i = 0; i < bps; i++)
                    *(samples[chn] + i) = (res >>  i * 8) & 0xff;
            }
            samples[chn] += steps[chn];
        }
        phase += step;
        if (phase >= max_phase)
            phase -= max_phase;
    }
    *_phase = phase;
}

int write_samples(snd_pcm_t *handle, signed short *samples, snd_pcm_channel_area_t *areas)
{
    double phase = 0;
    signed short *ptr;
    int err, cptr;
    while (1) {
        generate_sine(areas, 0, period_size, &phase);
        ptr = samples;
        cptr = period_size;
        while (cptr > 0) {
            err = snd_pcm_writei(handle, ptr, cptr);
            if (err == -EAGAIN)
                continue;
            if (err < 0) {
                if (xrun_recovery(handle, err) < 0) {
                    printf("Write error: %s\n", snd_strerror(err));
                    exit(EXIT_FAILURE);
                }
                break;  /* skip one period */
            }
            ptr += err * channels;
            cptr -= err;
        }
    }
}


// Opens a specified playback device and sets hardware settings.
void open_playback_device(snd_pcm_t **handle, snd_pcm_hw_params_t **params, snd_pcm_uframes_t * period_size, unsigned int * sample_rate, char * dev_name) {
    int dir;
    int res = snd_pcm_open(handle, dev_name, SND_PCM_STREAM_PLAYBACK, SND_PCM_ASYNC);

    // Allocate hardware parameters object and set defaults.
    snd_pcm_hw_params_alloca(params);
    snd_pcm_hw_params_any(*handle, *params);
    // Set format and channels.
    snd_pcm_hw_params_set_format(*handle, *params, FORMAT);
    snd_pcm_hw_params_set_channels(*handle, *params, CHANNELS);
    // Set sample rate and period size (_near sets parameter space as close as possible)
    snd_pcm_hw_params_set_rate_near(*handle, *params, sample_rate, &dir);
    snd_pcm_hw_params_set_period_size_near(*handle, *params, period_size, &dir);

    res = snd_pcm_hw_params(*handle, *params);
    // Get period size and time
    snd_pcm_hw_params_get_period_size(*params, period_size, &dir);
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
    char *samples;
    snd_pcm_uframes_t period_size = 16; // period size
    unsigned int sample_rate = 44100;

    open_playback_device(&handle, &params, &period_size, &sample_rate, "default");
    size = period_size * 2 * CHANNELS; /* 2 bytes/sample, 2 channels */
    samples = (char *) malloc(period_size * CHANNELS * snd_pcm_format_physical_width(FORMAT));

    /* 5 seconds in microseconds divided by
    * period time */
    loops = TIME / sample_rate;

    for (; loops > 0; loops--) {
        get_sample(samples, size);
        res = snd_pcm_writei(handle, samples, period_size);
    }

    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    free(samples);

    return 0;
}
