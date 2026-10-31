
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ui.h"
#include "system.h"

#include "payment.h"

#define PAYMENTS_FILE "data/payments.txt"
#define MEMBERS_FILE  "data/members.txt"

#define ID_LENGTH 20
#define MEMBER_ID_LENGTH 10
#define DATE_LENGTH 11
#define METHOD_LENGTH 20

typedef struct {
    char paymentId[ID_LENGTH];
    char memberId[MEMBER_ID_LENGTH];
    float amount;
    char paymentDate[DATE_LENGTH];
    char method[METHOD_LENGTH];
} Payment;

/* ---------------------------------------------------------
   INPUT HELPERS
   --------------------------------------------------------- */

static void clearInputBuffer(void)
{
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF) {
        /* Discard remaining input */
    }
}

static void readText(const char *prompt, char *buffer, size_t size)
{
    printf("%s", prompt);

    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }

    buffer[strcspn(buffer, "\n")] = '\0';

    /* If input exceeded the buffer, discard the remaining characters. */
    if (strchr(buffer, '\0') != NULL) {
        /* No action needed here; newline handling is done below. */
    }
}

/* ---------------------------------------------------------
   DATE VALIDATION
   Format: YYYY-MM-DD
   --------------------------------------------------------- */

static int validDate(const char *date)
{
    int year, month, day;
    int daysInMonth;

    if (date == NULL || strlen(date) != 10) {
        return 0;
    }

    if (date[4] != '-' || date[7] != '-') {
        return 0;
    }

    for (int i = 0; i < 10; i++) {
        if (i != 4 && i != 7 && !isdigit((unsigned char)date[i])) {
            return 0;
        }
    }

    if (sscanf(date, "%4d-%2d-%2d", &year, &month, &day) != 3) {
        return 0;
    }

    if (year < 1900 || month < 1 || month > 12 || day < 1) {
        return 0;
    }

    switch (month) {
        case 4:
        case 6:
        case 9:
        case 11:
            daysInMonth = 30;
            break;

        case 2:
            daysInMonth = 28;

            if ((year % 400 == 0) ||
                (year % 4 == 0 && year % 100 != 0)) {
                daysInMonth = 29;
            }
            break;

        default:
            daysInMonth = 31;
    }

    return day <= daysInMonth;
}

/* ---------------------------------------------------------
   MEMBER VALIDATION
   Expected members.txt format:
   memberId name phone email status
   --------------------------------------------------------- */

static int paymentMemberExists(const char *memberId)
{
    FILE *file = fopen(MEMBERS_FILE, "r");

    char id[MEMBER_ID_LENGTH];
    char name[50];
    char phone[20];
    char email[60];
    char status[15];

    if (file == NULL) {
        printf("Error: Could not open members file.\n");
        return 0;
    }

    while (fscanf(file, "%9s %49s %19s %59s %14s",
                  id, name, phone, email, status) == 5) {

        if (strcmp(id, memberId) == 0) {
            fclose(file);

            if (strcmp(status, "Active") == 0 ||
                strcmp(status, "ACTIVE") == 0 ||
                strcmp(status, "active") == 0) {
                return 1;
            }

            printf("Error: This member is not active.\n");
            return 0;
        }
    }

    fclose(file);
    printf("Error: Member ID '%s' was not found.\n", memberId);
    return 0;
}

/* ---------------------------------------------------------
   PAYMENT FILE HELPERS
   Expected payments.txt format:
   paymentId memberId amount paymentDate method
   --------------------------------------------------------- */

static int readPayment(FILE *file, Payment *payment)
{
    return fscanf(file, "%19s %9s %f %10s %19s",
                  payment->paymentId,
                  payment->memberId,
                  &payment->amount,
                  payment->paymentDate,
                  payment->method) == 5;
}

