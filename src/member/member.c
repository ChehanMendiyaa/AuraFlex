
#include <stdio.h>
#include <string.h>
#include "member.h"

#define FILE_NAME "data/members.txt"
#define TEMP_FILE "data/temp.txt"

/* Check whether a member ID already exists */
static int memberExists(char id[])
{
    Member m;
    FILE *fp = fopen(FILE_NAME, "r");

    if (fp == NULL) return 0;

    while (fscanf(fp, "%9s %49s %19s %59s %14s",
           m.memberId, m.fullName, m.phone, m.email, m.status) == 5) {
        if (strcmp(id, m.memberId) == 0) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

/* Register a new member */
void registerMember(void)
{
    Member m;
    FILE *fp;

    printf("\n===== REGISTER MEMBER =====\n");
    printf("Member ID: ");
    scanf("%9s", m.memberId);

    if (memberExists(m.memberId)) {
        printf("Member ID already exists!\n");
        return;
    }

    printf("Name (one word): ");
    scanf("%49s", m.fullName);
    printf("Phone: ");
    scanf("%19s", m.phone);
    printf("Email: ");
    scanf("%59s", m.email);

    strcpy(m.status, "ACTIVE");

    fp = fopen(FILE_NAME, "a");
    if (fp == NULL) {
        printf("Cannot open members file.\n");
        return;
    }

    fprintf(fp, "%s %s %s %s %s\n",
            m.memberId, m.fullName, m.phone, m.email, m.status);
    fclose(fp);

    printf("Member registered successfully!\n");
}

/* View all members */
void viewMembers(void)
{
    Member m;
    FILE *fp = fopen(FILE_NAME, "r");

    if (fp == NULL) {
        printf("No members found.\n");
        return;
    }

    printf("\n===== ALL MEMBERS =====\n");

    while (fscanf(fp, "%9s %49s %19s %59s %14s",
           m.memberId, m.fullName, m.phone, m.email, m.status) == 5) {
        printf("\nID: %s\nName: %s\nPhone: %s\nEmail: %s\nStatus: %s\n",
               m.memberId, m.fullName, m.phone, m.email, m.status);
    }

    fclose(fp);
}

/* Search for a member */
void searchMember(void)
{
    Member m;
    char id[10];
    FILE *fp;

    printf("Enter Member ID: ");
    scanf("%9s", id);

    fp = fopen(FILE_NAME, "r");
    if (fp == NULL) {
        printf("No members found.\n");
        return;
    }

    while (fscanf(fp, "%9s %49s %19s %59s %14s",
           m.memberId, m.fullName, m.phone, m.email, m.status) == 5) {
        if (strcmp(id, m.memberId) == 0) {
            printf("\nID: %s\nName: %s\nPhone: %s\nEmail: %s\nStatus: %s\n",
                   m.memberId, m.fullName, m.phone, m.email, m.status);
            fclose(fp);
            return;
        }
    }

    fclose(fp);
    printf("Member not found.\n");
}

/* Update member details */
void updateMember(void)
{
    Member m;
    char id[10];
    FILE *in, *out;
    int found = 0;

    printf("Enter Member ID to update: ");
    scanf("%9s", id);

    in = fopen(FILE_NAME, "r");
    if (in == NULL) {
        printf("No members found.\n");
        return;
    }

    out = fopen(TEMP_FILE, "w");
    if (out == NULL) {
        fclose(in);
        printf("Cannot create temporary file.\n");
        return;
    }

    while (fscanf(in, "%9s %49s %19s %59s %14s",
           m.memberId, m.fullName, m.phone, m.email, m.status) == 5) {

        if (strcmp(id, m.memberId) == 0) {
            printf("New name (one word): ");
            scanf("%49s", m.fullName);
            printf("New phone: ");
            scanf("%19s", m.phone);
            printf("New email: ");
            scanf("%59s", m.email);
            found = 1;
        }

        fprintf(out, "%s %s %s %s %s\n",
                m.memberId, m.fullName, m.phone, m.email, m.status);
    }

    fclose(in);
    fclose(out);

    if (!found) {
        remove(TEMP_FILE);
        printf("Member not found.\n");
        return;
    }

    if (remove(FILE_NAME) != 0 || rename(TEMP_FILE, FILE_NAME) != 0) {
        printf("Error saving updated member records.\n");
        return;
    }

    printf("Member updated successfully!\n");
}

/* Change a member's status */
static void changeStatus(char newStatus[])
{
    Member m;
    char id[10];
    FILE *in, *out;
    int found = 0;

    printf("Enter Member ID: ");
    scanf("%9s", id);

    in = fopen(FILE_NAME, "r");
    if (in == NULL) {
        printf("No members found.\n");
        return;
    }

    out = fopen(TEMP_FILE, "w");
    if (out == NULL) {
        fclose(in);
        printf("Cannot create temporary file.\n");
        return;
    }

    while (fscanf(in, "%9s %49s %19s %59s %14s",
           m.memberId, m.fullName, m.phone, m.email, m.status) == 5) {

        if (strcmp(id, m.memberId) == 0) {
            if (strcmp(m.status, newStatus) == 0) {
                printf("Member already has status %s.\n", newStatus);
            }
            strcpy(m.status, newStatus);
            found = 1;
        }

        fprintf(out, "%s %s %s %s %s\n",
                m.memberId, m.fullName, m.phone, m.email, m.status);
    }

    fclose(in);
    fclose(out);

    if (!found) {
        remove(TEMP_FILE);
        printf("Member not found.\n");
        return;
    }

    if (remove(FILE_NAME) != 0 || rename(TEMP_FILE, FILE_NAME) != 0) {
        printf("Error saving member status.\n");
        return;
    }

    printf("Member status changed to %s.\n", newStatus);
}

void freezeMember(void)
{
    changeStatus("FROZEN");
}

void unfreezeMember(void)
{
    changeStatus("ACTIVE");
}

/* Member management menu */
void memberMenu(void)
{
    int choice;

    do {
        printf("\n===== MEMBER MANAGEMENT =====\n");
        printf("1. Register Member\n");
        printf("2. View Members\n");
        printf("3. Search Member\n");
        printf("4. Update Member\n");
        printf("5. Freeze Member\n");
        printf("6. Unfreeze Member\n");
        printf("7. Return to Dashboard\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            printf("Please enter a valid number.\n");
            choice = 0;
            continue;
        }

        switch (choice) {
            case 1: registerMember(); break;
            case 2: viewMembers(); break;
            case 3: searchMember(); break;
            case 4: updateMember(); break;
            case 5: freezeMember(); break;
            case 6: unfreezeMember(); break;
            case 7: printf("Returning to dashboard...\n"); break;
            default: printf("Invalid choice.\n");
        }

    } while (choice != 7);
}
