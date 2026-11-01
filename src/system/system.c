#include <stdio.h>
#include "platform.h"
#include "system.h"

void clearScreen(void)
{
    platform_clear_screen();
}

void clearScreen2s(void)
{
    platform_sleep_ms(2000);
    platform_clear_screen();
}