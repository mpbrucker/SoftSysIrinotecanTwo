# Irinotecan: a Digital Synthesizer in C
### William Derksen, Matt Brucker

## Main Idea

The objective of this project is to create a real-time synthesizer in C that uses keyboard input to play computer-generated notes.

### Minimum Valuable Synthesizer

At a minimum, our synthesizer should be able to read keyboard input in real-time and play a single simple (sine wave) note for the duration of a keypress, with a different note playing depending on which key is pressed. In the MVP version of our project, we will use a library such as libasound to interface with ALSA (the audio hardware interface for Linux).

### Stretch Goals

The stretch goals for this project include handling playing multiple notes at the same time, different types of generic waveforms, and basic audio effects such as frequency modulation. 

## Team Learning Goals

**William:** Learn how to write optimized C code and design software that runs quickly in real-time.

**Matt:** Learn more about how to implement input buffering in C, and gain experience implmenting and using C data structures.

## Useful resources

So far, we've implemented basic note-playing functionality using ALSA as the interface with the sound hardware. For resources, we've been using a [few](https://www.linuxjournal.com/article/6735) [tutorials](https://www.alsa-project.org/alsa-doc/alsa-lib/_2test_2pcm_8c-example.html) that cover the basics of using ALSA's PCM library, as well as the math behind waveform generation. For input buffering, we're following some [tutorials](http://www.cs.ukzn.ac.za/~hughm/os/notes/ncurses.html) to implement basic input buffering that is relatively fast.

## What we made

We reached our MVP - a window pops up, and if you're focused on it you can press a key and a corresponding note will play. We used XLib to capture key input. A key part of our input processing code (found in `keyInput.c`) is our use of multithreading; we use a mutex to allow the key press input code to change the frequency variable used by the playback code:

```
void setKey(int keyCode){
    pthread_mutex_lock(&mutex);
    currkey = keyCode;
    //Update PCM
    updatePCM(Key2Freq(keyCode));
    pthread_mutex_unlock(&mutex);
}
```

Here, updatePCM updates the freq variable used in `pcm.c`. `key2Freq` maps from numeric key codes to frequencies (in Hz). In `pcm.c,` the frequency is used to generate the sound data:

```
while (count-- > 0) {
    res = sin(phase) * maxval;
    for (chn = 0; chn < CHANNELS; chn++) {
        /* Generate data in big endian format */
        for (int i = 0; i < bps; i++) {
            *(samples[chn] + i) = (res >>  i * 8) & 0xff;
        }
        samples[chn] += steps[chn];
    }
    phase += step;
    if (phase >= max_phase)
        phase -= max_phase;
}
```

Here, `res` is the sampled sine wave data. The inner `for` loop converts the sound data (in byte form) into big endian format and updates the buffer; and `step` moves the sampling "forward" in the sine wave at a rate proportional to `freq/sample_rate`. This buffer (`samples`) is later written to hardware using ALSA. Together, these functions allow the user to change the frequency of the tone being played using key presses.

## Implementing playback parameters

One design decision we made was to implement playback parameters in `pcm.c` using a struct (`tone_params`) instead of the original implementation, which included all variables as separate function parameters. We chose to combine several parameters of `sample_sine` into a struct, because `pcm.c` has a lot of functions passing the same set of parameters around; putting them into a struct made function signatures easier to read, and it also allowed us to update function parameters much more easily if needed. However, adding a struct for playback parameters did make debugging more difficult; in several of the functions, we "unpack" and make copies of some of the variables in the struct, which caused issues when the struct variables were updated by `open_playback_device` leading to memory issues. 

## Reflection

Ultimately, we tried our best and we did succeed at finishing our MVP, which feels like an accomplishment given the bumps in the road we experienced along the way. We were not able to fully fix some issues with the ALSA library causing a slight delay, but we realized pretty quickly that fixing this would require us getting deep into understanding the intricacies of sound hardware and ALSA, and given that the documentation for ALSA is quite confusing we decided not to pursue it further. However, although it isn't as performant as we would like it to be, it does work and it fulfills the criteria defined by our MVP.

**Matt:** While I didn't end up learning about input buffering due to how we split up the work, I do feel like I got a good amount of experience working with structs which I found pretty enlightening, and I learned a *lot* about working with C libraries through our use of ALSA. In the future I would take a more bottom-up approach to usage of a complex library like ALSA and prioritize understanding everything that's happening over getting code running.

**Will:**

**Trello board:** [link](https://trello.com/b/RNBfHLNZ/irinotecan)
**Github repo:** [link](https://github.com/mpbrucker/SoftSysIrinotecanTwo)