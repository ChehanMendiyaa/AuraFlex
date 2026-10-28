#include <stdio.h>
#include "ui.h"
#include <windows.h>
#include <time.h>

// void printLogo(void)
// {
//     SetConsoleOutputCP(CP_UTF8);
//     SetConsoleCP(CP_UTF8);
//     printf("\n");
//     printf("╠════════════════════════════════════════════════════════════════════════════════╣\n");
//     printf("║                                                                                ║\n");
//     printf("║      █████╗ ██╗   ██╗██████╗  █████╗      ███████╗██╗     ███████╗ ██╗   ██╗   ║\n");
//     printf("║     ██╔══██╗██║   ██║██╔══██╗██╔══██╗     ██╔════╝██║     ██╔════╝ ╚██╗ ██╔╝   ║\n");
//     printf("║     ███████║██║   ██║██████╔╝███████║     █████╗  ██║     █████╗    ╚███╔╝     ║\n");
//     printf("║     ██╔══██║██║   ██║██╔══██╗██╔══██║     ██╔══╝  ██║     ██╔══╝    ██╔██╗     ║\n");
//     printf("║     ██║  ██║╚██████╔╝██║  ██║██║  ██║     ██║    ███████╗██████╗  ██╔╝   ██╗   ║\n");
//     printf("║     ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝     ╚═╝    ╚══════╝╚══════╝ ╚═╝     ╚═╝  ║\n");
//     printf("║                                                                                ║\n");
//     printf("║                            GYM & FITNESS MANAGEMENT                            ║\n");
//     printf("╠══════════════════════════════════╦═════════════════════════════════════════════╣\n");

// }

void printLogo(void)
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                                ║\n");
    printf("║      █████╗ ██╗   ██╗██████╗  █████╗      ███████╗██╗     ███████╗ ██╗   ██╗   ║\n");
    printf("║     ██╔══██╗██║   ██║██╔══██╗██╔══██╗     ██╔════╝██║     ██╔════╝ ╚██╗ ██╔╝   ║\n");
    printf("║     ███████║██║   ██║██████╔╝███████║     █████╗  ██║     █████╗    ╚███╔╝     ║\n");
    printf("║     ██╔══██║██║   ██║██╔══██╗██╔══██║     ██╔══╝  ██║     ██╔══╝    ██╔██╗     ║\n");
    printf("║     ██║  ██║╚██████╔╝██║  ██║██║  ██║     ██║    ███████╗██████╗  ██╔╝   ██╗   ║\n");
    printf("║     ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝     ╚═╝    ╚══════╝╚══════╝ ╚═╝     ╚═╝  ║\n");
    printf("║                                                                                ║\n");
    printf("║                          GYM & FITNESS MANAGEMENT                              ║\n");
    printf("╠══════════════════════════════════╦═════════════════════════════════════════════╣\n");
}

void printLogoWithoutl(void)
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                                ║\n");
    printf("║      █████╗ ██╗   ██╗██████╗  █████╗      ███████╗██╗     ███████╗ ██╗   ██╗   ║\n");
    printf("║     ██╔══██╗██║   ██║██╔══██╗██╔══██╗     ██╔════╝██║     ██╔════╝ ╚██╗ ██╔╝   ║\n");
    printf("║     ███████║██║   ██║██████╔╝███████║     █████╗  ██║     █████╗    ╚███╔╝     ║\n");
    printf("║     ██╔══██║██║   ██║██╔══██╗██╔══██║     ██╔══╝  ██║     ██╔══╝    ██╔██╗     ║\n");
    printf("║     ██║  ██║╚██████╔╝██║  ██║██║  ██║     ██║    ███████╗██████╗  ██╔╝   ██╗   ║\n");
    printf("║     ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝     ╚═╝    ╚══════╝╚══════╝ ╚═╝     ╚═╝  ║\n");
    printf("║                                                                                ║\n");
    printf("║                          GYM & FITNESS MANAGEMENT                              ║\n");
    printf("╠════════════════════════════════════════════════════════════════════════════════╣\n");
}
void displayDateTime(void)
{
    time_t now = time(NULL);
    struct tm *currentTime = localtime(&now);

    if (currentTime == NULL) {
        return;
    }

    printf("Date: %04d-%02d-%02d | Time: %02d:%02d:%02d\n",
           currentTime->tm_year + 1900,
           currentTime->tm_mon + 1,
           currentTime->tm_mday,
           currentTime->tm_hour,
           currentTime->tm_min,
           currentTime->tm_sec);
}

const char *getDateTime(void)
{
    static char dateTime[30];

    time_t now = time(NULL);
    struct tm *currentTime = localtime(&now);

    if (currentTime == NULL) {
        return "";
    }

    strftime(dateTime, sizeof(dateTime),
             "%Y-%m-%d ║ %H:%M:%S", currentTime);

    return dateTime;
}