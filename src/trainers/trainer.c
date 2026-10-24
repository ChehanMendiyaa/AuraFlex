#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trainer.h"
#include "ui.h"

#define TRAINERS_FILE "data/trainers.txt"
#define TEMP_FILE "data/trainers_temp.txt"

typedef struct {
    char trainerId[10];
    char fullName[50];
    char phone[20];
    char specialization[40];
    char status[15];
} Trainer;

/* Read one trainer record */
static int readTrainer(FILE *file, Trainer *trainer)
{
    return fscanf(file, "%9s %49s %19s %39s %14s",
                  trainer->trainerId,
                  trainer->fullName,
                  trainer->phone,
                  trainer->specialization,
                  trainer->status) == 5;
}

/* Check whether a trainer ID already exists */
static int trainerIdExists(const char trainerId[])
{
    FILE *file = fopen(TRAINERS_FILE, "r");
    Trainer trainer;

    if (file == NULL)
        return 0;

    while (readTrainer(file, &trainer)) {
        if (strcmp(trainer.trainerId, trainerId) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

/* Find a trainer and return the matching record */
static int findTrainer(const char trainerId[], Trainer *found)
{
    FILE *file = fopen(TRAINERS_FILE, "r");
    Trainer trainer;

    if (file == NULL) {
        printf("Error: Cannot open trainers.txt.\n");
        return -1;
    }

    while (readTrainer(file, &trainer)) {
        if (strcmp(trainer.trainerId, trainerId) == 0) {
            *found = trainer;
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

/* Register a new trainer */
void registerTrainer(void)
{
    FILE *file;
    Trainer trainer;

    printf("\n--- Register New Trainer ---\n");

    printf("Enter Trainer ID: ");
    if (scanf("%9s", trainer.trainerId) != 1)
        return;

    if (trainerIdExists(trainer.trainerId)) {
        printf("Trainer ID already exists.\n");
        return;
    }

    printf("Enter trainer name (one word): ");
    if (scanf("%49s", trainer.fullName) != 1)
        return;

    printf("Enter phone number: ");
    if (scanf("%19s", trainer.phone) != 1)
        return;

    printf("Enter specialization (one word): ");
    if (scanf("%39s", trainer.specialization) != 1)
        return;

    strcpy(trainer.status, "ACTIVE");

    file = fopen(TRAINERS_FILE, "a");

    if (file == NULL) {
        printf("Error: Cannot open trainers.txt for writing.\n");
        return;
    }

    if (fprintf(file, "%s %s %s %s %s\n",
                trainer.trainerId,
                trainer.fullName,
                trainer.phone,
                trainer.specialization,
                trainer.status) < 0) {
        printf("Error writing trainer record.\n");
        fclose(file);
        return;
    }

    if (fclose(file) != 0) {
        printf("Error saving trainer record.\n");
        return;
    }

    printf("Trainer registered successfully!\n");
}

/* View all trainers */
void viewTrainers(void)
{
    FILE *file = fopen(TRAINERS_FILE, "r");
    Trainer trainer;
    int found = 0;

    printf("\n--- All Trainers ---\n");
    printf("%-10s %-20s %-15s %-20s %-12s\n",
           "ID", "Name", "Phone", "Specialization", "Status");
    printf("--------------------------------------------------------------------------\n");

    if (file == NULL) {
        printf("No trainer file found.\n");
        return;
    }

    while (readTrainer(file, &trainer)) {
        printf("%-10s %-20s %-15s %-20s %-12s\n",
               trainer.trainerId,
               trainer.fullName,
               trainer.phone,
               trainer.specialization,
               trainer.status);
        found = 1;
    }

    fclose(file);

    if (!found)
        printf("No trainer records found.\n");
}

/* Search trainers by ID */
void searchTrainer(void)
{
    Trainer trainer;
    char trainerId[10];
    int result;

    printf("\n--- Search Trainer ---\n");
    printf("Enter Trainer ID: ");

    if (scanf("%9s", trainerId) != 1)
        return;

    result = findTrainer(trainerId, &trainer);

    if (result == -1)
        return;

    if (result == 0) {
        printf("Trainer not found.\n");
        return;
    }

    printf("\nTrainer ID: %s\n", trainer.trainerId);
    printf("Name: %s\n", trainer.fullName);
    printf("Phone: %s\n", trainer.phone);
    printf("Specialization: %s\n", trainer.specialization);
    printf("Status: %s\n", trainer.status);
}

/* Update trainer details */
void updateTrainer(void)
{
    FILE *file, *temp;
    Trainer trainer;
    char trainerId[10];
    int found = 0;
    int failed = 0;

    printf("\n--- Update Trainer ---\n");
    printf("Enter Trainer ID: ");

    if (scanf("%9s", trainerId) != 1)
        return;

    file = fopen(TRAINERS_FILE, "r");

    if (file == NULL) {
        printf("No trainer file found.\n");
        return;
    }

    temp = fopen(TEMP_FILE, "w");

    if (temp == NULL) {
        fclose(file);
        printf("Error creating temporary file.\n");
        return;
    }

    while (readTrainer(file, &trainer)) {
        if (strcmp(trainer.trainerId, trainerId) == 0) {
            found = 1;

            printf("Enter new name (one word): ");
            if (scanf("%49s", trainer.fullName) != 1) {
                failed = 1;
                break;
            }

            printf("Enter new phone number: ");
            if (scanf("%19s", trainer.phone) != 1) {
                failed = 1;
                break;
            }

            printf("Enter new specialization (one word): ");
            if (scanf("%39s", trainer.specialization) != 1) {
                failed = 1;
                break;
            }
        }

        if (fprintf(temp, "%s %s %s %s %s\n",
                    trainer.trainerId,
                    trainer.fullName,
                    trainer.phone,
                    trainer.specialization,
                    trainer.status) < 0) {
            failed = 1;
            break;
        }
    }

    if (ferror(file))
        failed = 1;

    fclose(file);

    if (fclose(temp) != 0)
        failed = 1;

    if (failed || !found) {
        remove(TEMP_FILE);

        if (failed)
            printf("Error processing trainer records. Original file kept.\n");
        else
            printf("Trainer not found.\n");

        return;
    }

    /* Preserve the original file if replacement fails. */
    if (rename(TEMP_FILE, TRAINERS_FILE) != 0) {
        printf("Could not replace trainers.txt.\n");
        printf("Your original file has been preserved.\n");
        remove(TEMP_FILE);
        return;
    }

    printf("Trainer details updated successfully.\n");
}

/* Activate or deactivate a trainer */
void changeTrainerStatus(void)
{
    FILE *file, *temp;
    Trainer trainer;
    char trainerId[10];
    char newStatus[15];
    int found = 0;
    int failed = 0;

    printf("\n--- Change Trainer Status ---\n");
    printf("Enter Trainer ID: ");

    if (scanf("%9s", trainerId) != 1)
        return;

    printf("Enter status (ACTIVE/INACTIVE): ");

    if (scanf("%14s", newStatus) != 1)
        return;

    if (strcmp(newStatus, "ACTIVE") != 0 &&
        strcmp(newStatus, "INACTIVE") != 0) {
        printf("Invalid status. Use ACTIVE or INACTIVE.\n");
        return;
    }

    file = fopen(TRAINERS_FILE, "r");

    if (file == NULL) {
        printf("No trainer file found.\n");
        return;
    }

    temp = fopen(TEMP_FILE, "w");

    if (temp == NULL) {
        fclose(file);
        printf("Error creating temporary file.\n");
        return;
    }

    while (readTrainer(file, &trainer)) {
        if (strcmp(trainer.trainerId, trainerId) == 0) {
            strcpy(trainer.status, newStatus);
            found = 1;
        }

        if (fprintf(temp, "%s %s %s %s %s\n",
                    trainer.trainerId,
                    trainer.fullName,
                    trainer.phone,
                    trainer.specialization,
                    trainer.status) < 0) {
            failed = 1;
            break;
        }
    }

    if (ferror(file))
        failed = 1;

    fclose(file);

    if (fclose(temp) != 0)
        failed = 1;

    if (failed || !found) {
        remove(TEMP_FILE);

        if (failed)
            printf("Error processing trainer records. Original file kept.\n");
        else
            printf("Trainer not found.\n");

        return;
    }

    if (rename(TEMP_FILE, TRAINERS_FILE) != 0) {
        printf("Could not replace trainers.txt.\n");
        printf("Your original file has been preserved.\n");
        remove(TEMP_FILE);
        return;
    }

    printf("Trainer status changed to %s successfully.\n", newStatus);
}

/* ================================================================== */
/*  ADDED: members of a trainer                                        */
/*  Links are saved in data/trainer_members.txt  (memberId trainerId)  */
/* ================================================================== */

#define MEMBERS_FILE "data/members.txt"
#define LINKS_FILE "data/trainer_members.txt"
#define LINKS_TEMP_FILE "data/trainer_members_temp.txt"
#define MAX_MEMBERS_PER_TRAINER 20

/* Check whether a member ID exists (first word of a line in members.txt) */
static int memberExists(const char memberId[])
{
    FILE *file = fopen(MEMBERS_FILE, "r");
    char word[100];
    int c;

    if (file == NULL)
        return 0;

    while (fscanf(file, "%99s", word) == 1) {
        if (strcmp(word, memberId) == 0) {
            fclose(file);
            return 1;
        }

        /* skip the rest of this line */
        c = getc(file);
        while (c != '\n' && c != EOF)
            c = getc(file);
    }

    fclose(file);
    return 0;
}

/* Find the trainer of a member. Returns 1 and fills trainerId if found. */
static int findMemberTrainer(const char memberId[], char trainerId[])
{
    FILE *file = fopen(LINKS_FILE, "r");
    char linkMember[10];
    char linkTrainer[10];

    if (file == NULL)
        return 0;

    while (fscanf(file, "%9s %9s", linkMember, linkTrainer) == 2) {
        if (strcmp(linkMember, memberId) == 0) {
            strcpy(trainerId, linkTrainer);
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

/* Count how many members a trainer has */
static int countMembersOfTrainer(const char trainerId[])
{
    FILE *file = fopen(LINKS_FILE, "r");
    char linkMember[10];
    char linkTrainer[10];
    int count = 0;

    if (file == NULL)
        return 0;

    while (fscanf(file, "%9s %9s", linkMember, linkTrainer) == 2) {
        if (strcmp(linkTrainer, trainerId) == 0)
            count++;
    }

    fclose(file);
    return count;
}

/* Assign a member to a trainer */
void assignMemberToTrainer(void)
{
    FILE *file;
    Trainer trainer;
    char memberId[10];
    char trainerId[10];
    char currentTrainer[10];
    int result;

    printf("\n--- Assign Member to Trainer ---\n");

    printf("Enter Member ID: ");
    if (scanf("%9s", memberId) != 1)
        return;

    if (!memberExists(memberId)) {
        printf("Member not found in members.txt.\n");
        return;
    }

    if (findMemberTrainer(memberId, currentTrainer)) {
        printf("This member already has trainer %s.\n", currentTrainer);
        printf("Remove the member from that trainer first.\n");
        return;
    }

    printf("Enter Trainer ID: ");
    if (scanf("%9s", trainerId) != 1)
        return;

    result = findTrainer(trainerId, &trainer);

    if (result == -1)
        return;

    if (result == 0) {
        printf("Trainer not found.\n");
        return;
    }

    if (strcmp(trainer.status, "ACTIVE") != 0) {
        printf("This trainer is not ACTIVE.\n");
        return;
    }

    if (countMembersOfTrainer(trainerId) >= MAX_MEMBERS_PER_TRAINER) {
        printf("This trainer already has %d members (maximum).\n",
               MAX_MEMBERS_PER_TRAINER);
        return;
    }

    file = fopen(LINKS_FILE, "a");

    if (file == NULL) {
        printf("Error: Cannot open trainer_members.txt for writing.\n");
        return;
    }

    if (fprintf(file, "%s %s\n", memberId, trainerId) < 0) {
        printf("Error writing the record.\n");
        fclose(file);
        return;
    }

    if (fclose(file) != 0) {
        printf("Error saving the record.\n");
        return;
    }

    printf("Member %s assigned to %s successfully!\n", memberId, trainer.fullName);
}

/* Remove a member from their trainer */
void removeMemberFromTrainer(void)
{
    FILE *file, *temp;
    char memberId[10];
    char linkMember[10];
    char linkTrainer[10];
    int found = 0;
    int failed = 0;

    printf("\n--- Remove Member from Trainer ---\n");
    printf("Enter Member ID: ");

    if (scanf("%9s", memberId) != 1)
        return;

    file = fopen(LINKS_FILE, "r");

    if (file == NULL) {
        printf("No members are assigned to any trainer yet.\n");
        return;
    }

    temp = fopen(LINKS_TEMP_FILE, "w");

    if (temp == NULL) {
        fclose(file);
        printf("Error creating temporary file.\n");
        return;
    }

    while (fscanf(file, "%9s %9s", linkMember, linkTrainer) == 2) {
        if (strcmp(linkMember, memberId) == 0) {
            found = 1;
            continue;               /* leave this record out */
        }

        if (fprintf(temp, "%s %s\n", linkMember, linkTrainer) < 0) {
            failed = 1;
            break;
        }
    }

    if (ferror(file))
        failed = 1;

    fclose(file);

    if (fclose(temp) != 0)
        failed = 1;

    if (failed || !found) {
        remove(LINKS_TEMP_FILE);

        if (failed)
            printf("Error processing records. Original file kept.\n");
        else
            printf("This member has no trainer.\n");

        return;
    }

    if (rename(LINKS_TEMP_FILE, LINKS_FILE) != 0) {
        printf("Could not replace trainer_members.txt.\n");
        printf("Your original file has been preserved.\n");
        remove(LINKS_TEMP_FILE);
        return;
    }

    printf("Member removed from trainer successfully.\n");
}

/* Show the members of one trainer */
void viewTrainerMembers(void)
{
    FILE *file;
    Trainer trainer;
    char trainerId[10];
    char linkMember[10];
    char linkTrainer[10];
    int result;
    int count = 0;

    printf("\n--- Members of a Trainer ---\n");
    printf("Enter Trainer ID: ");

    if (scanf("%9s", trainerId) != 1)
        return;

    result = findTrainer(trainerId, &trainer);

    if (result == -1)
        return;

    if (result == 0) {
        printf("Trainer not found.\n");
        return;
    }

    printf("\nTrainer: %s (%s)\n", trainer.fullName, trainer.trainerId);
    printf("Members:\n");

    file = fopen(LINKS_FILE, "r");

    if (file != NULL) {
        while (fscanf(file, "%9s %9s", linkMember, linkTrainer) == 2) {
            if (strcmp(linkTrainer, trainerId) == 0) {
                printf("  %s\n", linkMember);
                count++;
            }
        }
        fclose(file);
    }

    if (count == 0)
        printf("  No members assigned.\n");
    else
        printf("Total: %d / %d\n", count, MAX_MEMBERS_PER_TRAINER);
}

/* Print one row of the menu box (the box is 80 characters wide inside) */
static void printMenuRow(const char text[])
{
    printf("║    %-76s║\n", text);
}

/* Trainer management menu */
void trainerMenu(void)
{
    int choice;
    int ch;

    do {
        printLogoWithoutl();
        printf("║%31s%s%31s║\n", "", "TRAINER MANAGEMENT", "");
        printf("║                                                                                ║\n");
        printMenuRow("1. Register Trainer");
        printMenuRow("2. View All Trainers");
        printMenuRow("3. Search Trainer");
        printMenuRow("4. Update Trainer Details");
        printMenuRow("5. Activate/Deactivate Trainer");
        printMenuRow("6. Assign Member to Trainer");
        printMenuRow("7. Remove Member from Trainer");
        printMenuRow("8. View a Trainer's Members");
        printMenuRow("9. Return to Main Menu");
        printf("║                                                                                ║\n");
        printf("╚════════════════════════════════════════════════════════════════════════════════╝\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }
            choice = 0;
        }

        switch (choice) {
            case 1:
                registerTrainer();
                break;
            case 2:
                viewTrainers();
                break;
            case 3:
                searchTrainer();
                break;
            case 4:
                updateTrainer();
                break;
            case 5:
                changeTrainerStatus();
                break;
            case 6:
                assignMemberToTrainer();
                break;
            case 7:
                removeMemberFromTrainer();
                break;
            case 8:
                viewTrainerMembers();
                break;
            case 9:
                printf("Returning to main menu...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }

    } while (choice != 9);
}