#include <stdio.h>
#include <string.h>
#include "auth.h"
#include <system.h>

#define USERS_FILE "data/users.txt"
#define MAX_ATTEMPTS 3

int login(User *loggedUser)
{
    char username[30];
    char password[50];

    if (loggedUser == NULL) {
        return 0;
    }
    clearScreen();

    for (int attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {

        printf("\n========================================\n");
        printf("          GYM MANAGEMENT SYSTEM\n");
        printf("                  LOGIN\n");
        printf("========================================\n");

        printf("Username: ");

        if (scanf("%29s", username) != 1) {
            return 0;
        }

        printf("Password: ");

        if (scanf("%49s", password) != 1) {
            return 0;
        }

        FILE *file = fopen(USERS_FILE, "r");

        if (file == NULL) {
            printf("\nError: Cannot open %s\n", USERS_FILE);
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
            printf("\nLogin successful!\n");
            printf("Welcome, %s!\n", loggedUser->userName);
            printf("User ID: %s\n", loggedUser->userId);
            printf("Role: %s\n", loggedUser->role);

            return 1;
        }

        printf("\nInvalid credentials or inactive account.\n");

        if (attempt < MAX_ATTEMPTS) {
            printf("Attempts remaining: %d\n",
                   MAX_ATTEMPTS - attempt);
        }
    }

    printf("\nMaximum login attempts reached.\n");

    return 0;
}
