
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "membership.h"
#include "member.h"
#include "ui.h"
#include "system.h"

#define MEMBER_FILE "data/members.txt"
#define MEMBERSHIP_FILE "data/memberships.txt"
#define PAYMENT_FILE "data/payments.txt"
#define TEMP_FILE "data/temp.txt"
#define BACKUP_FILE "data/memberships.bak"

/* Clear remaining input characters */
static void clearInputBuffer(void)
{
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        /* Discard remaining characters */
    }
}

/* Read a menu choice safely */
static int readChoice(void)
{
    int choice;
    int ch;

    if (scanf("%d", &choice) != 1)
    {
        clearInputBuffer();
        return -1;
    }

    ch = getchar();

    while (ch != '\n' && ch != EOF)
    {
        ch = getchar();
    }

    return choice;
}

/* Check whether a member exists.
   Assumes the member ID is the first field in members.txt. */
int checkMember(const char *memberId)
{
    FILE *fp = fopen(MEMBER_FILE, "r");
    char id[20];

    if (fp == NULL)
    {
        return 0;
    }

    while (fscanf(fp, "%19s", id) == 1)
    {
        if (strcmp(id, memberId) == 0)
        {
            fclose(fp);
            return 1;
        }

        clearInputBuffer();
    }

    fclose(fp);
    return 0;
}

