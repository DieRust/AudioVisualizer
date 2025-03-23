// please.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <process.h>
#include "portAudioFunctions.h"

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
