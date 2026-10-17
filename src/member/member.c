#include <stdio.h>
#include <string.h>
#include "member.h"

#define MEMBERS_FILE "data/members.txt"

void registerMember(void)
{
    Member member;
    FILE *file;

    printf("\n===== REGISTER MEMBER =====\n");
    printf("Member ID: ");
    scanf("%9s", member.memberId);
    printf("Name (one word): ");
    scanf("%49s", member.fullName);
    printf("Phone: ");
    scanf("%19s", member.phone);
    printf("Email: ");
    scanf("%59s", member.email);

    strcpy(member.status, "ACTIVE");

    file = fopen(MEMBERS_FILE, "a");
    
    if (file == NULL) {
        printf("Error opening data/members.txt\n");
        return;
    }

    fprintf(file, "%s %s %s %s %s\n",
            member.memberId,
             member.fullName,
             member.phone,
            member.email, 
            member.status);
    fclose(file);
    printf("Member registered successfully!\n");
}

void viewMembers(void)
{
    Member member;
    FILE *file = fopen(MEMBERS_FILE, "r");

    if (file == NULL) {
        printf("No member records found yet.\n");
        return;
    }

    printf("\n===== ALL MEMBERS =====\n");
    while (fscanf(file, "%9s %49s %19s %59s %14s",
                  member.memberId,
                   member.fullName,
                  member.phone,
                   member.email,
                   member.status) == 5) {

        printf("\nID: %s\nName: %s\nPhone: %s\nEmail: %s\nStatus: %s\n",
               member.memberId,
                member.fullName,
                member.phone,
                 member.email,
                  member.status
                );
    }

    fclose(file);
}

void searchMember(void)
{
    Member member;
    char searchId[10];
    int found = 0;

    printf("Enter Member ID: ");
    scanf("%9s", searchId);

    FILE *file = fopen(MEMBERS_FILE, "r");
    if (file == NULL) {
        printf("No member records found.\n");
        return;
    }

    while (fscanf(file, "%9s %49s %19s %59s %14s",
                  member.memberId,
                   member.fullName,
                  member.phone,
                   member.email,
                    member.status
                ) == 5) {

        if (strcmp(searchId, member.memberId) == 0) {
            printf("\nID: %s\nName: %s\nPhone: %s\nEmail: %s\nStatus: %s\n",
                   member.memberId,
                    member.fullName,
                   member.phone,
                    member.email,
                     member.status);
            found = 1;
            break;
        }
    }

    fclose(file);
    if (!found) {
        printf("Member not found.\n");
    }
}

void memberMenu(void)
{
    int choice;

    do {
        printf("\n===== MEMBER MANAGEMENT =====\n");
        printf("1. Register Member\n");
        printf("2. View All Members\n");
        printf("3. Search Member\n");
        printf("4. Return to Dashboard\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            printf("Please enter a number.\n");
            choice = 0;
            continue;
        }

        switch (choice) {
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
             printf("Returning to dashboard...\n"); 
             break;
            default:
             printf("Invalid choice. Try again.\n");
        }
    } while (choice != 4);
}
