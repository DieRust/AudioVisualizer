#include "portAudioFunctions.h"

enum Color {
    white,
    red,
    blue,
    green,
    yellow,
    purple,
    cyan,
};


void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void colorPicker(enum color c) {
    switch (c) {
    case 0:
        printf("\033[0;37m");
        break;
    case 1:
        printf("\033[0;31m");
        break;
    case 2:
        printf("\033[0;34m");
        break;
    case 3:
        printf("\033[0;32m");
        break;
    case 4:
        printf("\033[0;33m");
        break;
    case 5:
        printf("\033[0;35m");
        break;
    case 6:
        printf("\033[0;36m");
        break;
    }
}

void drawBar(int x, int y, int totalHeight, int height) {
    for (int i = 0; i < totalHeight; i++) {
        gotoxy(x, y - i);
        colorPicker(green);
        if (i < height) {
            if (i >= floor(0.3 * totalHeight)) {
                colorPicker(yellow);
            }
            if (i > 0.6 * totalHeight) {
                colorPicker(red);
            }
            printf("[]");
        }
        else {
            printf("  ");
        }
    }
}





void checkErr(PaError err) {
    if (err != paNoError) {
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
        exit(EXIT_FAILURE);
    }
}

int setDevice() {
    int choise;
    int numDevices = Pa_GetDeviceCount();
    printf("Number of devices: %d\n", numDevices);

    if (numDevices < 0) {
        printf("Error getting device count.\n");
        exit(EXIT_FAILURE);
    }
    else if (numDevices == 0) {
        printf("There are no available audio devices on this machine.\n");
        exit(EXIT_SUCCESS);
    }
    const PaDeviceInfo* deviceInfo;
    for (int i = 0; i < numDevices; i++) {
        deviceInfo = Pa_GetDeviceInfo(i);
        printf("Device %d:\n", i);
        printf("  name: %s\n", deviceInfo->name);
        printf("  maxInputChannels: %d\n", deviceInfo->maxInputChannels);
        printf("  maxOutputChannels: %d\n", deviceInfo->maxOutputChannels);
        printf("  defaultSampleRate: %f\n", deviceInfo->defaultSampleRate);
        printf("----------------------------\n");
    }
    printf("please enter number of your device: ");
    scanf_s("%d", &choise);
    return choise;
}

streamCallbackData* allocateSpectroMemory(double sampleRate) {
    streamCallbackData* spectroData = (streamCallbackData*)malloc(sizeof(streamCallbackData));
    spectroData->out = (double*)malloc(sizeof(double) * FRAMES_PER_BUFFER);
    spectroData->in = (double*)malloc(sizeof(double) * FRAMES_PER_BUFFER);
    if (spectroData->in == NULL || spectroData->out == NULL) {
        printf("cannot allocate memmory for calculations\n");
        exit(EXIT_FAILURE);
    }
    spectroData->p = fftw_plan_r2r_1d(FRAMES_PER_BUFFER, spectroData->in, spectroData->out, FFTW_R2HC, FFTW_ESTIMATE);

    double sampleRatio = FRAMES_PER_BUFFER / sampleRate;
    spectroData->startIndex = ceil(SPECTRO_FREQ_MIN * sampleRatio);
    spectroData->spectroSize = MIN(ceil(SPECTRO_FREQ_MAX * sampleRatio), FRAMES_PER_BUFFER / 2.0) - spectroData->startIndex;

    return spectroData;
}

void setInputParameters(PaStreamParameters* inputParameters, int inputDevice) {
    memset(inputParameters, 0, sizeof(PaStreamParameters));
    inputParameters->channelCount = NUM_CHANNELS;
    inputParameters->device = inputDevice;
    inputParameters->hostApiSpecificStreamInfo = NULL;
    inputParameters->sampleFormat = paFloat32;
    inputParameters->suggestedLatency = Pa_GetDeviceInfo(inputDevice)->defaultLowInputLatency;
}

int patestCallback(
    const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
    void* userData
) {
    float* in = (float*)inputBuffer;
    (void)outputBuffer;
    streamCallbackData* callbackData = (streamCallbackData*)userData;

    for (unsigned long i = 0; i < framesPerBuffer; i++) {
        callbackData->in[i] = in[i * NUM_CHANNELS];
    }

    fftw_execute(callbackData->p);

    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

    int maxX = info.srWindow.Right - info.srWindow.Left - 1;
    int maxY = info.srWindow.Bottom - info.srWindow.Top;
    int dispSize = maxX / 2;
    int height = maxY * 3 / 5;
    //system("cls");

    for (int i = 0; i < dispSize; i++) {
        // Sample frequency data logarithmically
        double proportion = pow(i / (double)dispSize, 4);
        double freq = callbackData->out[(int)(callbackData->startIndex
            + proportion * callbackData->spectroSize)];

        // Display full block characters with heights based on frequency intensity
        if (freq < 0.125) {
            drawBar(i * 2, maxY, height, 0.125 * height);
        }
        else if (freq < 0.25) {
            drawBar(i * 2, maxY, height, 0.25 * height);
        }
        else if (freq < 0.375) {
            drawBar(i * 2, maxY, height, 0.375 * height);
        }
        else if (freq < 0.5) {
            drawBar(i * 2, maxY, height, 0.5 * height);
        }
        else if (freq < 0.625) {
            drawBar(i * 2, maxY, height, 0.625 * height);
        }
        else if (freq < 0.75) {
            drawBar(i * 2, maxY, height, 0.75 * height);
        }
        else if (freq < 0.875) {
            drawBar(i * 2, maxY, height, 0.875 * height);
        }
        else {
            drawBar(i * 2, maxY, height, height);
        }
    }


    //fflush(stdout);
    gotoxy(0, 0);
    return 0;
}