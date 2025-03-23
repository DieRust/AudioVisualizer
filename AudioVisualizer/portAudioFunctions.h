#ifndef _AUDIOFUNCTIONS
#define _AUDIOFUNCTIONS
#include <portaudio.h>
#include <stdlib.h>
#include <stdio.h>
#include <fftw3.h>
#include <math.h>
#include <windows.h>

#define FRAMES_PER_BUFFER 512
#define NUM_CHANNELS 2
#define SPECTRO_FREQ_MIN 20
#define SPECTRO_FREQ_MAX 20000

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

typedef struct a{
    double* in;
    double* out;
    fftw_plan p;
    int startIndex;
    int spectroSize;
} streamCallbackData;

void checkErr(PaError err);
int setDevice();
streamCallbackData* allocateSpectroMemory(double sampleRate);
void setInputParameters(PaStreamParameters* inputParameters, int inputDevice);
int patestCallback(
    const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
    void* userData
);

#endif // !_AUDIOFUNCTIONS
