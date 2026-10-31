#include <stdio.h>
#include "platform.h"
#include "system.h"

/*
 * Clear the terminal screen.
 * Windows: cls
 * Ubuntu/Linux: clear
 */
void clearScreen(void)
{
    platform_clear_screen();
}

/*
 * Wait for 2 seconds and then clear the terminal.
 * Windows: Sleep()
 * Ubuntu/Linux: nanosleep()
 */
void clearScreen2s(void)
{
    platform_sleep_ms(2000);
    platform_clear_screen();
}