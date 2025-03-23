#ifndef _TERMINALFUNCTIONS

#define _TERMINALFUNCTIONS

#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "portAudioFunctions.h"

enum Color {
    white,
    red,
    blue,
    green,
    yellow,
    purple,
    cyan
};

void gotoxy(int x, int y);
void colorPicker(enum color c);
void drawBar(int x, int y, int totalHeight, int height);

#endif // !_TERMINALFUNCTIONS
