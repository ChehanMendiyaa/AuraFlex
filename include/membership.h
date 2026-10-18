#ifndef MEMBERSHIP_H
#define MEMBERSHIP_H

typedef struct {
    char membershipId[10];
    char memberId[10];
    char planName[20];
    char startDate[11];
    char endDate[11];
    float fee;
    char status[15];

} Membership;

typedef struct {
    char paymentId[10];
    char memberId[10];
    float amount;
    char paymentDate[11];
    char method[20];

} Payment;

void membershipMenu(void);
void assignMembership(void);
void viewMemberships(void);
void recordPayment(void);
void viewPayments(void);

#endif

