#include <stdio.h>
#include <string.h>
#include <time.h>
#include "membership.h"

#define MEMBER_FILE "data/members.txt"
#define MEMBERSHIP_FILE "data/memberships.txt"
#define PAYMENT_FILE "data/payments.txt"
#define TEMP_FILE "data/temp.txt"

/* Check member ID and name */
static int checkMember(char id[], char name[])
{
    char savedID[10], savedName[50];
    char phone[20], email[60], status[15];

    FILE *file = fopen(MEMBER_FILE, "r");
    if (file == NULL) return 0;

    while (fscanf(file, "%9s %49s %19s %59s %14s",
                  savedID, savedName, phone, email, status) == 5) {
        if (strcmp(id, savedID) == 0 &&
            strcmp(name, savedName) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

/* Check whether membership ID is already used */
static int checkMembershipID(char id[])
{
    Membership m;
    FILE *file = fopen(MEMBERSHIP_FILE, "r");
    if (file == NULL) return 0;

    while (fscanf(file, "%9s %9s %19s %10s %10s %f %14s",
                  m.membershipId, m.memberId, m.planName,
                  m.startDate, m.endDate, &m.fee, m.status) == 7) {
        if (strcmp(id, m.membershipId) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

/* Check whether payment ID is already used */
static int checkPaymentID(char id[])
{
    Payment p;
    FILE *file = fopen(PAYMENT_FILE, "r");
    if (file == NULL) return 0;

    while (fscanf(file, "%9s %9s %f %10s %19s",
                  p.paymentId, p.memberId, &p.amount,
                  p.paymentDate, p.method) == 5) {
        if (strcmp(id, p.paymentId) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

/* Read one membership from file */
static int readMembership(Membership *m, FILE *file)
{
    return fscanf(file, "%9s %9s %19s %10s %10s %f %14s",
                  m->membershipId, m->memberId, m->planName,
                  m->startDate, m->endDate, &m->fee, m->status) == 7;
}

/* Write one membership to file */
static int writeMembership(Membership *m, FILE *file)
{
    return fprintf(file, "%s %s %s %s %s %.2f %s\n",
                   m->membershipId, m->memberId, m->planName,
                   m->startDate, m->endDate, m->fee, m->status) >= 0;
}

/* Save a payment */
static int savePayment(Payment *p)
{
    if (checkPaymentID(p->paymentId)) {
        printf("This Payment ID is already used.\n");
        return 0;
    }

    FILE *file = fopen(PAYMENT_FILE, "a");
    if (file == NULL) {
        printf("Cannot open payment file.\n");
        return 0;
    }

    int saved = fprintf(file, "%s %s %.2f %s %s\n",
                        p->paymentId, p->memberId, p->amount,
                        p->paymentDate, p->method) >= 0;

    if (fclose(file) != 0) saved = 0;
    return saved;
}

/* Take payment for a membership */
static int takePayment(Membership *m)
{
    Payment p;
    char name[50];

    printf("Confirm member name: ");
    if (scanf("%49s", name) != 1) return 0;

    if (!checkMember(m->memberId, name)) {
        printf("Member ID and name do not match.\n");
        return 0;
    }

    printf("Payment ID: ");
    if (scanf("%9s", p.paymentId) != 1) return 0;

    printf("Payment date (YYYY-MM-DD): ");
    if (scanf("%10s", p.paymentDate) != 1) return 0;

    printf("Payment method (Cash/Card/Transfer): ");
    if (scanf("%19s", p.method) != 1) return 0;

    strcpy(p.memberId, m->memberId);
    p.amount = m->fee;

    if (savePayment(&p)) {
        printf("Payment saved successfully.\n");
        return 1;
    }

    return 0;
}

/* Assign a membership */
void assignMembership(void)
{
    Membership m;
    char name[50], answer;

    printf("\n===== ASSIGN MEMBERSHIP =====\n");

    printf("Membership ID: ");
    if (scanf("%9s", m.membershipId) != 1) return;

    if (checkMembershipID(m.membershipId)) {
        printf("Membership ID already exists.\n");
        return;
    }

    printf("Member ID: ");
    if (scanf("%9s", m.memberId) != 1) return;

    printf("Member name: ");
    if (scanf("%49s", name) != 1) return;

    if (!checkMember(m.memberId, name)) {
        printf("Member ID and name do not match.\n");
        return;
    }

    printf("Plan (Monthly/Quarterly/Yearly): ");
    if (scanf("%19s", m.planName) != 1) return;

    printf("Start date (YYYY-MM-DD): ");
    if (scanf("%10s", m.startDate) != 1) return;

    printf("End date (YYYY-MM-DD): ");
    if (scanf("%10s", m.endDate) != 1) return;

    if (strcmp(m.startDate, m.endDate) >= 0) {
        printf("End date must be after start date.\n");
        return;
    }

    printf("Membership fee: ");
    if (scanf("%f", &m.fee) != 1 || m.fee <= 0) {
        printf("Fee must be greater than zero.\n");
        return;
    }

    strcpy(m.status, "ACTIVE");

    FILE *file = fopen(MEMBERSHIP_FILE, "a");
    if (file == NULL) {
        printf("Cannot open membership file.\n");
        return;
    }

    int saved = writeMembership(&m, file);
    if (fclose(file) != 0) saved = 0;

    if (!saved) {
        printf("Could not save membership.\n");
        return;
    }

    printf("Membership assigned successfully.\n");
    printf("Pay now? (Y/N): ");

    if (scanf(" %c", &answer) != 1) return;

    if (answer == 'Y' || answer == 'y') {
        if (!takePayment(&m))
            printf("Payment can be recorded later.\n");
    } else {
        printf("Payment can be recorded later.\n");
    }
}

/* Display all memberships */
void viewMemberships(void)
{
    Membership m;
    FILE *file = fopen(MEMBERSHIP_FILE, "r");

    if (file == NULL) {
        printf("No memberships found.\n");
        return;
    }

    printf("\n===== ALL MEMBERSHIPS =====\n");

    while (readMembership(&m, file)) {
        printf("\nMembership ID: %s\n", m.membershipId);
        printf("Member ID: %s\n", m.memberId);
        printf("Plan: %s\n", m.planName);
        printf("Start date: %s\n", m.startDate);
        printf("End date: %s\n", m.endDate);
        printf("Fee: %.2f\n", m.fee);
        printf("Status: %s\n", m.status);
    }

    fclose(file);
}

/* Renew a membership */
void renewMembership(void)
{
    Membership m;
    char id[10];
    int found = 0, error = 0;

    printf("Membership ID to renew: ");
    if (scanf("%9s", id) != 1) return;

    FILE *file = fopen(MEMBERSHIP_FILE, "r");
    if (file == NULL) {
        printf("No memberships found.\n");
        return;
    }

    FILE *temp = fopen(TEMP_FILE, "w");
    if (temp == NULL) {
        fclose(file);
        printf("Cannot create temporary file.\n");
        return;
    }

    while (readMembership(&m, file)) {
        if (strcmp(m.membershipId, id) == 0) {
            found = 1;

            if (strcmp(m.status, "CANCELLED") == 0) {
                printf("Cancelled membership cannot be renewed.\n");
                error = 1;
            } else {
                printf("New start date (YYYY-MM-DD): ");
                if (scanf("%10s", m.startDate) != 1) error = 1;

                if (!error) {
                    printf("New end date (YYYY-MM-DD): ");
                    if (scanf("%10s", m.endDate) != 1) error = 1;
                }

                if (!error && strcmp(m.startDate, m.endDate) >= 0) {
                    printf("End date must be after start date.\n");
                    error = 1;
                }

                if (!error) {
                    printf("New fee: ");
                    if (scanf("%f", &m.fee) != 1 || m.fee <= 0)
                        error = 1;
                }

                if (!error) strcpy(m.status, "ACTIVE");
            }
        }

        if (!writeMembership(&m, temp)) error = 1;
    }

    fclose(file);
    if (fclose(temp) != 0) error = 1;

    if (!found || error) {
        printf(!found ? "Membership not found.\n" : "Renewal failed.\n");
        remove(TEMP_FILE);
        return;
    }

    if (remove(MEMBERSHIP_FILE) != 0) {
        printf("Could not update membership file.\n");
        remove(TEMP_FILE);
        return;
    }

    if (rename(TEMP_FILE, MEMBERSHIP_FILE) != 0) {
        printf("Could not save renewed membership.\n");
        return;
    }

    printf("Membership renewed successfully.\n");
}

/* Display expired memberships */
void viewExpiredMemberships(void)
{
    Membership m;
    char today[11];

    time_t now = time(NULL);
    struct tm *current = localtime(&now);

    if (current == NULL) {
        printf("Cannot get today's date.\n");
        return;
    }

    strftime(today, sizeof(today), "%Y-%m-%d", current);

    FILE *file = fopen(MEMBERSHIP_FILE, "r");
    if (file == NULL) {
        printf("No memberships found.\n");
        return;
    }

    int count = 0;
    printf("\n===== EXPIRED MEMBERSHIPS =====\n");

    while (readMembership(&m, file)) {
        if (strcmp(m.status, "ACTIVE") == 0 &&
            strcmp(m.endDate, today) < 0) {
            printf("Membership: %s | Member: %s | Ended: %s\n",
                   m.membershipId, m.memberId, m.endDate);
            count++;
        }
    }

    if (count == 0)
        printf("No expired memberships found.\n");

    fclose(file);
}

/* Cancel a membership */
void cancelMembership(void)
{
    Membership m;
    char id[10];
    int found = 0, error = 0;

    printf("Membership ID to cancel: ");
    if (scanf("%9s", id) != 1) return;

    FILE *file = fopen(MEMBERSHIP_FILE, "r");
    if (file == NULL) {
        printf("No memberships found.\n");
        return;
    }

    FILE *temp = fopen(TEMP_FILE, "w");
    if (temp == NULL) {
        fclose(file);
        printf("Cannot create temporary file.\n");
        return;
    }

    while (readMembership(&m, file)) {
        if (strcmp(m.membershipId, id) == 0) {
            found = 1;
            strcpy(m.status, "CANCELLED");
        }

        if (!writeMembership(&m, temp)) error = 1;
    }

    fclose(file);
    if (fclose(temp) != 0) error = 1;

    if (!found || error) {
        printf(!found ? "Membership not found.\n" : "Cancellation failed.\n");
        remove(TEMP_FILE);
        return;
    }

    if (remove(MEMBERSHIP_FILE) != 0) {
        printf("Could not update membership file.\n");
        remove(TEMP_FILE);
        return;
    }

    if (rename(TEMP_FILE, MEMBERSHIP_FILE) != 0) {
        printf("Could not save cancellation.\n");
        return;
    }

    printf("Membership cancelled successfully.\n");
}

/* Record a payment later */
void recordPayment(void)
{
    Payment p;
    char name[50];

    printf("Payment ID: ");
    if (scanf("%9s", p.paymentId) != 1) return;

    printf("Member ID: ");
    if (scanf("%9s", p.memberId) != 1) return;

    printf("Member name: ");
    if (scanf("%49s", name) != 1) return;

    if (!checkMember(p.memberId, name)) {
        printf("Member ID and name do not match.\n");
        return;
    }

    printf("Amount: ");
    if (scanf("%f", &p.amount) != 1 || p.amount <= 0) {
        printf("Amount must be greater than zero.\n");
        return;
    }

    printf("Payment date (YYYY-MM-DD): ");
    if (scanf("%10s", p.paymentDate) != 1) return;

    printf("Payment method (Cash/Card/Transfer): ");
    if (scanf("%19s", p.method) != 1) return;

    if (savePayment(&p))
        printf("Payment saved successfully.\n");
}

/* Display all payments */
void viewPayments(void)
{
    Payment p;
    FILE *file = fopen(PAYMENT_FILE, "r");

    if (file == NULL) {
        printf("No payments found.\n");
        return;
    }

    printf("\n===== PAYMENT HISTORY =====\n");

    while (fscanf(file, "%9s %9s %f %10s %19s",
                  p.paymentId, p.memberId, &p.amount,
                  p.paymentDate, p.method) == 5) {
        printf("%s | Member: %s | %.2f | %s | %s\n",
               p.paymentId, p.memberId, p.amount,
               p.paymentDate, p.method);
    }

    fclose(file);
}

/* Main membership menu */
void membershipMenu(void)
{
    int choice;

    do {
        printf("\n===== MEMBERSHIP & PAYMENTS =====\n");
        printf("1. Assign Membership\n");
        printf("2. View Memberships\n");
        printf("3. Record Payment\n");
        printf("4. View Payments\n");
        printf("5. Renew Membership\n");
        printf("6. View Expired Memberships\n");
        printf("7. Cancel Membership\n");
        printf("8. Return to Dashboard\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf("Please enter a number.\n");
            continue;
        }

        switch (choice) {
            case 1: assignMembership(); break;
            case 2: viewMemberships(); break;
            case 3: recordPayment(); break;
            case 4: viewPayments(); break;
            case 5: renewMembership(); break;
            case 6: viewExpiredMemberships(); break;
            case 7: cancelMembership(); break;
            case 8: printf("Returning to dashboard...\n"); break;
            default: printf("Invalid choice.\n");
        }

    } while (choice != 8);
}
