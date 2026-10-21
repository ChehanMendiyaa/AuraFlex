#include <stdio.h>
#include <string.h>
#include "auth.h"
#include "member.h"
#include "membership.h"
#include "trainer.h"
#include "reports.h"
#include "system.h"
#include "ui.h"
#include "attendance.h"
#include "payment.h"

static void clearInputBuffer(void)
{
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF) {
        /* Discard remaining input */
    }
}

static void pauseScreen(void)
{
    printf("\nPress Enter to return to the dashboard...");

    /* Wait for the user to press Enter */
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        /* Discard input */
    }
}



int main(void)
{
    User loggedUser;
    int choice;
    int logoutChoice;
    printLogo();
    // printf("\n============================================\n");
    // printf("       GYM & FITNESS MANAGEMENT SYSTEM\n");
    // printf("============================================\n");

    /* Login */
    if (!login(&loggedUser)) {
        printf("\nAccess denied. Program terminated.\n");
        return 1;
    }

    /* Determine the logout menu number */
    if (strcmp(loggedUser.role, "ADMIN") == 0) {
        logoutChoice = 7;
    } else if (strcmp(loggedUser.role, "RECEPTIONIST") == 0 ||
               strcmp(loggedUser.role, "TRAINER") == 0) {
        logoutChoice = 6;
    } else {
        printf("\nError: Invalid role configuration.\n");
        return 1;
    }

    /* Keep displaying the dashboard until logout */
    clearScreen();
    while (1) {
        printf("\n============================================\n");
        printf("                 DASHBOARD\n");
        printf("============================================\n");
        printf("Welcome, %s!\n", loggedUser.userName);
        printf("Role: %s\n", loggedUser.role);
        printf("--------------------------------------------\n");

        if (strcmp(loggedUser.role, "ADMIN") == 0) {
            clearScreen();
            printLogo();
            printf("║   [1] Member Management          ║                                             ║\n");
            printf("║   [2] Membership Plans           ║                                             ║\n");
            printf("║   [3] Trainer Management         ║                                             ║\n");
            printf("║   [4] Payment Management         ║                                             ║\n");
            printf("║   [5] Attendance Management      ║                                             ║\n");
            printf("║   [6] Reports                    ║                                             ║\n");
            printf("║   [7] Logout                     ║                                             ║\n");
            printf("╠══════════════════════════════════║═════════════════════════════════════════════╣\n");
            printf("║\n");
            printf("║════ Enter your choice: ");
        } else if (strcmp(loggedUser.role, "RECEPTIONIST") == 0) {
            printf("1. Register Member\n");
            printf("2. Manage Memberships\n");
            printf("3. Record Payment\n");
            printf("4. Record Attendance\n");
            printf("5. View Members\n");
            printf("6. Logout\n");
        } else {
            printf("1. View Assigned Members\n");
            printf("2. Manage Workout Plans\n");
            printf("3. Manage Nutrition Plans\n");
            printf("4. View Fitness Progress\n");
            printf("5. Manage Goals\n");
            printf("6. Logout\n");
        }

        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("\nInvalid input. Please enter a number.\n");
            clearInputBuffer();
            continue;
        }

        clearInputBuffer();
        clearScreen();
        /* Logout */
        if (choice == logoutChoice) {
            printf("\nLogged out successfully. Goodbye, %s!\n",
                   loggedUser.userName);
            break;
        }

        /* Check the selected menu option */
        if (choice < 1 || choice >= logoutChoice) {
            printf("\nInvalid menu choice. Please try again.\n");
            continue;
        }

        printf("\n--------------------------------------------\n");

        if (strcmp(loggedUser.role, "ADMIN") == 0) {
            switch (choice) {
                case 1:
                clearScreen();
                memberMenu();
                break;
                case 2:
                clearScreen();
                membershipMenu(); 
                 break;
                case 3:
                 printf("Trainer Management\n"); 
                 break;
                case 4:
                 paymentMenu(); 
                 break;
                case 5:
                 attendanceMenu(); 
                 break;
                case 6:
                 reportsMenu(); 
                 break;
            }
        } else if (strcmp(loggedUser.role, "RECEPTIONIST") == 0) {
            switch (choice) {
                case 1:
                    clearScreen();
                    memberMenu();
                    break;
                case 2:
                 clearScreen();
                 membershipMenu(); 
                 break;
                case 3:
                 printf("Record Payment\n"); 
                 break;
                case 4:
                    clearScreen();
                 attendanceMenu(); 
                 break;
                case 5:
                 printf("View Members\n"); 
                 break;
            }
        } else {

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
