#include "terminalFunctions.h"

void gotoxy(int x, int y) {
    COORD coord = {0,0};
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