/* Check whether a membership ID already exists */
int checkMembershipID(const char *membershipId)
{
    FILE *fp = fopen(MEMBERSHIP_FILE, "r");
    Membership m;

    if (fp == NULL)
    {
        return 0;
    }

    while (fscanf(fp, "%19s %19s %49s %14s %14s %f %19s",
                  m.membershipId,
                  m.memberId,
                  m.planName,
                  m.startDate,
                  m.endDate,
                  &m.fee,
                  m.status) == 7)
    {
        if (strcmp(m.membershipId, membershipId) == 0)
        {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

/* Check whether a payment ID already exists */
int checkPaymentID(const char *paymentId)
{
    FILE *fp = fopen(PAYMENT_FILE, "r");
    Payment p;

    if (fp == NULL)
    {
        return 0;
    }

    while (fscanf(fp, "%19s %19s %f %14s %19s",
                  p.paymentId,
                  p.memberId,
                  &p.amount,
                  p.paymentDate,
                  p.method) == 5)
    {
        if (strcmp(p.paymentId, paymentId) == 0)
        {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

/* Read a membership record */
int readMembership(FILE *fp, Membership *m)
{
    if (fp == NULL || m == NULL)
    {
        return 0;
    }

    return fscanf(fp, "%19s %19s %49s %14s %14s %f %19s",
                  m->membershipId,
                  m->memberId,
                  m->planName,
                  m->startDate,
                  m->endDate,
                  &m->fee,
                  m->status) == 7;
}

/* Write a membership record */
void writeMembership(FILE *fp, const Membership *m)
{
    if (fp == NULL || m == NULL)
    {
        return;
    }

    fprintf(fp, "%s %s %s %s %s %.2f %s\n",
            m->membershipId,
            m->memberId,
            m->planName,
            m->startDate,
            m->endDate,
            m->fee,
            m->status);
}

/* Save a payment record */
int savePayment(const Payment *p)
{
    FILE *fp;

    if (p == NULL)
    {
        return 0;
    }

    fp = fopen(PAYMENT_FILE, "a");

    if (fp == NULL)
    {
        printf("Error opening payment file!\n");
        return 0;
    }

    fprintf(fp, "%s %s %.2f %s %s\n",
            p->paymentId,
            p->memberId,
            p->amount,
            p->paymentDate,
            p->method);

    fclose(fp);
    return 1;
}

/* Collect payment details */
int takePayment(const char *memberId, float amount)
{
    Payment p;

    printf("\n========== PAYMENT DETAILS ==========\n");

    printf("Enter Payment ID: ");

    if (scanf("%19s", p.paymentId) != 1)
    {
        clearInputBuffer();
        return 0;
    }

    if (checkPaymentID(p.paymentId))
    {
        printf("Payment ID already exists!\n");
        return 0;
    }

    strcpy(p.memberId, memberId);
    p.amount = amount;

    printf("Enter Payment Date (YYYY-MM-DD): ");

    if (scanf("%14s", p.paymentDate) != 1)
    {
        clearInputBuffer();
        return 0;
    }

    printf("Payment Method (Cash/Card/Online): ");

    if (scanf("%19s", p.method) != 1)
    {
        clearInputBuffer();
        return 0;
    }

    if (strcmp(p.method, "Cash") != 0 &&
        strcmp(p.method, "Card") != 0 &&
        strcmp(p.method, "Online") != 0)
    {
        printf("Invalid payment method!\n");
        return 0;
    }

    if (!savePayment(&p))
    {
        return 0;
    }

    printf("Payment recorded successfully!\n");
    return 1;
}

/* Assign a membership */
void assignMembership(void)
{
    Membership m;
    FILE *fp;

    printf("\n========== ASSIGN MEMBERSHIP ==========\n");

    printf("Enter Membership ID: ");

    if (scanf("%19s", m.membershipId) != 1)
    {
        clearInputBuffer();
        return;
    }

    if (checkMembershipID(m.membershipId))
    {
        printf("Membership ID already exists!\n");
        return;
    }

    printf("Enter Member ID: ");

    if (scanf("%19s", m.memberId) != 1)
    {
        clearInputBuffer();
        return;
    }

    if (!checkMember(m.memberId))
    {
        printf("Member does not exist!\n");
        return;
    }

    printf("Enter Plan Name: ");

    if (scanf("%49s", m.planName) != 1)
    {
        clearInputBuffer();
        return;
    }

    printf("Enter Start Date (YYYY-MM-DD): ");

    if (scanf("%14s", m.startDate) != 1)
    {
        clearInputBuffer();
        return;
    }

    printf("Enter End Date (YYYY-MM-DD): ");

    if (scanf("%14s", m.endDate) != 1)
    {
        clearInputBuffer();
        return;
    }

    /* Date validation intentionally removed. */

    printf("Enter Membership Fee: ");

    if (scanf("%f", &m.fee) != 1 || m.fee < 0)
    {
        clearInputBuffer();
        printf("Invalid membership fee!\n");
        return;
    }

    strcpy(m.status, "ACTIVE");

    if (!takePayment(m.memberId, m.fee))
    {
        printf("Membership assignment cancelled because payment failed.\n");
        return;
    }

    fp = fopen(MEMBERSHIP_FILE, "a");

    if (fp == NULL)
    {
        printf("Error opening membership file!\n");
        return;
    }

    writeMembership(fp, &m);
    fclose(fp);

    printf("Membership assigned successfully!\n");
}

/* View all memberships */
void viewMemberships(void)
{
    FILE *fp = fopen(MEMBERSHIP_FILE, "r");
    Membership m;
    clearScreen();
    // printf("\n========== ALL MEMBERSHIPS ==========\n");

    if (fp == NULL)
    {
        printf("No membership records found.\n");
        return;
    }

    // printf("%-15s %-15s %-15s %-12s %-12s %-12s %-12s\n",
    //        "Membership ID", "Member ID", "Plan",
    //        "Start Date", "End Date", "Fee", "Status");

    // printf("--------------------------------------------------------------------------------\n");

    printf("╔═════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                              ALL MEMBERSHIPS                                                    ║\n");
    printf("║                                                                                                                 ║\n");
    printf("╠═══════════════════╦══════════════════╦═════════════════╦═════════════╦═══════════════╦═════════════╦════════════╣\n");
    printf("║ MEMBERSHIP ID     ║    MEMBER ID     ║     PLAN        ║ START DATE  ║   END DATE    ║     FEE     ║  STATUS    ║\n");
    printf("╠═══════════════════╬══════════════════╬═════════════════╬═════════════╬═══════════════╬═════════════╬════════════╣\n");

    while (readMembership(fp, &m))
    {
        printf("║    %-15s║  %-15s ║  %-15s║%-12s ║  %-12s ║ %-12.2f║%-12s║\n",
               m.membershipId,
               m.memberId,
               m.planName,
               m.startDate,
               m.endDate,
               m.fee,
               m.status);
    }

    fclose(fp);
    printf("╚═══════════════════╩══════════════════╩═════════════════╩═════════════╩═══════════════╩═════════════╩════════════╝\n");
}

/* Renew a membership */
void renewMembership(void)
{
    char membershipId[20];
    Membership m;
    int found = 0;
    FILE *fp;
    FILE *temp;

    printf("\n========== RENEW MEMBERSHIP ==========\n");
    printf("Enter Membership ID: ");

    if (scanf("%19s", membershipId) != 1)
    {
        clearInputBuffer();
        return;
    }

    fp = fopen(MEMBERSHIP_FILE, "r");

    if (fp == NULL)
    {
        printf("Unable to open membership file!\n");
        return;
    }

    temp = fopen(TEMP_FILE, "w");

    if (temp == NULL)
    {
        fclose(fp);
        printf("Unable to create temporary file!\n");
        return;
    }

    while (readMembership(fp, &m))
    {
        if (strcmp(m.membershipId, membershipId) == 0)
        {
            found = 1;

            printf("Current End Date: %s\n", m.endDate);

            printf("Enter New Start Date (YYYY-MM-DD): ");

            if (scanf("%14s", m.startDate) != 1)
            {
                clearInputBuffer();
                fclose(fp);
                fclose(temp);
                remove(TEMP_FILE);
                return;
            }

            printf("Enter New End Date (YYYY-MM-DD): ");

            if (scanf("%14s", m.endDate) != 1)
            {
                clearInputBuffer();
                fclose(fp);
                fclose(temp);
                remove(TEMP_FILE);
                return;
            }

            /* Date validation intentionally removed. */

            printf("Enter Renewal Fee: ");

            if (scanf("%f", &m.fee) != 1 || m.fee < 0)
            {
                clearInputBuffer();
                fclose(fp);
                fclose(temp);
                remove(TEMP_FILE);
                printf("Invalid renewal fee!\n");
                return;
            }

            if (!takePayment(m.memberId, m.fee))
            {
                fclose(fp);
                fclose(temp);
                remove(TEMP_FILE);
                printf("Renewal cancelled because payment failed.\n");
                return;
            }

            strcpy(m.status, "ACTIVE");
            printf("Membership renewed successfully!\n");
        }

        writeMembership(temp, &m);
    }

    fclose(fp);
    fclose(temp);

    if (!found)
    {
        remove(TEMP_FILE);
        printf("Membership not found!\n");
        return;
    }

    remove(BACKUP_FILE);

    if (rename(MEMBERSHIP_FILE, BACKUP_FILE) != 0)
    {
        printf("Unable to back up membership file!\n");
        remove(TEMP_FILE);
        return;
    }

    if (rename(TEMP_FILE, MEMBERSHIP_FILE) != 0)
    {
        printf("Unable to update membership file!\n");

        if (rename(BACKUP_FILE, MEMBERSHIP_FILE) != 0)
        {
            printf("Warning: Could not restore the original membership file!\n");
        }

        remove(TEMP_FILE);
        return;
    }

    remove(BACKUP_FILE);
}

/* View memberships marked as expired */
void viewExpiredMemberships(void)
{
    FILE *fp = fopen(MEMBERSHIP_FILE, "r");
    Membership m;
    int found = 0;

    printf("\n========== EXPIRED MEMBERSHIPS ==========\n");

    if (fp == NULL)
    {
        printf("No membership records found.\n");
        return;
    }

    while (readMembership(fp, &m))
    {
        if (strcmp(m.status, "EXPIRED") == 0)
        {
            printf("Membership ID : %s\n", m.membershipId);
            printf("Member ID     : %s\n", m.memberId);
            printf("Plan          : %s\n", m.planName);
            printf("End Date      : %s\n", m.endDate);
            printf("----------------------------------------\n");
            found = 1;
        }
    }

    if (!found)
    {
        printf("No expired memberships found.\n");
    }

    fclose(fp);
}

/* Cancel a membership */
void cancelMembership(void)
{
    char membershipId[20];
    Membership m;
    int found = 0;
    FILE *fp;
    FILE *temp;

    printf("\n========== CANCEL MEMBERSHIP ==========\n");
    printf("Enter Membership ID: ");

    if (scanf("%19s", membershipId) != 1)
    {
        clearInputBuffer();
        return;
    }

    fp = fopen(MEMBERSHIP_FILE, "r");

    if (fp == NULL)
    {
        printf("Unable to open membership file!\n");
        return;
    }

    temp = fopen(TEMP_FILE, "w");

    if (temp == NULL)
    {
        fclose(fp);
        printf("Unable to create temporary file!\n");
        return;
    }

    while (readMembership(fp, &m))
    {
        if (strcmp(m.membershipId, membershipId) == 0)
        {
            found = 1;

            if (strcmp(m.status, "CANCELLED") == 0)
            {
                printf("Membership is already cancelled.\n");
            }
            else
            {
                strcpy(m.status, "CANCELLED");
                printf("Membership cancelled successfully!\n");
            }
        }

        writeMembership(temp, &m);
    }

    fclose(fp);
    fclose(temp);

    if (!found)
    {
        remove(TEMP_FILE);
        printf("Membership not found!\n");
        return;
    }

    remove(BACKUP_FILE);

    if (rename(MEMBERSHIP_FILE, BACKUP_FILE) != 0)
    {
        printf("Unable to back up membership file!\n");
        remove(TEMP_FILE);
        return;
    }

    if (rename(TEMP_FILE, MEMBERSHIP_FILE) != 0)
    {
        printf("Unable to update membership file!\n");

        if (rename(BACKUP_FILE, MEMBERSHIP_FILE) != 0)
        {
            printf("Warning: Could not restore the original membership file!\n");
        }

        remove(TEMP_FILE);
        return;
    }

    remove(BACKUP_FILE);
}

/* Record a standalone payment */
void recordPayment(void)
{
    char memberId[20];
    float amount;

    printf("\n========== RECORD PAYMENT ==========\n");

    printf("Enter Member ID: ");

    if (scanf("%19s", memberId) != 1)
    {
        clearInputBuffer();
        return;
    }

    if (!checkMember(memberId))
    {
        printf("Member does not exist!\n");
        return;
    }

    printf("Enter Amount: ");

    if (scanf("%f", &amount) != 1 || amount <= 0)
    {
        clearInputBuffer();
        printf("Invalid payment amount!\n");
        return;
    }

    if (takePayment(memberId, amount))
    {
        printf("Payment recorded successfully!\n");
    }
}

/* View payment history */
void viewPayments(void)
{
    FILE *fp = fopen(PAYMENT_FILE, "r");
    Payment p;

    printf("\n========== PAYMENT HISTORY ==========\n");

    if (fp == NULL)
    {
        printf("No payment records found.\n");
        return;
    }

    printf("%-15s %-15s %-12s %-15s %-15s\n",
           "Payment ID", "Member ID", "Amount",
           "Payment Date", "Method");

    printf("------------------------------------------------------------------------\n");

    while (fscanf(fp, "%19s %19s %f %14s %19s",
                  p.paymentId,
                  p.memberId,
                  &p.amount,
                  p.paymentDate,
                  p.method) == 5)
    {
        printf("%-15s %-15s %-12.2f %-15s %-15s\n",
               p.paymentId,
               p.memberId,
               p.amount,
               p.paymentDate,
               p.method);
    }

    fclose(fp);
}

/* Membership management menu */
void membershipMenu(void)
{
    int choice;

    do
    {
         clearScreen();
        printLogoWithoutl();

        printf("║   [1]  Assign Membership                                                       ║\n");
        printf("║   [2]  View Memberships                                                        ║\n");
        printf("║   [3]  Renew Membership                                                        ║\n");
        printf("║   [4]  View Expired Memberships                                                ║\n");
        printf("║   [5]  Cancel Membership                                                       ║\n");
        printf("║   [6]  Record Payment                                                          ║\n");
        printf("║   [7]  View Payments                                                           ║\n");
        printf("║   [8]  Return to Dashboard                                                     ║\n");
        printf("╚════════════════════════════════════════════════════════════════════════════════╝\n");
        printf("Enter choice: ");

        choice = readChoice();

        switch (choice)
        {
            case 1:
                assignMembership();
                break;

            case 2:
                viewMemberships();
                break;

            case 3:
                renewMembership();
                break;

            case 4:
                viewExpiredMemberships();
                break;

            case 5:
                cancelMembership();
                break;

            case 6:
                recordPayment();
                break;

            case 7:
                viewPayments();
                break;

            case 0:
                printf("Returning to main menu...\n");
                break;

            default:
                printf("Invalid choice! Please try again.\n");
                break;
        }

        if (choice != 0)
        {
            printf("\nPress Enter to continue...");
            clearInputBuffer();
            getchar();
        }

    } while (choice != 0);
}
