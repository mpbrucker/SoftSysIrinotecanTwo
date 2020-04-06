#include "Irinotecan.h"
#include "keyInput.h"
#include "pcm.h"
#include "hashset_itr.h"

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#define SEMITONE 1.059463094359295

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
hashset_t keys;
double multiplier = 1;


double Key2Freq(int keyCode){
    double freq = 0;
    switch(keyCode){
        case 66: //A#3
            freq = 233.08;
            break;
        case 50: //B3
            freq = 246.94;
            break;
        case 52: //C4
            freq = 261.63;
            break;
        case 39: //C#4
            freq = 277.18;
            break;
        case 53: //D4
            freq = 293.66;
            break;
        case 40: //D#4
            freq = 311.13;
            break;
        case 54: //E4
            freq = 329.63;
            break;
        case 55: //F4
            freq = 349.23;
            break;
        case 42: //F#4
            freq = 369.99;
            break;
        case 56: //G4
            freq = 392.00;
            break;
        case 43: //G#4
            freq = 415.30;
            break;
        case 57: //A4
            freq = 440.00;
            break;
        case 49: //A#4
        case 44: //A#4
            freq = 466.16;
            break;

        case 23: //B4
        case 58: //B4
            freq = 493.88;
            break;

        case 59: //C5
        case 24: //C5
            freq = 523.25;
            break;

        case 46: //C#5
        case 11: //C#5
            freq = 554.37;
            break;

        case 60: //D5
        case 25: //D5
            freq = 587.33;
            break;

        case 47: //D#5
        case 12: //D#5
            freq = 622.25;
            break;

        case 61: //E5
        case 26: //E5
            freq = 659.25;
            break;
        
        
        
        case 62: //F5
        case 27: //F5
            freq = 698.46;
            break;
        case 36: //F#5
        case 14: //F#5
            freq = 739.99;
            break;
        case 28: //G5
            freq = 783.99;
            break;
        case 15: //G#5
            freq = 830.61;
            break;
        case 29: //A5
            freq = 880.00;
            break;
        case 16: //A#5
            freq = 932.33;
            break;
        case 30: //B5
            freq =  987.77;
            break;
        case 31: //C6
            freq = 1046.50;
            break;
        case 18: //C#6
            freq = 1108.73;
            break;
        case 32: //D6
            freq = 1174.66;
            break;
        case 19: //D#6
            freq = 1244.51;
            break;
        case 33: //E6
            freq = 1318.51;
            break;
        case 34: //F6
            freq = 1396.91;
            break;
        case 21: //F#6
            freq = 1479.98;
            break;
        case 35: //G6
        case 67: //G6
            freq = 1567.98;
            break;
        case 22: //G#6
        case 68: //G#6
            freq = 1661.22;
            break;
        case 69: //A6
        case 51: //A6
            freq = 1760.00;
            break;

        case 70: //A#6
            freq = 1864.66;
            break;
        case 71: //B6
            freq = 1975.53;
            break;
        case 72: //C6
            freq = 2093.00;
            break;
        case 73: //C#6
            freq = 2217.46;
            break;
        case 74: //D6
            freq = 2349.32;
            break;
        case 75: //D#6
            freq = 2489.02;
            break;
        case 76: //E6
            freq = 2637.02;
            break;

        case 113: //Left Arrow
            multiplier *= 1/SEMITONE;  // semitones
            soundModPrint();
            break;
        case 114: //Right Arrow
            multiplier *= SEMITONE;  // semitones
            soundModPrint();
            break;
        case 111: //Up Arrow
            multiplier *= 2;  // octaves
            soundModPrint();
            break;
        case 116: //Down Arrow
            multiplier *= .5;  // octaves
            soundModPrint();
            break;
        default:
            break;
    }
    return freq;
}

void soundModPrint(){
    double octs = log2(multiplier);
    int octaves = octs>0 ? floor(octs) : ceil(octs);
    double semis = log(multiplier*pow(2, -octaves))/log(SEMITONE);
    int semitones = round(semis);

    printf("\nsound modification: %d octave, %d semitones\n", octaves, semitones);
}

void KeyProcessing(){
    //printf("Set: ");
    hashset_itr_t hitr = hashset_iterator(keys);
    int i = -1;
    do{
        int val = (int)hashset_iterator_value(hitr);
        if (val != 0 && val != 1){
            i++;
            updatePCM(Key2Freq(val)*multiplier, i); //Update set values
        }
    } while (hashset_iterator_next(hitr) != -1 && i < MAXNOTES);
    i++;
    for (;i < MAXNOTES; i++){
        updatePCM(0, i); //set rest to 0 hz
    }
    //printf("\n total length: %lu\n", hashset_num_items(keys));
    //Key2Freq(keyCode);
}

void setKey(int keyCode){
    pthread_mutex_lock(&mutex);
    if (hashset_add(keys, (void *) (size_t)keyCode)){
        //printf("added: %d\n", keyCode);
    }
    //Update PCM
    KeyProcessing();
    
    pthread_mutex_unlock(&mutex);
}

void releaseKey(int keyCode){
    pthread_mutex_lock(&mutex);
    if (hashset_remove(keys, (void *) (size_t)keyCode)){
        //printf("removed: %d\n", keyCode);
        KeyProcessing();
    }
    
    pthread_mutex_unlock(&mutex);
}

int main(){
    pthread_t tid;
    int err;
    err = pthread_create(&tid, NULL, keyCapture, NULL); // Start keyCapture
    if (err) {
        printf("Error:unable to create thread, %d\n", err);
        exit(-1);
    }
    // Run PCM
    runPCM();
}