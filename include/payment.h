#ifndef PAYMENT_H
#define PAYMENT_H

void paymentRecord(void);
int processMembershipPayment(const char *memberId, float amount);
void searchPaymentHistory(void);
void viewPaymentHistory(void);
void calculateTotalPayments(void);
void paymentMenu(void);

#endif
