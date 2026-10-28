#ifndef AUTH_H
#define AUTH_H
/*
 * AuraFlex Gym Management System
 * Authentication Header
 */

#define USER_ID_LENGTH 10
#define USERNAME_LENGTH 30
#define PASSWORD_LENGTH 50
#define ROLE_LENGTH 20
#define STATUS_LENGTH 15

/*
 * User structure
 * Stores the details of a system user.
 */
typedef struct {
    char userId[USER_ID_LENGTH];
    char userName[USERNAME_LENGTH];
    char password[PASSWORD_LENGTH];
    char role[ROLE_LENGTH];
    char status[STATUS_LENGTH];
} User;

/*
 * Login function
 *
 * Parameters:
 *     loggedUser - pointer to the User structure
 *                   where authenticated user details are stored.
 *
 * Returns:
 *     1 - Login successful
 *     0 - Login unsuccessful
 */
int login(User *loggedUser);

#endif