#include <stdio.h>
#include <string.h>
#include <time.h>

#include "ui.h"
#include "system.h"
#include "reports.h"
#include "member.h"
#include "membership.h"

/* Data files */
#define MEMBER_FILE "data/members.txt"
#define MEMBERSHIP_FILE "data/memberships.txt"
#define PAYMENT_FILE "data/payments.txt"

/* Get today's date in YYYY-MM-DD format */
static void getToday(char today[11])
{
    time_t now = time(NULL);
    struct tm *currentDate = localtime(&now);

    if (currentDate == NULL) {
        strcpy(today, "0000-00-00");
        return;
    }

    strftime(today, 11, "%Y-%m-%d", currentDate);
}

/* Wait for the user to press Enter */
static void pressEnterToContinue(void)
{
    int ch;

    printf("\nPress Enter to return to the reports menu...");
    fflush(stdout);

    while ((ch = getchar()) != '\n' && ch != EOF) {
        /* Clear remaining input */
    }
}

/* 3. Display active memberships whose end dates have passed */
void expiredMembershipReport(void)
{
    FILE *file = fopen(MEMBERSHIP_FILE, "r");

    char membershipId[10], memberId[10];
    char plan[20], startDate[11], endDate[11];
    char status[15], today[11];
    float fee;
    int count = 0;

    clearScreen();

    if (file == NULL) {
        printf("Cannot open memberships.txt\n");
        return;
    }
    getToday(today);

    printf("╔════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                 EXPIRED MEMBERSHIP REPORT                                      ║\n");
    printf("║                                                              Today's date: %-10s          ║\n", today);
    printf("╠════════════╦══════════════════╦══════════════╦══════════════════════════════════════╦══════════╣\n");
    printf("║ MEMBER ID  ║ NAME             ║ PHONE        ║ EMAIL                                ║ STATUS   ║\n");
    printf("╠════════════╬══════════════════╬══════════════╬══════════════════════════════════════╬══════════╣\n");

    /*
       Keep the original table design, but display fields
       that actually belong to the membership record.
    */
    while (fscanf(file, "%9s %9s %19s %10s %10s %f %14s",
                  membershipId, memberId, plan, startDate,
                  endDate, &fee, status) == 7) {

        if (strcmp(status, "ACTIVE") == 0 &&
            strcmp(endDate, today) < 0) {

            printf("║ %-10.10s ║ %-16.16s ║ %-12.12s ║ %-36.36s ║ %-8.8s ║\n",
                   membershipId, memberId, plan, endDate, status);

            count++;
        }
    }

    printf("╚════════════╩══════════════════╩══════════════╩══════════════════════════════════════╩══════════╝\n");

    if (count == 0) {
        printf("No expired active memberships found.\n");
    }

    printf("\nTotal expired active memberships: %d\n", count);

    fclose(file);
}

/* 4. Display all payment records */
void paymentReport(void)
{
    FILE *file = fopen(PAYMENT_FILE, "r");

    char paymentId[10], memberId[10];
    char paymentDate[11], method[20];
    float amount, total = 0.0f;
    int count = 0;

    clearScreen();

    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                              ║\n");
    printf("║                              PAYMENT REPORT                                  ║\n");
    printf("╠══════════════╦══════════════╦══════════════╦═════════════════╦═══════════════╣\n");
    printf("║ PAYMENT ID   ║ MEMBER ID    ║ AMOUNT       ║ PAYMENT DATE    ║ METHOD        ║\n");
    printf("╠══════════════╬══════════════╬══════════════╬═════════════════╬═══════════════╣\n");

    if (file == NULL) {
        printf("║                         Cannot open payments.txt                           ║\n");
        printf("╚══════════════════════════════════════════════════════════════════════════════╝\n");
        return;
    }

    while (fscanf(file, "%9s %9s %f %10s %19s",
                  paymentId, memberId, &amount,
                  paymentDate, method) == 5) {

        printf("║ %-12.12s ║ %-12.12s ║ %12.2f ║ %-15.15s ║ %-12.12s  ║\n",
               paymentId, memberId, amount, paymentDate, method);

        total += amount;
        count++;
    }

    printf("╠══════════════╩══════════════╩══════════════╩═════════════════╩═══════════════╣\n");
    printf("║ Total payment records: %-53d ║\n", count);
    printf("║ Total recorded payments: %-51.2f ║\n", total);
    printf("╚══════════════════════════════════════════════════════════════════════════════╝\n");

    if (count == 0) {
        printf("\nNo payment records found.\n");
    }

    fclose(file);
}


