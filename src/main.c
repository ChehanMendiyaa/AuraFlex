#include <stdio.h>
#include <string.h>
#include "auth.h"

void clearInputBuffer(void)
{
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF) {
        /* Clear remaining input */
    }
}

void pauseScreen(void)
{
    printf("\nPress Enter to return to the dashboard...");

    int ch;

    /* Clear any remaining characters from previous input */
    while ((ch = getchar()) != '\n' && ch != EOF) {
        /* Discard characters */
    }

    getchar();
}

int main(void)
{
    User loggedUser;
    int choice;
    int logoutChoice;

    printf("\n");
    printf("============================================\n");
    printf("       GYM & FITNESS MANAGEMENT SYSTEM\n");
    printf("============================================\n");

    /* Login */
    if (!login(&loggedUser)) {
        printf("\nAccess denied. Program terminated.\n");
        return 1;
    }

    /* Set the Logout option for the current role */
    if (strcmp(loggedUser.role, "ADMIN") == 0) {
        logoutChoice = 7;
    }
    else if (strcmp(loggedUser.role, "RECEPTIONIST") == 0 ||
             strcmp(loggedUser.role, "TRAINER") == 0) {
        logoutChoice = 6;
    }
    else {
        printf("\nError: Invalid role configuration.\n");
        return 1;
    }

    /* Dashboard loop */
    while (1) {
        printf("\n");
        printf("============================================\n");
        printf("                 DASHBOARD\n");
        printf("============================================\n");
        printf("Welcome, %s!\n", loggedUser.userName);
        printf("Role: %s\n", loggedUser.role);
        printf("--------------------------------------------\n");

        /* Admin dashboard */
        if (strcmp(loggedUser.role, "ADMIN") == 0) {
            printf("1. Member Management\n");
            printf("2. Membership Plans\n");
            printf("3. Trainer Management\n");
            printf("4. Payment Management\n");
            printf("5. Attendance Management\n");
            printf("6. Reports\n");
            printf("7. Logout\n");
        }

        /* Receptionist dashboard */
        else if (strcmp(loggedUser.role, "RECEPTIONIST") == 0) {
            printf("1. Register Member\n");
            printf("2. Manage Memberships\n");
            printf("3. Record Payment\n");
            printf("4. Record Attendance\n");
            printf("5. View Members\n");
            printf("6. Logout\n");
        }

        /* Trainer dashboard */
        else if (strcmp(loggedUser.role, "TRAINER") == 0) {
            printf("1. View Assigned Members\n");
            printf("2. Manage Workout Plans\n");
            printf("3. Manage Nutrition Plans\n");
            printf("4. View Fitness Progress\n");
            printf("5. Manage Goals\n");
            printf("6. Logout\n");
        }

        printf("--------------------------------------------\n");
        printf("Enter your choice: ");

        /* Validate menu input */
        if (scanf("%d", &choice) != 1) {
            printf("\nInvalid input. Please enter a number.\n");
            clearInputBuffer();
            continue;
        }

        clearInputBuffer();

        /* Logout */
        if (choice == logoutChoice) {
            printf("\n============================================\n");
            printf("Logged out successfully. Goodbye, %s!\n",
                   loggedUser.userName);
            printf("============================================\n");
            break;
        }

        /* Validate menu selection */
        if (choice < 1 || choice >= logoutChoice) {
            printf("\nInvalid menu choice. Please try again.\n");
            continue;
        }

        /* Feature placeholders */
        printf("\n--------------------------------------------\n");

        if (strcmp(loggedUser.role, "ADMIN") == 0) {
            switch (choice) {
                case 1:
                    printf("Member Management\n");
                    break;
                case 2:
                    printf("Membership Plans\n");
                    break;
                case 3:
                    printf("Trainer Management\n");
                    break;
                case 4:
                    printf("Payment Management\n");
                    break;
                case 5:
                    printf("Attendance Management\n");
                    break;
                case 6:
                    printf("Reports\n");
                    break;
            }
        }
        else if (strcmp(loggedUser.role, "RECEPTIONIST") == 0) {
            switch (choice) {
                case 1:
                    printf("Register Member\n");
                    break;
                case 2:
                    printf("Manage Memberships\n");
                    break;
                case 3:
                    printf("Record Payment\n");
                    break;
                case 4:
                    printf("Record Attendance\n");
                    break;
                case 5:
                    printf("View Members\n");
                    break;
            }
        }
        else if (strcmp(loggedUser.role, "TRAINER") == 0) {
            switch (choice) {
                case 1:
                    printf("View Assigned Members\n");
                    break;
                case 2:
                    printf("Manage Workout Plans\n");
                    break;
                case 3:
                    printf("Manage Nutrition Plans\n");
                    break;
                case 4:
                    printf("View Fitness Progress\n");
                    break;
                case 5:
                    printf("Manage Goals\n");
                    break;
            }
        }

        printf("\nThis feature has not been implemented yet.\n");
        printf("--------------------------------------------\n");

        pauseScreen();
    }

    return 0;
}
