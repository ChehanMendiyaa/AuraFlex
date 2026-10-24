
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "reports.h"

/* Data files */
#define MEMBER_FILE "data/members.txt"
#define MEMBERSHIP_FILE "data/memberships.txt"
#define PAYMENT_FILE "data/payments.txt"

/* Get today's date in YYYY-MM-DD format */
static void getToday(char today[])
{
    time_t now = time(NULL);
    struct tm *currentDate = localtime(&now);

    if (currentDate == NULL) {
        strcpy(today, "0000-00-00");
        return;
    }

    strftime(today, 11, "%Y-%m-%d", currentDate);
}

/* 1. Display all members */
void memberReport(void)
{
    FILE *file = fopen(MEMBER_FILE, "r");

    char id[10], name[50], phone[20];
    char email[60], status[15];
    int count = 0;

    if (file == NULL) {
        printf("\nCannot open members.txt\n");
        return;
    }

    printf("\n========== MEMBER REPORT ==========\n");
    printf("%-10s %-20s %-15s %-25s %-12s\n",
           "ID", "Name", "Phone", "Email", "Status");
    printf("--------------------------------------------------------------------------\n");

    while (fscanf(file, "%9s %49s %19s %59s %14s",
                  id, name, phone, email, status) == 5) {
        printf("%-10s %-20s %-15s %-25s %-12s\n",
               id, name, phone, email, status);
        count++;
    }

    printf("\nTotal members: %d\n", count);

    fclose(file);
}

/* 2. Display all memberships */
void membershipReport(void)
{
    FILE *file = fopen(MEMBERSHIP_FILE, "r");

    char membershipId[10], memberId[10];
    char plan[20], startDate[11], endDate[11];
    char status[15];
    float fee;
    int count = 0;

    if (file == NULL) {
        printf("\nCannot open memberships.txt\n");
        return;
    }

    printf("\n========== MEMBERSHIP REPORT ==========\n");
    printf("%-10s %-10s %-12s %-12s %-12s %-10s %-12s\n",
           "Mem.ID", "MemberID", "Plan", "Start", "End",
           "Fee", "Status");
    printf("-------------------------------------------------------------------------------\n");

    while (fscanf(file, "%9s %9s %19s %10s %10s %f %14s",
                  membershipId, memberId, plan, startDate,
                  endDate, &fee, status) == 7) {
        printf("%-10s %-10s %-12s %-12s %-12s %-10.2f %-12s\n",
               membershipId, memberId, plan, startDate,
               endDate, fee, status);
        count++;
    }

    printf("\nTotal membership records: %d\n", count);

    fclose(file);
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

    if (file == NULL) {
        printf("\nCannot open memberships.txt\n");
        return;
    }

    getToday(today);

    printf("\n========== EXPIRED MEMBERSHIP REPORT ==========\n");
    printf("Today's date: %s\n\n", today);
    printf("%-10s %-10s %-12s %-12s %-10s\n",
           "Mem.ID", "MemberID", "Plan", "End Date", "Fee");
    printf("---------------------------------------------------------------\n");

    while (fscanf(file, "%9s %9s %19s %10s %10s %f %14s",
                  membershipId, memberId, plan, startDate,
                  endDate, &fee, status) == 7) {

        /* Dates must use YYYY-MM-DD format */
        if (strcmp(status, "ACTIVE") == 0 &&
            strcmp(endDate, today) < 0) {

            printf("%-10s %-10s %-12s %-12s %-10.2f\n",
                   membershipId, memberId, plan, endDate, fee);
            count++;
        }
    }

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

    if (file == NULL) {
        printf("\nCannot open payments.txt\n");
        return;
    }

    printf("\n========== PAYMENT REPORT ==========\n");
    printf("%-12s %-12s %-12s %-15s %-12s\n",
           "Payment ID", "Member ID", "Amount",
           "Payment Date", "Method");
    printf("-----------------------------------------------------------------\n");

    while (fscanf(file, "%9s %9s %f %10s %19s",
                  paymentId, memberId, &amount,
                  paymentDate, method) == 5) {

        printf("%-12s %-12s %-12.2f %-15s %-12s\n",
               paymentId, memberId, amount,
               paymentDate, method);

        total += amount;
        count++;
    }

    printf("\nTotal payment records: %d\n", count);
    printf("Total recorded payments: %.2f\n", total);

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
        printf("\nCannot open members.txt\n");
        return;
    }

    while (fscanf(file, "%9s %49s %19s %59s %14s",
                  id, name, phone, email, status) == 5) {
        totalMembers++;
    }

    fclose(file);

    /* Count memberships and their statuses */
    file = fopen(MEMBERSHIP_FILE, "r");

    if (file == NULL) {
        printf("\nCannot open memberships.txt\n");
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

    /* Count payments and calculate their total */
    file = fopen(PAYMENT_FILE, "r");

    if (file == NULL) {
        printf("\nCannot open payments.txt\n");
        return;
    }

    while (fscanf(file, "%9s %9s %f %10s %19s",
                  paymentId, memberId, &amount,
                  paymentDate, method) == 5) {

        totalPayments++;
        totalPaymentAmount += amount;
    }

    fclose(file);

    /* Display summary */
    printf("\n========== AURAFLEX SUMMARY REPORT ==========\n");
    printf("Report date: %s\n\n", today);

    printf("MEMBERS\n");
    printf("Total members:                  %d\n", totalMembers);

    printf("\nMEMBERSHIPS\n");
    printf("Total membership records:       %d\n", totalMemberships);
    printf("Active and not expired:         %d\n", activeMemberships);
    printf("Expired active memberships:     %d\n", expiredMemberships);
    printf("Cancelled memberships:          %d\n", cancelledMemberships);

    printf("\nPAYMENTS\n");
    printf("Total payment records:          %d\n", totalPayments);
    printf("Total recorded payment amount:  %.2f\n",
           totalPaymentAmount);

    printf("\n=============================================\n");
}

/* Main reports menu */
void reportsMenu(void)
{
    int choice;
    int result;
    int ch;

    do {
        printf("\n========== AURAFLEX REPORTS ==========\n");
        printf("1. Member Report\n");
        printf("2. Membership Report\n");
        printf("3. Expired Membership Report\n");
        printf("4. Payment Report\n");
        printf("5. Summary Report\n");
        printf("6. Return to Main Menu\n");
        printf("Enter your choice: ");

        result = scanf("%d", &choice);

        if (result != 1) {
            printf("Invalid input. Please enter a number.\n");

            while ((ch = getchar()) != '\n' && ch != EOF) {
                /* Clear invalid input */
            }

            continue;
        }

        switch (choice) {
            case 1:
                memberReport();
                break;

            case 2:
                membershipReport();
                break;

            case 3:
                expiredMembershipReport();
                break;

            case 4:
                paymentReport();
                break;

            case 5:
                summaryReport();
                break;

            case 6:
                printf("Returning to the main menu...\n");
                break;

            default:
                printf("Invalid choice. Please select 1-6.\n");
        }

    } while (choice != 6);
}
