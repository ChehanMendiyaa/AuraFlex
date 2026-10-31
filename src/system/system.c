#include <stdlib.h>
#include <system.h>
#include "platform.h"

void clearScreen(void) {
    system("cls");
}

void clearScreen2s(void) {
    Sleep(2000);
    system("cls");
}
