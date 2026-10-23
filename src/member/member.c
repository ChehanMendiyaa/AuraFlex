
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "member.h"
#include "system.h"
#include "ui.h"

#define MEMBERS_FILE "data/members.txt"
#define TEMP_FILE "data/temp.txt"

/* Read one member record from the file */
static int readMember(FILE *file, Member *m)
{
    return fscanf(file, "%9s %49s %19s %59s %14s",
                  m->memberId,
                  m->fullName,
                  m->phone,
                  m->email,
                  m->status) == 5;
}

/* Check whether a member ID already exists */
static int memberExists(const char *id)
{
    Member m;
    FILE *file = fopen(MEMBERS_FILE, "r");

    if (file == NULL)
        return 0;

    while (readMember(file, &m))
    {
        if (strcmp(id, m.memberId) == 0)
        {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

/* Register a new member */
void registerMember(void)
{
    Member m;
    FILE *file;

    clearScreen();

    printf("\n===== REGISTER MEMBER =====\n");

    printf("Member ID: ");
    if (scanf("%9s", m.memberId) != 1)
        return;

    if (memberExists(m.memberId))
    {
        printf("Member ID already exists!\n");
        return;
    }

    printf("Name (one word): ");
    if (scanf("%49s", m.fullName) != 1)
        return;

    printf("Phone: ");
    if (scanf("%19s", m.phone) != 1)
        return;

    printf("Email: ");
    if (scanf("%59s", m.email) != 1)
        return;

    strcpy(m.status, "ACTIVE");

    file = fopen(MEMBERS_FILE, "a");

    if (file == NULL)
    {
        printf("Cannot open members file.\n");
        return;
    }

    fprintf(file, "%s %s %s %s %s\n",
            m.memberId,
            m.fullName,
            m.phone,
            m.email,
            m.status);

    fclose(file);

    printf("\nMember registered successfully!\n");
}

/* View all members */
void viewMembers(void)
{
    Member m;
    FILE *file;
    int total = 0;

    clearScreen();

    file = fopen(MEMBERS_FILE, "r");

    if (file == NULL)
    {
        printf("\nNo member records found.\n");
        return;
    }

    /* Count members first */
    while (readMember(file, &m))
    {
        total++;
    }

    rewind(file);

    printf("╔════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                       All MEMBERS                                              ║\n");
    printf("║                                                                     No of Members: %-10d  ║\n", total);
    printf("╠════════════╦══════════════════╦══════════════╦══════════════════════════════════════╦══════════╣\n");
    printf("║ MEMBER ID  ║ NAME             ║ PHONE        ║ EMAIL                                ║ STATUS   ║\n");
    printf("╠════════════╬══════════════════╬══════════════╬══════════════════════════════════════╬══════════╣\n");

    while (readMember(file, &m))
    {
        printf("║ %-10.10s ║ %-16.16s ║ %-12.12s ║ %-36.36s ║ %-8.8s ║\n",
               m.memberId,
               m.fullName,
               m.phone,
               m.email,
               m.status);
    }

    printf("╚════════════╩══════════════════╩══════════════╩══════════════════════════════════════╩══════════╝\n");

    fclose(file);
}

/* Search for a member by ID */
void searchMember(void)
{
    Member m;
    char id[10];
    int found = 0;

    FILE *file;

    clearScreen();

    printf("\n===== SEARCH MEMBER =====\n");
    printf("\n");
    printf("Enter Member ID: ");

    if (scanf("%9s", id) != 1)
        return;

    file = fopen(MEMBERS_FILE, "r");

    if (file == NULL)
    {
        printf("No member records found.\n");
        return;
    }

    while (readMember(file, &m))
    {
        if (strcmp(id, m.memberId) == 0)
        {
            printf("\n===== MEMBER FOUND =====\n");
            printf("ID:     %s\n", m.memberId);
            printf("Name:   %s\n", m.fullName);
            printf("Phone:  %s\n", m.phone);
            printf("Email:  %s\n", m.email);
            printf("Status: %s\n", m.status);

            found = 1;
            break;
        }
    }

    fclose(file);

    if (!found)
        printf("Member not found.\n");
}

/* Update member details */
void updateMember(void)
{
    Member m;
    char id[10];
    FILE *in, *out;
    int found = 0;

    clearScreen();

    printf("\n===== UPDATE MEMBER =====\n");
    printf("\n");
    printf("Enter Member ID: ");

    if (scanf("%9s", id) != 1)
        return;

    in = fopen(MEMBERS_FILE, "r");

    if (in == NULL)
    {
        printf("No member records found.\n");
        return;
    }

    out = fopen(TEMP_FILE, "w");

    if (out == NULL)
    {
        fclose(in);
        printf("Cannot create temporary file.\n");
        return;
    }

    while (readMember(in, &m))
    {
        if (strcmp(id, m.memberId) == 0)
        {
            printf("New name (one word): ");
            if (scanf("%49s", m.fullName) != 1)
            {
                fclose(in);
                fclose(out);
                remove(TEMP_FILE);
                return;
            }

            printf("New phone: ");
            if (scanf("%19s", m.phone) != 1)
            {
                fclose(in);
                fclose(out);
                remove(TEMP_FILE);
                return;
            }

            printf("New email: ");
            if (scanf("%59s", m.email) != 1)
            {
                fclose(in);
                fclose(out);
                remove(TEMP_FILE);
                return;
            }

            found = 1;
        }

        fprintf(out, "%s %s %s %s %s\n",
                m.memberId,
                m.fullName,
                m.phone,
                m.email,
                m.status);
    }

    fclose(in);

    if (fclose(out) != 0)
    {
        remove(TEMP_FILE);
        printf("Error writing temporary file.\n");
        return;
    }

    if (!found)
    {
        remove(TEMP_FILE);
        printf("Member not found.\n");
        return;
    }

    /* Replace the original file */
    if (remove(MEMBERS_FILE) != 0)
    {
        remove(TEMP_FILE);
        printf("Error replacing members file.\n");
        return;
    }

    if (rename(TEMP_FILE, MEMBERS_FILE) != 0)
    {
        printf("Could not rename temporary file. Check data/temp.txt.\n");
        return;
    }

    printf("Member updated successfully!\n");
}

/* Change member status to FROZEN or ACTIVE */
static void changeStatus(const char *newStatus)
{
    Member m;
    char id[10];
    FILE *in, *out;
    int found = 0;

    clearScreen();

    printf("\n===== CHANGE MEMBER STATUS =====\n");
    printf("\n");
    printf("Enter Member ID: ");

    if (scanf("%9s", id) != 1)
        return;

    in = fopen(MEMBERS_FILE, "r");

    if (in == NULL)
    {
        printf("No member records found.\n");
        return;
    }

    out = fopen(TEMP_FILE, "w");

    if (out == NULL)
    {
        fclose(in);
        printf("Cannot create temporary file.\n");
        return;
    }

    while (readMember(in, &m))
    {
        if (strcmp(id, m.memberId) == 0)
        {
            if (strcmp(m.status, newStatus) == 0)
            {
                printf("Member already has status %s.\n", newStatus);
            }
            else
            {
                strcpy(m.status, newStatus);
                printf("Member status changed to %s.\n", newStatus);
            }

            found = 1;
        }

        fprintf(out, "%s %s %s %s %s\n",
                m.memberId,
                m.fullName,
                m.phone,
                m.email,
                m.status);
    }

    fclose(in);

    if (fclose(out) != 0)
    {
        remove(TEMP_FILE);
        printf("Error writing temporary file.\n");
        return;
    }

    if (!found)
    {
        remove(TEMP_FILE);
        printf("Member not found.\n");
        return;
    }

    if (remove(MEMBERS_FILE) != 0)
    {
        remove(TEMP_FILE);
        printf("Error replacing members file.\n");
        return;
    }

    if (rename(TEMP_FILE, MEMBERS_FILE) != 0)
    {
        printf("Could not rename temporary file. Check data/temp.txt.\n");
        return;
    }
}

/* Freeze a member */
void freezeMember(void)
{
    changeStatus("FROZEN");
}

/* Unfreeze a member */
void unfreezeMember(void)
{
    changeStatus("ACTIVE");
}

/* Member management menu */
void memberMenu(void)
{
    int choice;

    do
    {
        clearScreen();
        printLogoWithoutl();

        printf("║   [1]  Register Member                                                         ║\n");
        printf("║   [2]  View All Members                                                        ║\n");
        printf("║   [3]  Search Member                                                           ║\n");
        printf("║   [4]  Update Member                                                           ║\n");
        printf("║   [5]  Freeze Member                                                           ║\n");
        printf("║   [6]  Unfreeze Member                                                         ║\n");
        printf("║   [7]  Return to Dashboard                                                     ║\n");
        printf("╚════════════════════════════════════════════════════════════════════════════════╝\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1)
        {
            int ch;

            while ((ch = getchar()) != '\n' && ch != EOF)
            {
                /* Clear invalid input */
            }

            printf("Please enter a valid number.\n");
            choice = 0;
            continue;
        }

        switch (choice)
        {
            case 1:
                registerMember();
                break;

            case 2:
                viewMembers();
                break;

            case 3:
                searchMember();
                break;

            case 4:
                updateMember();
                break;

            case 5:
                freezeMember();
                break;

            case 6:
                unfreezeMember();
                break;

            case 7:
                printf("Returning to dashboard...\n");
                break;

            default:
                printf("Invalid choice.\n");
        }

        if (choice >= 1 && choice <= 6)
        {
            int ch;

            printf("\nPress Enter to continue...");

            while ((ch = getchar()) != '\n' && ch != EOF)
            {
                /* Clear remaining input */
            }

            getchar();
        }

    } while (choice != 7);
}
