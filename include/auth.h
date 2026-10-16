#ifndef AUTH_H
#define AUTH_H

typedef struct {
    char userId[10];
    char userName[30];
    char password[50];
    char role[20];
    char status[15];
} User;

int login(User *loggedUser);

#endif
