#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32

    #include <windows.h>
    #include <conio.h>

    /* Windows: read one key without displaying it */
    static inline int platform_getch(void)
    {
        return _getch();
    }

    /* Windows: clear terminal */
    static inline void platform_clear_screen(void)
    {
        system("cls");
    }

    /* Windows: delay in milliseconds */
    static inline void platform_sleep_ms(unsigned int milliseconds)
    {
        Sleep(milliseconds);
    }

#else

    #include <termios.h>
    #include <unistd.h>
    #include <time.h>

    /* Ubuntu/Linux: read one key without displaying it */
    static inline int platform_getch(void)
    {
        struct termios oldTerm, newTerm;
        unsigned char ch;

        if (tcgetattr(STDIN_FILENO, &oldTerm) != 0)
            return EOF;

        newTerm = oldTerm;
        newTerm.c_lflag &= (tcflag_t) ~(ICANON | ECHO);

        if (tcsetattr(STDIN_FILENO, TCSANOW, &newTerm) != 0)
            return EOF;

        ssize_t result = read(STDIN_FILENO, &ch, 1);

        tcsetattr(STDIN_FILENO, TCSANOW, &oldTerm);

        return result == 1 ? ch : EOF;
    }

    /* Ubuntu/Linux: clear terminal */
    static inline void platform_clear_screen(void)
    {
        system("clear");
    }

    /* Ubuntu/Linux: delay in milliseconds */
    static inline void platform_sleep_ms(unsigned int milliseconds)
    {
        struct timespec delay;

        delay.tv_sec = milliseconds / 1000;
        delay.tv_nsec = (long)(milliseconds % 1000) * 1000000L;

        nanosleep(&delay, NULL);
    }

#endif

#endif