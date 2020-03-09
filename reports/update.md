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

## What we've done so far

So far, we've implemented basic note-playing functionality using ALSA as the interface with the sound hardware. For resources, we've been using a [few](https://www.linuxjournal.com/article/6735) [tutorials](https://www.alsa-project.org/alsa-doc/alsa-lib/_2test_2pcm_8c-example.html) that cover the basics of using ALSA's PCM library, as well as the math behind waveform generation. For input buffering, we're following some [tutorials](http://www.cs.ukzn.ac.za/~hughm/os/notes/ncurses.html) to implement basic input buffering that is relatively fast.

## What we're doing now

Currently, we are:
* Debugging memory issues with the sound playback code (Matt). This will be done once a tone of a given frequency can be correctly generated (currently, it always plays the same note).
* Refactoring the sound playback code to be more readable, using structs to store parameters (Matt). This will be done once all functions in pcm.c make use of the struct we created to hold sound parameters.
* Implementing simple input buffering (Will). This will be done once we have a function that returns the value of the current key being pressed, or -1 or a similar value if no keys are pressed.