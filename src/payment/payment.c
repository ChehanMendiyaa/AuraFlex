#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "payment.h"

#define PAYMENTS_FILE "data/payments.txt"
#define MEMBERS_FILE "data/members.txt"

typedef struct {
    char paymentId[20];
    char memberId[10];
    float amount;
    char paymentDate[11];
    char method[20];
} Payment;

/* Check that a member exists and is active.
   Returns 1 if active, 0 if not found/inactive, -1 on file error. */
static int paymentMemberExists(const char memberId[])
{
    FILE *file = fopen(MEMBERS_FILE, "r");
    char id[10], name[50], phone[20];
    char email[60], status[15];

    if (file == NULL) {
        printf("Error: Cannot open %s.\n", MEMBERS_FILE);
        return -1;
    }

    while (fscanf(file, "%9s %49s %19s %59s %14s",
                  id, name, phone, email, status) == 5) {
        if (strcmp(id, memberId) == 0) {
            fclose(file);
            return strcmp(status, "ACTIVE") == 0;
        }
    }

    fclose(file);
    return 0;
}

/* Read one record from the existing payment file format. */
static int readPayment(FILE *file, Payment *payment)
{
    return fscanf(file, "%19s %9s %f %10s %19s",
                  payment->paymentId,
                  payment->memberId,
                  &payment->amount,
                  payment->paymentDate,
                  payment->method) == 5;
}

/* Check whether a payment ID already exists.
   Returns -1 on file error, 0 if available, 1 if duplicate. */
