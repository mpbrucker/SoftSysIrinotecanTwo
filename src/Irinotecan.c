#include "Irinotecan.h"
#include "keyInput.h"
#include "pcm.h"

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

double Key2Freq(int keyCode){
    double freq = 0;
    switch(keyCode){
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
        case 44: //A#4
            freq = 466.16;
            break;
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
        
        
        
        
        case 27: //F5
            freq = 698.46;
            break;
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
            freq = 1567.98;
            break;
        default:
            break;
    }
}

void setKey(int keyCode){
    pthread_mutex_lock(&mutex);
    currkey = keyCode;
    //Update PCM
    updatePCM(Key2Freq(keyCode));
    pthread_mutex_unlock(&mutex);
}

void releaseKey(int keyCode){
    pthread_mutex_lock(&mutex);
    if (keyCode == currkey){
        currkey = -1;
        //Update PCM
        updatePCM(0);
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