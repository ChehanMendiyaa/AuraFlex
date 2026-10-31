#include <stdio.h>
#include <string.h>

#include "auth.h"
#include "system.h"
#include "ui.h"
#include "platform.h"

#define USERS_FILE "data/users.txt"
#define MAX_ATTEMPTS 3

/*
 * Read password securely from the Windows console.
 * Displays '*' instead of the actual password characters.
 * Supports Backspace and Enter.
 */
static void inputPassword(char password[], int maxLength)
{
    int i = 0;
    int ch;

    printf(" ");
    fflush(stdout);

    while (1)
    {
        ch = platform_getch();

        // Stop when Enter is pressed
        if (ch == '\r' || ch == '\n' || ch == EOF)
        {
            break;
        }

        // Handle Backspace
        if (ch == 8 || ch == 127)
        {
            if (i > 0)
            {
                i--;
                printf("\b \b");
                fflush(stdout);
            }
        }

        // Accept printable characters
        else if (ch >= 32 && ch <= 126 && i < maxLength - 1)
        {
            password[i] = (char)ch;
            i++;

            // Display asterisk instead of password
            printf("*");
            fflush(stdout);
        }
    }

    // End the password string
    password[i] = '\0';

    printf("\n");
}

/*
 * Authenticate a user.
 * Returns 1 for success and 0 for failure.
 */
int login(User *loggedUser)
{
    char username[USERNAME_LENGTH];
    char password[PASSWORD_LENGTH];

    if (loggedUser == NULL)
    {
        return 0;
    }

    for (int attempt = 1; attempt <= MAX_ATTEMPTS; attempt++)
    {
        clearScreen();

        printf("\n");
        printf("  ╔══════════════════════════════════════════════════════════╗\n");
        printf("  ║                                                          ║\n");
        printf("  ║                       USER LOGIN                         ║\n");
        printf("  ║                                                          ║\n");
        printf("  ╠══════════════════════════════════════════════════════════╣\n");
        printf("  ║                                                          ║\n");
        printf("  ║  Enter your credentials to access AuraFlex.              ║\n");
        printf("  ║                                                          ║\n");
        printf("  ║  Username:                                               ║\n");
        printf("  ║  >  ");

        if (scanf("%29s", username) != 1)
        {
            return 0;
        }

        printf("  ║                                                          ║\n");
        printf("  ║  Password:                                               ║\n");
        printf("  ║  > ");

        // Mask password input with asterisks
        inputPassword(password, sizeof(password));

        printf("  ║                                                          ║\n");
        printf("  ╚══════════════════════════════════════════════════════════╝\n");

        FILE *file = fopen(USERS_FILE, "r");

        if (file == NULL)
        {
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
                      current.status) == 5)
        {
            if (strcmp(username, current.userName) == 0 &&
                strcmp(password, current.password) == 0 &&
                strcmp(current.status, "ACTIVE") == 0)
            {
                *loggedUser = current;
                found = 1;
                break;
            }
        }

        fclose(file);

        if (found)
        {
            clearScreen();

            printf("\n");
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
        printf("  ║               INVALID LOGIN CREDENTIALS                  ║\n");
        printf("  ║                                                          ║\n");
        printf("  ║  Attempts remaining: %-35d ║\n",
               MAX_ATTEMPTS - attempt);
        printf("  ╚══════════════════════════════════════════════════════════╝\n");

        if (attempt < MAX_ATTEMPTS)
        {
            printf("\n  Press Enter to try again...");

            // Clear leftover username input, including its newline
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF)
            {
                // Discard remaining input
            }

            // Wait for Enter before retrying
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
