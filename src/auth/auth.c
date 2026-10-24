
#include <stdio.h>
#include <string.h>
#include "auth.h"
#include "system.h"
#include "ui.h"

#define USERS_FILE "data/users.txt"
#define MAX_ATTEMPTS 3

int login(User *loggedUser)
{
    char username[30];
    char password[50];

    if (loggedUser == NULL) {
        return 0;
    }

    for (int attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {

        clearScreen();
        //printLogo();

        printf("\n");
        printf("  ╔══════════════════════════════════════════════════════════╗\n");
        printf("  ║                                                          ║\n");
        printf("  ║                      USER LOGIN                          ║\n");
        printf("  ║                                                          ║\n");
        printf("  ╠══════════════════════════════════════════════════════════╣\n");
        printf("  ║                                                          ║\n");
        printf("  ║  Enter your credentials to access AuraFlex.              ║\n");
        printf("  ║                                                          ║\n");
        printf("  ║  Username:                                               ║\n");
        printf("  ║  > ");

        if (scanf("%29s", username) != 1) {
            return 0;
        }

        printf("  ║                                                          ║\n");
        printf("  ║  Password:                                               ║\n");
        printf("  ║  > ");

        if (scanf("%49s", password) != 1) {
            return 0;
        }

        //printf("\n");
        printf("  ║                                                          ║\n");
        printf("  ╚══════════════════════════════════════════════════════════╝\n");

        FILE *file = fopen(USERS_FILE, "r");

        if (file == NULL) {
            printf("\n");
            printf("  ╔══════════════════════════════════════════════════════════╗\n");
            printf("  ║  ERROR: Cannot open users file.                          ║\n");
            printf("  ╚══════════════════════════════════════════════════════════╝\n");

            clearScreen2s();
            return 0;
        }

        User current;
        int found = 0;

        while (fscanf(file, "%9s %29s %49s %19s %14s",
                      current.userId,
                      current.userName,
                      current.password,
                      current.role,
                      current.status) == 5) {

            if (strcmp(username, current.userName) == 0 &&
                strcmp(password, current.password) == 0 &&
                strcmp(current.status, "ACTIVE") == 0) {

                *loggedUser = current;
                found = 1;
                break;
            }
        }

        fclose(file);

        if (found) {
            printf("\n");
            clearScreen();
            printf("  ╔══════════════════════════════════════════════════════════╗\n");
            printf("  ║                                                          ║\n");
            printf("  ║                   LOGIN SUCCESSFUL                       ║\n");
            printf("  ║                                                          ║\n");
            printf("  ╠══════════════════════════════════════════════════════════╣\n");
            printf("  ║  Welcome: %-46.46s ║\n", loggedUser->userName);
            printf("  ║  User ID: %-46.46s ║\n", loggedUser->userId);
            printf("  ║  Role:    %-46.46s ║\n", loggedUser->role);
            printf("  ║                                                          ║\n");
            printf("  ╚══════════════════════════════════════════════════════════╝\n");

            clearScreen2s();
            return 1;
        }
        clearScreen();
        printf("\n");
        printf("  ╔══════════════════════════════════════════════════════════╗\n");
        printf("  ║                                                          ║\n");
        printf("  ║                INVALID LOGIN CREDENTIALS                 ║\n");
        printf("  ║                                                          ║\n");
        printf("  ║  Attempts remaining: %-35d ║\n",
               MAX_ATTEMPTS - attempt);
        printf("  ╚══════════════════════════════════════════════════════════╝\n");

        if (attempt < MAX_ATTEMPTS) {
            printf("\n   Press Enter to try again...");

            /* Consume the newline left by scanf(). */
            getchar();

            /* Wait for the user to press Enter. */
            getchar();
        }
    }
    clearScreen();
    printf("\n");
    printf("  ╔══════════════════════════════════════════════════════════╗\n");
    printf("  ║                                                          ║\n");
    printf("  ║             MAXIMUM LOGIN ATTEMPTS REACHED               ║\n");
    printf("  ║                                                          ║\n");
    printf("  ╚══════════════════════════════════════════════════════════╝\n");

    clearScreen2s();
    return 0;
}
