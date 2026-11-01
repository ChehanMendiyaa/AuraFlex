#ifndef AUTH_H
#define AUTH_H

#define USER_ID_LENGTH 10
#define USERNAME_LENGTH 30
#define PASSWORD_LENGTH 50
#define ROLE_LENGTH 20
#define STATUS_LENGTH 15

typedef struct {
    char userId[USER_ID_LENGTH];
    char userName[USERNAME_LENGTH];
    char password[PASSWORD_LENGTH];
    char role[ROLE_LENGTH];
    char status[STATUS_LENGTH];
} User;

int login(User *loggedUser);

#endif