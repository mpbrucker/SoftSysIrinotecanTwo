# Digital Synthesizers in C
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

## Project Resources

In addition to the [documentation](https://www.alsa-project.org/alsa-doc/alsa-lib/) for the ALSA C library, we plan to look at existing utilities and ALSA wrapper libraries, such as [Ardour](https://github.com/Ardour/ardour), [alsaaudio](https://larsimmisch.github.io/pyalsaaudio/pyalsaaudio.html#what-is-alsa), [LMMS](https://github.com/LMMS/lmms), etc. We are still looking for more resources on how to do effective and performant input buffering.

## Next Steps

* Search for resources on how best to implement input buffering. We will know we are done with this once we've found a resource that tells us how to implement input buffering.
* Write a first-pass implementation of real-time input buffering. This task will be completed by our creating a working C program that can take keyboard input in real-time (and display it by printing it out).
* Write a first-pass version of playing sound using the ALSA C library. We'll know we're done with this task when we've created a compiling C program that plays a continuous note for some amount of time.