static int paymentIdExists(const char paymentId[])
{
    FILE *file = fopen(PAYMENTS_FILE, "r");
    Payment payment;

    if (file == NULL)
        return 0;

    while (1) {
        int result = readPayment(file, &payment);

        if (result != 1)
            break;

        if (strcmp(payment.paymentId, paymentId) == 0) {
            fclose(file);
            return 1;
        }
    }

    if (ferror(file)) {
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

/* Generate the next available payment ID, e.g. P001. */
static int generatePaymentId(char paymentId[])
{
    FILE *file = fopen(PAYMENTS_FILE, "r");
    Payment payment;
    int highest = 0;
    int number;

    if (file != NULL) {
        while (readPayment(file, &payment)) {
            if (sscanf(payment.paymentId, "P%d", &number) == 1 &&
                number > highest) {
                highest = number;
            }
        }

        if (ferror(file)) {
            fclose(file);
            return 0;
        }

        fclose(file);
    }

    if (highest >= 999999)
        return 0;

    do {
        highest++;
        snprintf(paymentId, 20, "P%03d", highest);

        int exists = paymentIdExists(paymentId);

        if (exists == -1)
            return 0;

        if (exists == 0)
            return 1;

    } while (highest < 999999);

    return 0;
}

/* Validate a date's format and basic ranges. */
static int validDate(const char date[])
{
    int year, month, day;
    int i;

    if (strlen(date) != 10 ||
        date[4] != '-' || date[7] != '-') {
        return 0;
    }

    for (i = 0; i < 10; i++) {
        if (i == 4 || i == 7)
            continue;

        if (!isdigit((unsigned char)date[i]))
            return 0;
    }

    if (sscanf(date, "%4d-%2d-%2d",
               &year, &month, &day) != 3) {
        return 0;
    }

    if (year < 1 || month < 1 || month > 12 ||
        day < 1 || day > 31) {
        return 0;
    }

    return 1;
}

/* Record a payment without conflicting with membership.c. */
void paymentRecord(void)
{
    FILE *file;
    Payment payment;
    int memberResult;
    int ch;
    char amountText[40];
    char *end;
    float amount;

    printf("\n--- Record Payment ---\n");

    printf("Enter Member ID: ");

    if (scanf("%9s", payment.memberId) != 1) {
        printf("Invalid member ID input.\n");
        return;
    }

    memberResult = paymentMemberExists(payment.memberId);

    if (memberResult == -1)
        return;

    if (memberResult == 0) {
        printf("Member not found or member is not ACTIVE.\n");
        return;
    }

    printf("Enter payment amount: ");

    if (scanf("%39s", amountText) != 1) {
        printf("Invalid amount.\n");
        return;
    }

    amount = strtof(amountText, &end);

    if (end == amountText || *end != '\0' ||
        amount <= 0.0f) {
        printf("Enter a valid positive amount.\n");
        return;
    }

    payment.amount = amount;

    printf("Enter payment date (YYYY-MM-DD): ");

    if (scanf("%10s", payment.paymentDate) != 1 ||
        !validDate(payment.paymentDate)) {
        printf("Invalid date. Use YYYY-MM-DD.\n");
        return;
    }

    printf("Payment method (CASH/CARD/ONLINE): ");

    if (scanf("%19s", payment.method) != 1) {
        printf("Invalid payment method.\n");
        return;
    }

    for (int i = 0; payment.method[i] != '\0'; i++)
        payment.method[i] =
            (char)toupper((unsigned char)payment.method[i]);

    if (strcmp(payment.method, "CASH") != 0 &&
        strcmp(payment.method, "CARD") != 0 &&
        strcmp(payment.method, "ONLINE") != 0) {
        printf("Choose CASH, CARD, or ONLINE.\n");
        return;
    }

    if (!generatePaymentId(payment.paymentId)) {
        printf("Could not generate a payment ID. Check the payment file.\n");
        return;
    }

    file = fopen(PAYMENTS_FILE, "a");

    if (file == NULL) {
        printf("Error: Cannot open %s for writing.\n",
               PAYMENTS_FILE);
        return;
    }

    if (fprintf(file, "%s %s %.2f %s %s\n",
                payment.paymentId,
                payment.memberId,
                payment.amount,
                payment.paymentDate,
                payment.method) < 0) {
        printf("Error writing payment record.\n");
        fclose(file);
        return;
    }

    if (fclose(file) != 0) {
        printf("Error closing the payment file.\n");
        return;
    }

    printf("\nPayment recorded successfully!\n");
    printf("Payment ID: %s\n", payment.paymentId);
    printf("Member ID: %s\n", payment.memberId);
    printf("Amount: %.2f\n", payment.amount);
    printf("Date: %s\n", payment.paymentDate);
    printf("Method: %s\n", payment.method);

    /* Clear any leftover input on this line. */
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

/* Search payment history by member ID. */
void searchPaymentHistory(void)
{
    FILE *file;
    Payment payment;
    char memberId[10];
    int found = 0;
    float total = 0.0f;

    printf("\n--- Search Payment History ---\n");
    printf("Enter Member ID: ");

    if (scanf("%9s", memberId) != 1) {
        printf("Invalid member ID.\n");
        return;
    }

    file = fopen(PAYMENTS_FILE, "r");

    if (file == NULL) {
        printf("No payment records found.\n");
        return;
    }

    printf("\n%-10s %-10s %-12s %-12s %-10s\n",
           "Payment ID", "Member ID", "Amount",
           "Date", "Method");
    printf("---------------------------------------------------------\n");

    while (readPayment(file, &payment)) {
        if (strcmp(payment.memberId, memberId) == 0) {
            printf("%-10s %-10s %-12.2f %-12s %-10s\n",
                   payment.paymentId,
                   payment.memberId,
                   payment.amount,
                   payment.paymentDate,
                   payment.method);

            total += payment.amount;
            found = 1;
        }
    }

    fclose(file);

    if (found)
        printf("\nTotal payments for %s: %.2f\n",
               memberId, total);
    else
        printf("No payments found for member %s.\n", memberId);
}

/* View all payment records. */
void viewPaymentHistory(void)
{
    FILE *file = fopen(PAYMENTS_FILE, "r");
    Payment payment;
    int found = 0;

    printf("\n--- All Payment Records ---\n");
    printf("%-10s %-10s %-12s %-12s %-10s\n",
           "Payment ID", "Member ID", "Amount",
           "Date", "Method");
    printf("---------------------------------------------------------\n");

    if (file == NULL) {
        printf("No payment records found.\n");
        return;
    }

    while (readPayment(file, &payment)) {
        printf("%-10s %-10s %-12.2f %-12s %-10s\n",
               payment.paymentId,
               payment.memberId,
               payment.amount,
               payment.paymentDate,
               payment.method);
        found = 1;
    }

    fclose(file);

    if (!found)
        printf("No valid payment records found.\n");
}

/* Calculate the total recorded payments. */
void calculateTotalPayments(void)
{
    FILE *file = fopen(PAYMENTS_FILE, "r");
    Payment payment;
    float total = 0.0f;
    int count = 0;

    printf("\n--- Total Payments Collected ---\n");

    if (file == NULL) {
        printf("No payment records found.\n");
        printf("Total collected: 0.00\n");
        return;
    }

    while (readPayment(file, &payment)) {
        total += payment.amount;
        count++;
    }

    fclose(file);

    printf("Number of payment records: %d\n", count);
    printf("Total collected: %.2f\n", total);
}

/* Payment menu. */
void paymentMenu(void)
{
    int choice;
    int ch;

    do {
        printf("\n====================================\n");
        printf("        AURAFLEX PAYMENTS\n");
        printf("====================================\n");
        printf("1. Record Payment\n");
        printf("2. Search Payment History\n");
        printf("3. View All Payments\n");
        printf("4. Calculate Total Payments\n");
        printf("5. Return to Main Menu\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }

            choice = 0;
        }

        switch (choice) {
            case 1:
                paymentRecord();
                break;
            case 2:
                searchPaymentHistory();
                break;
            case 3:
                viewPaymentHistory();
                break;
            case 4:
                calculateTotalPayments();
                break;
            case 5:
                printf("Returning to main menu...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }

    } while (choice != 5);
}