static int paymentIdExists(const char *paymentId)
{
    FILE *file = fopen(PAYMENTS_FILE, "r");
    Payment payment;

    if (file == NULL) {
        return 0;
    }

    while (readPayment(file, &payment)) {
        if (strcmp(payment.paymentId, paymentId) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

static int generatePaymentId(char *paymentId)
{
    FILE *file = fopen(PAYMENTS_FILE, "r");
    Payment payment;
    int highestNumber = 0;
    int number;

    if (file != NULL) {
        while (readPayment(file, &payment)) {
            if (payment.paymentId[0] == 'P' &&
                sscanf(payment.paymentId + 1, "%d", &number) == 1 &&
                number > highestNumber) {
                highestNumber = number;
            }
        }

        fclose(file);
    }

    do {
        highestNumber++;

        if (snprintf(paymentId, ID_LENGTH, "P%03d",
                     highestNumber) >= ID_LENGTH) {
            return 0;
        }
    } while (paymentIdExists(paymentId));

    return 1;
}

/* ---------------------------------------------------------
   SHARED PAYMENT PROCESS
   Both paymentRecord() and membership.c use this function.
   --------------------------------------------------------- */

static int savePayment(const char *memberId, float amount)
{
    Payment payment;
    FILE *file;
    char dateInput[DATE_LENGTH];
    char methodInput[METHOD_LENGTH];

    if (memberId == NULL || strlen(memberId) == 0) {
        printf("Error: Member ID is required.\n");
        return 0;
    }

    if (amount <= 0.0f) {
        printf("Error: Payment amount must be greater than zero.\n");
        return 0;
    }

    if (!paymentMemberExists(memberId)) {
        return 0;
    }

    if (strlen(memberId) >= sizeof(payment.memberId)) {
        printf("Error: Member ID is too long.\n");
        return 0;
    }

    strcpy(payment.memberId, memberId);
    payment.amount = amount;

    if (!generatePaymentId(payment.paymentId)) {
        printf("Error: Could not generate a payment ID.\n");
        return 0;
    }

    /* Get and validate payment date. */
    while (1) {
        readText("Enter payment date (YYYY-MM-DD): ",
                 dateInput, sizeof(dateInput));

        if (validDate(dateInput)) {
            strcpy(payment.paymentDate, dateInput);
            break;
        }

        printf("Invalid date. Please use YYYY-MM-DD.\n");
    }

    /* Get and validate payment method. */
    while (1) {
        readText("Enter payment method (CASH/CARD/ONLINE): ",
                 methodInput, sizeof(methodInput));

        for (size_t i = 0; methodInput[i] != '\0'; i++) {
            methodInput[i] =
                (char)toupper((unsigned char)methodInput[i]);
        }

        if (strcmp(methodInput, "CASH") == 0 ||
            strcmp(methodInput, "CARD") == 0 ||
            strcmp(methodInput, "ONLINE") == 0) {
            strcpy(payment.method, methodInput);
            break;
        }

        printf("Invalid method. Choose CASH, CARD, or ONLINE.\n");
    }

    file = fopen(PAYMENTS_FILE, "a");

    if (file == NULL) {
        printf("Error: Could not open payments file for writing.\n");
        return 0;
    }

    if (fprintf(file, "%s %s %.2f %s %s\n",
                payment.paymentId,
                payment.memberId,
                payment.amount,
                payment.paymentDate,
                payment.method) < 0) {
        fclose(file);
        printf("Error: Could not save payment.\n");
        return 0;
    }

    if (fclose(file) != 0) {
        printf("Warning: Error closing the payments file.\n");
        return 0;
    }

    printf("\nPayment recorded successfully!\n");
    printf("----------------------------------\n");
    printf("Payment ID : %s\n", payment.paymentId);
    printf("Member ID  : %s\n", payment.memberId);
    printf("Amount     : %.2f\n", payment.amount);
    printf("Date       : %s\n", payment.paymentDate);
    printf("Method     : %s\n", payment.method);
    printf("----------------------------------\n");

    return 1;
}

/* ---------------------------------------------------------
   PUBLIC FUNCTION: NORMAL PAYMENT
   --------------------------------------------------------- */

void paymentRecord(void)
{
    char memberId[MEMBER_ID_LENGTH];
    float amount;

    printf("\n========== RECORD PAYMENT ==========\n");

    readText("Enter member ID: ", memberId, sizeof(memberId));

    if (memberId[0] == '\0') {
        printf("Error: Member ID cannot be empty.\n");
        return;
    }

    printf("Enter payment amount: ");

    if (scanf("%f", &amount) != 1) {
        printf("Invalid amount. Please enter a number.\n");
        clearInputBuffer();
        return;
    }

    clearInputBuffer();

    if (amount <= 0.0f) {
        printf("Error: Amount must be greater than zero.\n");
        return;
    }

    savePayment(memberId, amount);
}

/* ---------------------------------------------------------
   PUBLIC FUNCTION: MEMBERSHIP PAYMENT
   Call this function from membership.c.
   --------------------------------------------------------- */

int processMembershipPayment(const char *memberId, float amount)
{
    printf("\n========== MEMBERSHIP PAYMENT ==========\n");
    printf("Member ID       : %s\n", memberId ? memberId : "");
    printf("Membership fee  : %.2f\n", amount);

    if (memberId == NULL || amount <= 0.0f) {
        printf("Error: Invalid membership payment details.\n");
        return 0;
    }

    return savePayment(memberId, amount);
}

/* ---------------------------------------------------------
   SEARCH PAYMENT HISTORY
   --------------------------------------------------------- */

void searchPaymentHistory(void)
{
    FILE *file;
    Payment payment;
    char searchId[MEMBER_ID_LENGTH];
    int found = 0;

    printf("\n========== SEARCH PAYMENT HISTORY ==========\n");
    readText("Enter member ID: ", searchId, sizeof(searchId));

    if (searchId[0] == '\0') {
        printf("Member ID cannot be empty.\n");
        return;
    }

    file = fopen(PAYMENTS_FILE, "r");

    if (file == NULL) {
        printf("No payment history found.\n");
        return;
    }

    printf("\n%-12s %-12s %-12s %-14s %-10s\n",
           "Payment ID", "Member ID", "Amount", "Date", "Method");
    printf("---------------------------------------------------------------\n");

    while (readPayment(file, &payment)) {
        if (strcmp(payment.memberId, searchId) == 0) {
            printf("%-12s %-12s %-12.2f %-14s %-10s\n",
                   payment.paymentId,
                   payment.memberId,
                   payment.amount,
                   payment.paymentDate,
                   payment.method);
            found = 1;
        }
    }

    fclose(file);

    if (!found) {
        printf("No payments found for member %s.\n", searchId);
    }
}

/* ---------------------------------------------------------
   VIEW ALL PAYMENTS
   --------------------------------------------------------- */

void viewPaymentHistory(void)
{
    FILE *file;
    Payment payment;
    int found = 0;

    //printf("\n================ ALL PAYMENT HISTORY ================\n");

    file = fopen(PAYMENTS_FILE, "r");

    if (file == NULL) {
        printf("No payment history found.\n");
        return;
    }

    // printf("%-12s %-12s %-12s %-14s %-10s\n",
    //        "Payment ID", "Member ID", "Amount", "Date", "Method");
    // printf("---------------------------------------------------------------\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                 ALL PAYMENT HISTORY                                   ║\n");
    printf("║                                                                                       ║\n");
    printf("╠═══════════════════╦═════════════════╦═════════════════╦══════════════════╦════════════╣\n");
    printf("║    PAYMENT ID     ║    MEMBER ID    ║      AMOUNT     ║       DATE       ║   METHOD   ║\n");
    printf("╠═══════════════════╬═════════════════╬═════════════════╬══════════════════╬════════════╬\n");

    while (readPayment(file, &payment)) {
        printf("║     %-12s  ║   %-12s  ║   %-12.2f  ║  %-14s  ║ %-10s ║\n",
               payment.paymentId,
               payment.memberId,
               payment.amount,
               payment.paymentDate,
               payment.method);

        found = 1;
    }
    printf("╚═══════════════════╩═════════════════╩═════════════════╩══════════════════╩════════════╝\n");
    fclose(file);

    if (!found) {
        printf("No payment records available.\n");
    }
}

/* ---------------------------------------------------------
   CALCULATE TOTAL PAYMENTS
   --------------------------------------------------------- */

void calculateTotalPayments(void)
{
    FILE *file;
    Payment payment;
    float total = 0.0f;
    int count = 0;

    file = fopen(PAYMENTS_FILE, "r");

    if (file == NULL) {
        printf("\nNo payment records available.\n");
        return;
    }

    while (readPayment(file, &payment)) {
        total += payment.amount;
        count++;
    }

    fclose(file);

    printf("\n========== PAYMENT SUMMARY ==========\n");
    printf("Number of payments : %d\n", count);
    printf("Total payments     : %.2f\n", total);
    printf("=====================================\n");
}

/* ---------------------------------------------------------
   PAYMENT MENU
   --------------------------------------------------------- */

void paymentMenu(void)
{
    int choice;

    do {
        clearScreen();
        printLogoWithoutl();
        printf("║   [1]  Record Payment                                                          ║\n");
        printf("║   [2]  Search Payment History                                                  ║\n");
        printf("║   [3]  View All Payments                                                       ║\n");
        printf("║   [4]  Calculate Total Payments                                                ║\n");
        printf("║   [5]  Return to Main Menu                                                     ║\n");
        printf("║════════════════════════════════════════════════════════════════════════════════╝\n");
        printf("║\n");
        printf("╚════Enter choice: ");

        if (scanf("%d", &choice) != 1)
        {
            clearInputBuffer();

            printf(RED "\nInvalid input! Please enter a number from 1 to 5.\n" RESET);
            printf(YELLOW "\nPress Enter to continue..." RESET);
            getchar();
            continue;
        }

        clearInputBuffer();

        switch (choice)
        {
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
                printf(GREEN "\nReturning to dashboard...\n" RESET);
                return;

            default:
                printf(RED "\nInvalid choice! Please select 1-5.\n" RESET);
                break;
        }

        if (choice != 5)
        {
            printf(YELLOW "\nPress Enter to continue..." RESET);
            getchar();
        }

    } while (choice != 5);
}
