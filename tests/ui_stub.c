#include <stdio.h>
#include "ui.h"

/* Stand-in for ui.c, only for testing on Mac/Linux.
   It draws the same frame as the real logo, so the menu box below
   connects to it: top line, title row, then the middle line (╠═══╣). */

void printLogo(void)
{
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                          AURA FLEX - GYM & FITNESS MANAGEMENT                  ║\n");
    printf("╠══════════════════════════════════╦═════════════════════════════════════════════╣\n");
}

void printLogoWithoutl(void)
{
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                          AURA FLEX - GYM & FITNESS MANAGEMENT                  ║\n");
    printf("╠════════════════════════════════════════════════════════════════════════════════╣\n");
}