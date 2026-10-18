#include <stdio.h>
#include <string.h>
#include "membership.h"

#define MEMBERSHIPS_FILE "data/memberships.txt"
#define PAYMENTS_FILE "data/payments.txt"

void assignMembership(void)
{
    Membership m;
    FILE *file;

    printf("\n===== ASSIGN MEMBERSHIP =====\n");

    printf("Membership ID (e.g. MS001): ");
    scanf("%9s", m.membershipId);

    printf("Member ID (e.g. M001): ");
    scanf("%9s", m.memberId);

    printf("Plan (Monthly/Quarterly/Yearly): ");
    scanf("%19s", m.planName);

    printf("Start date (YYYY-MM-DD): ");
    scanf("%10s", m.startDate);

    printf("End date (YYYY-MM-DD): ");
    scanf("%10s", m.endDate);

    printf("Membership fee: ");
    if (scanf("%f", &m.fee) != 1 || m.fee < 0) {
        printf("Invalid fee.\n");
        return;
    }

    strcpy(m.status, "ACTIVE");

    file = fopen(MEMBERSHIPS_FILE, "a");
    if (file == NULL) {
        printf("Could not open memberships.txt\n");
        return;
    }

    fprintf(file, "%s %s %s %s %s %.2f %s\n",
            m.membershipId, m.memberId, m.planName,
            m.startDate, m.endDate, m.fee, m.status);

    fclose(file);
    printf("Membership assigned successfully!\n");
}

void viewMemberships(void)
{
    Membership m;
    FILE *file = fopen(MEMBERSHIPS_FILE, "r");

    if (file == NULL) {
        printf("No memberships found yet.\n");
        return;
    }

    printf("\n===== MEMBERSHIPS =====\n");

    while (fscanf(file, "%9s %9s %19s %10s %10s %f %14s",
                  m.membershipId, m.memberId, m.planName,
                  m.startDate, m.endDate, &m.fee, m.status) == 7) {
        printf("\nMembership ID: %s\nMember ID: %s\nPlan: %s\n",
               m.membershipId, m.memberId, m.planName);
        printf("Start: %s\nEnd: %s\nFee: %.2f\nStatus: %s\n",
               m.startDate, m.endDate, m.fee, m.status);
    }

    fclose(file);
}

void recordPayment(void)
{
    Payment p;
    FILE *file;

    printf("\n===== RECORD PAYMENT =====\n");

    printf("Payment ID (e.g. P001): ");
    scanf("%9s", p.paymentId);

    printf("Member ID: ");
    scanf("%9s", p.memberId);

    printf("Amount: ");
    if (scanf("%f", &p.amount) != 1 || p.amount <= 0) {
        printf("Amount must be greater than zero.\n");
        return;
    }

    printf("Payment date (YYYY-MM-DD): ");
    scanf("%10s", p.paymentDate);

    printf("Method (Cash/Card/Transfer): ");
    scanf("%19s", p.method);

    file = fopen(PAYMENTS_FILE, "a");
    if (file == NULL) {
        printf("Could not open payments.txt\n");
        return;
    }

    fprintf(file, "%s %s %.2f %s %s\n",
            p.paymentId, p.memberId, p.amount,
            p.paymentDate, p.method);

    fclose(file);
    printf("Payment recorded successfully!\n");
}

void viewPayments(void)
{
    Payment p;
    FILE *file = fopen(PAYMENTS_FILE, "r");

    if (file == NULL) {
        printf("No payments found yet.\n");
        return;
    }

    printf("\n===== PAYMENT HISTORY =====\n");

    while (fscanf(file, "%9s %9s %f %10s %19s",
                  p.paymentId, p.memberId, &p.amount,
                  p.paymentDate, p.method) == 5) {
        printf("\nPayment ID: %s\nMember ID: %s\nAmount: %.2f\n",
               p.paymentId, p.memberId, p.amount);
        printf("Date: %s\nMethod: %s\n", p.paymentDate, p.method);
    }

    fclose(file);
}

void membershipMenu(void)
{
    int choice;

    do {
        printf("\n===== MEMBERSHIP & PAYMENTS =====\n");
        printf("1. Assign Membership\n");
        printf("2. View Memberships\n");
        printf("3. Record Payment\n");
        printf("4. View Payments\n");
        printf("5. Return to Dashboard\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            printf("Please enter a number.\n");
            choice = 0;
            continue;
        }

        switch (choice) {
            case 1: assignMembership(); break;
            case 2: viewMemberships(); break;
            case 3: recordPayment(); break;
            case 4: viewPayments(); break;
            case 5: printf("Returning to dashboard...\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 5);

}