/* 5. Display a summary of the system */
void summaryReport(void)
{
    FILE *file;
    char today[11];

    int totalMembers = 0;
    int totalMemberships = 0;
    int activeMemberships = 0;
    int expiredMemberships = 0;
    int cancelledMemberships = 0;
    int totalPayments = 0;

    float totalPaymentAmount = 0.0f;

    char id[10], name[50], phone[20];
    char email[60], status[15];

    char membershipId[10], memberId[10];
    char plan[20], startDate[11], endDate[11];
    float fee;

    char paymentId[10], paymentDate[11], method[20];
    float amount;

    getToday(today);

    /* Count members */
    file = fopen(MEMBER_FILE, "r");

    if (file == NULL) {
        clearScreen();
        printf("Cannot open members.txt\n");
        return;
    }

    while (fscanf(file, "%9s %49s %19s %59s %14s",
                  id, name, phone, email, status) == 5) {
        totalMembers++;
    }

    fclose(file);

    /* Count memberships */
    file = fopen(MEMBERSHIP_FILE, "r");

    if (file == NULL) {
        clearScreen();
        printf("Cannot open memberships.txt\n");
        return;
    }

    while (fscanf(file, "%9s %9s %19s %10s %10s %f %14s",
                  membershipId, memberId, plan, startDate,
                  endDate, &fee, status) == 7) {

        totalMemberships++;

        if (strcmp(status, "CANCELLED") == 0) {
            cancelledMemberships++;
        } else if (strcmp(status, "ACTIVE") == 0) {
            if (strcmp(endDate, today) < 0) {
                expiredMemberships++;
            } else {
                activeMemberships++;
            }
        }
    }

    fclose(file);

    /* Count payments */
    file = fopen(PAYMENT_FILE, "r");

    if (file == NULL) {
        clearScreen();
        printf("Cannot open payments.txt\n");
        return;
    }

    while (fscanf(file, "%9s %9s %f %10s %19s",
                  paymentId, memberId, &amount,
                  paymentDate, method) == 5) {

        totalPayments++;
        totalPaymentAmount += amount;
    }

    fclose(file);

    /* Display summary report UI */
    clearScreen();

    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                           AURAFLEX GYM SYSTEM                                ║\n");
    printf("║                              SUMMARY REPORT                                  ║\n");
    printf("║                         Report Date: %-10s                              ║\n", today);
    printf("╠══════════════════════════════════════════════════════════════════════════════╣\n");

    printf("║                              MEMBER SUMMARY                                  ║\n");
    printf("╠══════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║ %-57s %15d    ║\n", "Total Members", totalMembers);

    printf("╠══════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║                            MEMBERSHIP SUMMARY                                ║\n");
    printf("╠══════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║ %-57s %15d    ║\n", "Total Membership Records", totalMemberships);
    printf("║ %-57s %15d    ║\n", "Active and Not Expired", activeMemberships);
    printf("║ %-57s %15d    ║\n", "Expired Active Memberships", expiredMemberships);
    printf("║ %-57s %15d    ║\n", "Cancelled Memberships", cancelledMemberships);

    printf("╠══════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║                             PAYMENT SUMMARY                                  ║\n");
    printf("╠══════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║ %-57s %15d    ║\n", "Total Payment Records", totalPayments);
    printf("║ %-57s %15.2f    ║\n", "Total Recorded Payments", totalPaymentAmount);

    printf("╚══════════════════════════════════════════════════════════════════════════════╝\n");
}


/* Main reports menu */
void reportsMenu(void)
{
    int choice;
    int result;
    int ch;

    do {
        clearScreen();
        printLogoWithoutl();

        printf("║   [1]  Member Report                                                               ║\n");
        printf("║   [2]  Membership Report                                                           ║\n");
        printf("║   [3]  Expired Membership Report                                                   ║\n");
        printf("║   [4]  Payment Report                                                              ║\n");
        printf("║   [5]  Summary Report                                                              ║\n");
        printf("║   [6]  Return to Main Menu                                                         ║\n");
        printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n");
        printf("Enter choice: ");

        result = scanf("%d", &choice);

        if (result != 1) {
            printf("Invalid input. Please enter a number.\n");

            while ((ch = getchar()) != '\n' && ch != EOF) {
                /* Clear invalid input */
            }

            choice = 0;
            pressEnterToContinue();
            continue;
        }

        /* Consume the menu input line */
        while ((ch = getchar()) != '\n' && ch != EOF) {
            /* Clear remaining input */
        }

        switch (choice) {
            case 1:
                viewMembers();
                pressEnterToContinue();
                break;

            case 2:
                viewMemberships();
                pressEnterToContinue();
                break;

            case 3:
                expiredMembershipReport();
                pressEnterToContinue();
                break;

            case 4:
                paymentReport();
                pressEnterToContinue();
                break;

            case 5:
                summaryReport();
                pressEnterToContinue();
                break;

            case 6:
                printf("Returning to the main menu...\n");
                break;

            default:
                printf("Invalid choice. Please select 1-6.\n");
                pressEnterToContinue();
                break;
        }

    } while (choice != 6);
}
