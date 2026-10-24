
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "membership.h"
#include "member.h"
#include "payment.h"
#include "ui.h"
#include "system.h"

#define MEMBER_FILE     "data/members.txt"
#define MEMBERSHIP_FILE "data/memberships.txt"
#define TEMP_FILE       "data/temp.txt"
#define BACKUP_FILE     "data/memberships.bak"

/* ---------------------------------------------------------
   INPUT HELPERS
   --------------------------------------------------------- */

static void clearInputBuffer(void)
{
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF) {
        /* Discard remaining characters */
    }
}

static int readChoice(void)
{
    int choice;
    int ch;

    if (scanf("%d", &choice) != 1) {
        clearInputBuffer();
        return -1;
    }

    while ((ch = getchar()) != '\n' && ch != EOF) {
        /* Discard remaining characters */
    }

    return choice;
}

static void pauseScreen(void)
{
    printf("\nPress Enter to continue...");
    clearInputBuffer();
    getchar();
}

/* ---------------------------------------------------------
   CHECK MEMBER
   Assumes member ID is the first field in members.txt.
   Read the rest of each line from the FILE, not stdin.
   --------------------------------------------------------- */

int checkMember(const char *memberId)
{
    FILE *fp;
    char line[512];
    char id[20];

    if (memberId == NULL || memberId[0] == '\0') {
        return 0;
    }

    fp = fopen(MEMBER_FILE, "r");

    if (fp == NULL) {
        return 0;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (sscanf(line, "%19s", id) == 1) {
            if (strcmp(id, memberId) == 0) {
                fclose(fp);
                return 1;
            }
        }
    }

    fclose(fp);
    return 0;
}

/* ---------------------------------------------------------
   CHECK MEMBERSHIP ID
   --------------------------------------------------------- */

