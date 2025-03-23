// please.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include <process.h>
#include <portaudio.h>
#include <fftw3.h>

#define FRAMES_PER_BUFFER 512
#define NUM_CHANNELS 2
#define SPECTRO_FREQ_MIN 20
#define SPECTRO_FREQ_MAX 20000


typedef struct {
    double* in;
    double* out;
    fftw_plan p;
    int startIndex;
    int spectroSize;
} streamCallbackData;

static streamCallbackData* spectroData;

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

static void checkErr(PaError err) {
    if (err != paNoError) {
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
        exit(EXIT_FAILURE);
    }
}

static float abs(float a) {
    return a >= 0 ? a : -a;
}

static double Min(double a, double b) {
    return a <= b ? a : b;
}

static int patestCallback(
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
    spectroData->spectroSize = Min(ceil(SPECTRO_FREQ_MAX * sampleRatio), FRAMES_PER_BUFFER / 2.0) - spectroData->startIndex;

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

HANDLE myThread[1] = { NULL };
HANDLE runMutex;

void threadMision(void* pMyId) {
    do {
        Pa_Sleep(1);
    } while (WaitForSingleObject(runMutex, 75l) == WAIT_TIMEOUT);
}

void mainLoop() {
    printf("Press q to exit");
    int exitChar;
    runMutex = CreateMutexW(NULL, TRUE, NULL);
    myThread[0] = (HANDLE)_beginthread(threadMision, 0, NULL);
    do {
        exitChar = _getch();
    } while (tolower(exitChar) != 'q');

    ReleaseMutex(runMutex);
    WaitForSingleObject(myThread[0], INFINITE);
}

void clear_screen()
{
    printf(
        "\033[2J"       // clear the screen
        "\033[1;1H");  // move cursor home
}

int main() {
    PaError err;
    err = Pa_Initialize();
    checkErr(err);

    printf("here is list of your music device please choose one with name: streo mix\n");
    printf("if you get error invald device you must turn on your stero mix driver in windows\n");


    int inputDevice = setDevice();
    double sampleRate = Pa_GetDeviceInfo(inputDevice)->defaultSampleRate;
    streamCallbackData* spectroData = allocateSpectroMemory(sampleRate);
    //system("CLS");
    clear_screen();
    PaStreamParameters inputParameters;

    setInputParameters(&inputParameters, inputDevice);


    PaStream* stream;
    err = Pa_OpenStream(
        &stream,
        &inputParameters,
        NULL,
        sampleRate,
        FRAMES_PER_BUFFER,
        paNoFlag,
        patestCallback,
        spectroData
    );
    checkErr(err);

    err = Pa_StartStream(stream);
    checkErr(err);
    clear_screen();

    mainLoop();

    err = Pa_StopStream(stream);
    checkErr(err);

    err = Pa_CloseStream(stream);
    checkErr(err);

    err = Pa_Terminate();
    checkErr(err);

    fftw_destroy_plan(spectroData->p);
    fftw_free(spectroData->in);
    fftw_free(spectroData->out);
    free(spectroData);


    return EXIT_SUCCESS;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