int checkMembershipID(const char *membershipId)
{
    FILE *fp;
    Membership m;

    if (membershipId == NULL || membershipId[0] == '\0') {
        return 0;
    }

    fp = fopen(MEMBERSHIP_FILE, "r");

    if (fp == NULL) {
        return 0;
    }

    while (readMembership(fp, &m)) {
        if (strcmp(m.membershipId, membershipId) == 0) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

/* ---------------------------------------------------------
   READ MEMBERSHIP RECORD
   Format:
   membershipId memberId planName startDate endDate fee status
   --------------------------------------------------------- */

int readMembership(FILE *fp, Membership *m)
{
    if (fp == NULL || m == NULL) {
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

/* ---------------------------------------------------------
   WRITE MEMBERSHIP RECORD
   --------------------------------------------------------- */

void writeMembership(FILE *fp, const Membership *m)
{
    if (fp == NULL || m == NULL) {
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

/* ---------------------------------------------------------
   SAVE TEMPORARY MEMBERSHIP FILE
   Uses a backup so the original can be restored on failure.
   --------------------------------------------------------- */

static int replaceMembershipFile(void)
{
    /*
       Do not delete an existing backup before checking it.
       An existing backup may be the only recovery copy.
    */

    FILE *check = fopen(BACKUP_FILE, "r");

    if (check != NULL) {
        fclose(check);
        printf("Backup file already exists: %s\n", BACKUP_FILE);
        printf("Please inspect or restore it before continuing.\n");
        remove(TEMP_FILE);
        return 0;
    }

    if (rename(MEMBERSHIP_FILE, BACKUP_FILE) != 0) {
        printf("Unable to back up membership file!\n");
        remove(TEMP_FILE);
        return 0;
    }

    if (rename(TEMP_FILE, MEMBERSHIP_FILE) != 0) {
        printf("Unable to update membership file!\n");

        if (rename(BACKUP_FILE, MEMBERSHIP_FILE) != 0) {
            printf("Warning: Could not restore the original membership file!\n");
            printf("Your backup remains at: %s\n", BACKUP_FILE);
        }

        remove(TEMP_FILE);
        return 0;
    }

    if (remove(BACKUP_FILE) != 0) {
        printf("Warning: Membership saved, but backup could not be removed.\n");
    }

    return 1;
}

/* ---------------------------------------------------------
   ASSIGN MEMBERSHIP
   --------------------------------------------------------- */

void assignMembership(void)
{
    Membership m;
    FILE *fp;

    clearScreen();
    printf("\n========== ASSIGN MEMBERSHIP ==========\n");

    printf("Enter Membership ID: ");

    if (scanf("%19s", m.membershipId) != 1) {
        clearInputBuffer();
        printf("Invalid membership ID.\n");
        return;
    }

    if (checkMembershipID(m.membershipId)) {
        printf("Membership ID already exists!\n");
        return;
    }

    printf("Enter Member ID: ");

    if (scanf("%19s", m.memberId) != 1) {
        clearInputBuffer();
        printf("Invalid member ID.\n");
        return;
    }

    if (!checkMember(m.memberId)) {
        printf("Member does not exist!\n");
        return;
    }

    printf("Enter Plan Name: ");

    if (scanf("%49s", m.planName) != 1) {
        clearInputBuffer();
        printf("Invalid plan name.\n");
        return;
    }

    printf("Enter Start Date (YYYY-MM-DD): ");

    if (scanf("%14s", m.startDate) != 1) {
        clearInputBuffer();
        printf("Invalid start date input.\n");
        return;
    }

    printf("Enter End Date (YYYY-MM-DD): ");

    if (scanf("%14s", m.endDate) != 1) {
        clearInputBuffer();
        printf("Invalid end date input.\n");
        return;
    }

    /* Membership date validation intentionally removed. */

    printf("Enter Membership Fee: ");

    if (scanf("%f", &m.fee) != 1 || m.fee <= 0.0f) {
        clearInputBuffer();
        printf("Invalid membership fee! Fee must be greater than zero.\n");
        return;
    }

    strcpy(m.status, "ACTIVE");

    /*
       Reuse the payment module.
       It handles payment ID generation, date, method,
       validation, and writing the payment record.
    */

    if (!processMembershipPayment(m.memberId, m.fee)) {
        printf("Membership assignment cancelled because payment failed.\n");
        return;
    }

    fp = fopen(MEMBERSHIP_FILE, "a");

    if (fp == NULL) {
        printf("Error opening membership file!\n");
        printf("Warning: Payment may already have been recorded.\n");
        return;
    }

    writeMembership(fp, &m);

    if (fclose(fp) != 0) {
        printf("Error closing membership file.\n");
        printf("Warning: Please verify the membership record.\n");
        return;
    }

    printf("\nMembership assigned successfully!\n");
}

/* ---------------------------------------------------------
   VIEW ALL MEMBERSHIPS
   --------------------------------------------------------- */

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
/* ---------------------------------------------------------
   RENEW MEMBERSHIP
   --------------------------------------------------------- */

void renewMembership(void)
{
    char membershipId[20];
    Membership m;
    FILE *fp;
    FILE *temp;
    int found = 0;
    int paymentSucceeded = 0;

    clearScreen();
    printf("\n========== RENEW MEMBERSHIP ==========\n");
    printf("Enter Membership ID: ");

    if (scanf("%19s", membershipId) != 1) {
        clearInputBuffer();
        printf("Invalid membership ID.\n");
        return;
    }

    fp = fopen(MEMBERSHIP_FILE, "r");

    if (fp == NULL) {
        printf("Unable to open membership file!\n");
        return;
    }

    temp = fopen(TEMP_FILE, "w");

    if (temp == NULL) {
        fclose(fp);
        printf("Unable to create temporary file!\n");
        return;
    }

    while (readMembership(fp, &m)) {
        if (strcmp(m.membershipId, membershipId) == 0) {
            found = 1;

            if (strcmp(m.status, "CANCELLED") == 0) {
                printf("Cannot renew a cancelled membership.\n");
                fclose(fp);
                fclose(temp);
                remove(TEMP_FILE);
                return;
            }

            printf("Member ID         : %s\n", m.memberId);
            printf("Current Plan      : %s\n", m.planName);
            printf("Current End Date  : %s\n", m.endDate);

            printf("Enter New Start Date (YYYY-MM-DD): ");

            if (scanf("%14s", m.startDate) != 1) {
                clearInputBuffer();
                fclose(fp);
                fclose(temp);
                remove(TEMP_FILE);
                printf("Invalid start date input.\n");
                return;
            }

            printf("Enter New End Date (YYYY-MM-DD): ");

            if (scanf("%14s", m.endDate) != 1) {
                clearInputBuffer();
                fclose(fp);
                fclose(temp);
                remove(TEMP_FILE);
                printf("Invalid end date input.\n");
                return;
            }

            /* Membership date validation intentionally removed. */

            printf("Enter Renewal Fee: ");

            if (scanf("%f", &m.fee) != 1 || m.fee <= 0.0f) {
                clearInputBuffer();
                fclose(fp);
                fclose(temp);
                remove(TEMP_FILE);
                printf("Invalid renewal fee! Fee must be greater than zero.\n");
                return;
            }

            /*
               Process the renewal payment using payment.c.
               If payment fails, do not replace the membership file.
            */

            if (!processMembershipPayment(m.memberId, m.fee)) {
                fclose(fp);
                fclose(temp);
                remove(TEMP_FILE);
                printf("Renewal cancelled because payment failed.\n");
                return;
            }

            paymentSucceeded = 1;
            strcpy(m.status, "ACTIVE");
        }

        writeMembership(temp, &m);
    }

    fclose(fp);

    if (fclose(temp) != 0) {
        remove(TEMP_FILE);

        if (paymentSucceeded) {
            printf("Warning: Payment succeeded, but the temporary membership file could not be closed.\n");
        }

        return;
    }

    if (!found) {
        remove(TEMP_FILE);
        printf("Membership not found!\n");
        return;
    }

    if (!replaceMembershipFile()) {
        if (paymentSucceeded) {
            printf("Warning: Payment was recorded, but membership renewal was not saved.\n");
        }

        return;
    }

    printf("Membership renewed successfully!\n");
}

/* ---------------------------------------------------------
   VIEW EXPIRED MEMBERSHIPS
   Displays records whose status is EXPIRED.
   --------------------------------------------------------- */

void viewExpiredMemberships(void)
{
    FILE *fp;
    Membership m;
    int found = 0;

    clearScreen();
    printf("\n========== EXPIRED MEMBERSHIPS ==========\n");

    fp = fopen(MEMBERSHIP_FILE, "r");

    if (fp == NULL) {
        printf("No membership records found.\n");
        return;
    }

    while (readMembership(fp, &m)) {
        if (strcmp(m.status, "EXPIRED") == 0) {
            printf("\nMembership ID : %s\n", m.membershipId);
            printf("Member ID     : %s\n", m.memberId);
            printf("Plan          : %s\n", m.planName);
            printf("End Date      : %s\n", m.endDate);
            printf("----------------------------------------\n");

            found = 1;
        }
    }

    fclose(fp);

    if (!found) {
        printf("No expired memberships found.\n");
    }
}

/* ---------------------------------------------------------
   CANCEL MEMBERSHIP
   --------------------------------------------------------- */

void cancelMembership(void)
{
    char membershipId[20];
    Membership m;
    FILE *fp;
    FILE *temp;
    int found = 0;
    int alreadyCancelled = 0;

    clearScreen();
    printf("\n========== CANCEL MEMBERSHIP ==========\n");
    printf("Enter Membership ID: ");

    if (scanf("%19s", membershipId) != 1) {
        clearInputBuffer();
        printf("Invalid membership ID.\n");
        return;
    }

    fp = fopen(MEMBERSHIP_FILE, "r");

    if (fp == NULL) {
        printf("Unable to open membership file!\n");
        return;
    }

    temp = fopen(TEMP_FILE, "w");

    if (temp == NULL) {
        fclose(fp);
        printf("Unable to create temporary file!\n");
        return;
    }

    while (readMembership(fp, &m)) {
        if (strcmp(m.membershipId, membershipId) == 0) {
            found = 1;

            if (strcmp(m.status, "CANCELLED") == 0) {
                alreadyCancelled = 1;
            } else {
                strcpy(m.status, "CANCELLED");
            }
        }

        writeMembership(temp, &m);
    }

    fclose(fp);

    if (fclose(temp) != 0) {
        remove(TEMP_FILE);
        printf("Unable to finish writing temporary membership file.\n");
        return;
    }

    if (!found) {
        remove(TEMP_FILE);
        printf("Membership not found!\n");
        return;
    }

    if (alreadyCancelled) {
        remove(TEMP_FILE);
        printf("Membership is already cancelled.\n");
        return;
    }

    if (!replaceMembershipFile()) {
        return;
    }

    printf("Membership cancelled successfully!\n");
}

/* ---------------------------------------------------------
   RECORD A STANDALONE PAYMENT
   Delegate to the payment module.
   --------------------------------------------------------- */

void recordPayment(void)
{
    paymentRecord();
}

/* ---------------------------------------------------------
   VIEW PAYMENT HISTORY
   Delegate to the payment module.
   --------------------------------------------------------- */

void viewPayments(void)
{
    viewPaymentHistory();
}

/* ---------------------------------------------------------
   MEMBERSHIP MANAGEMENT MENU
   --------------------------------------------------------- */

void membershipMenu(void)
{
    int choice;

    do {
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

        switch (choice) {
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

        if (choice != 0) {
            pauseScreen();
        }

    } while (choice != 0);
}
