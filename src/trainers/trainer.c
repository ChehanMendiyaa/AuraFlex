
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trainer.h"
#include "ui.h"
#include "system.h"

#define TRAINERS_FILE "data/trainers.txt"
#define TEMP_FILE "data/trainers_temp.txt"
#define MEMBERS_FILE "data/members.txt"
#define ASSIGNMENTS_FILE "data/trainer_members.txt"
#define ASSIGNMENTS_TEMP_FILE "data/trainer_members_temp.txt"

typedef struct {
    char trainerId[10];
    char fullName[50];
    char phone[20];
    char specialization[40];
    char status[15];
} Trainer;

/* ---------- Input helpers ---------- */

static void readLine(const char *prompt, char *buffer, size_t size)
{
    int ch;
    size_t len;

    printf("%s", prompt);

    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }

    len = strlen(buffer);

    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    } else {
        while ((ch = getchar()) != '\n' && ch != EOF) {
            /* Discard excess input. */
        }
    }
}

static int readMenuChoice(void)
{
    char input[20];
    char *end;
    long value;

    readLine("Enter choice: ", input, sizeof(input));

    value = strtol(input, &end, 10);

    while (*end == ' ' || *end == '\t') {
        end++;
    }

    if (input[0] == '\0' || *end != '\0' ||
        value < 1 || value > 9) {
        return -1;
    }

    return (int)value;
}

static void pressEnterToContinue(void)
{
    char input[8];

    printf(YELLOW"\nPress Enter to continue..."RESET);
    fgets(input, sizeof(input), stdin);
}

static void ensureDataDirectory(void)
{
    /*
       The project should already contain the data directory.
       Create the files if they do not exist.
    */
    FILE *file;

    file = fopen(TRAINERS_FILE, "a");
    if (file != NULL) {
        fclose(file);
    }

    file = fopen(ASSIGNMENTS_FILE, "a");
    if (file != NULL) {
        fclose(file);
    }
}

/* ---------- Trainer file helpers ---------- */

static int readTrainer(FILE *file, Trainer *trainer)
{
    return fscanf(file, "%9s %49s %19s %39s %14s",
                  trainer->trainerId,
                  trainer->fullName,
                  trainer->phone,
                  trainer->specialization,
                  trainer->status) == 5;
}

static int trainerIdExists(const char *trainerId)
{
    FILE *file;
    Trainer trainer;

    file = fopen(TRAINERS_FILE, "r");

    if (file == NULL) {
        return 0;
    }

    while (readTrainer(file, &trainer)) {
        if (strcmp(trainer.trainerId, trainerId) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

static int findTrainer(const char *trainerId, Trainer *result)
{
    FILE *file;
    Trainer trainer;

    file = fopen(TRAINERS_FILE, "r");

    if (file == NULL) {
        return 0;
    }

    while (readTrainer(file, &trainer)) {
        if (strcmp(trainer.trainerId, trainerId) == 0) {
            if (result != NULL) {
                *result = trainer;
            }

            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

static void displayTrainer(const Trainer *trainer)
{   
    // printf("\nTrainer ID     : %s\n", trainer->trainerId);
    // printf("Full Name      : %s\n", trainer->fullName);
    // printf("Phone          : %s\n", trainer->phone);
    // printf("Specialization : %s\n", trainer->specialization);
    // printf("Status         : %s\n", trainer->status);

     printf("║ %-10.10s ║ %-16.16s ║ %-12.12s ║ %-12.12s ║ %-8.8s ║\n",
               trainer->trainerId,
               trainer->fullName,
               trainer->phone,
               trainer->specialization,
               trainer->status);

}

static int validSingleWord(const char *value)
{
    size_t i;

    if (value[0] == '\0') {
        return 0;
    }

    for (i = 0; value[i] != '\0'; i++) {
        if (value[i] == ' ' || value[i] == '\t') {
            return 0;
        }
    }

    return 1;
}

/*
   Records use whitespace-separated fields, so each field must be
   a single word. Use underscores for names or specializations
   containing spaces, for example "John_Smith" or "Personal_Training".
*/
static int inputTrainerDetails(Trainer *trainer)
{
    readLine("Enter trainer name (use _ instead of spaces): ",
             trainer->fullName, sizeof(trainer->fullName));

    if (!validSingleWord(trainer->fullName)) {
        printf("Invalid name. Enter a single word or use underscores.\n");
        return 0;
    }

    readLine("Enter phone number: ",
             trainer->phone, sizeof(trainer->phone));

    if (!validSingleWord(trainer->phone)) {
        printf("Invalid phone number.\n");
        return 0;
    }

    readLine("Enter specialization (use _ instead of spaces): ",
             trainer->specialization, sizeof(trainer->specialization));

    if (!validSingleWord(trainer->specialization)) {
        printf("Invalid specialization.\n");
        return 0;
    }

    return 1;
}

/* ---------- Member and assignment helpers ---------- */

static int memberExists(const char *memberId)
{
    FILE *file;
    char id[20];
    char name[50];
    char phone[30];
    char email[80];
    char status[20];

    file = fopen(MEMBERS_FILE, "r");

    if (file == NULL) {
        printf("Could not open %s.\n", MEMBERS_FILE);
        return 0;
    }

    while (fscanf(file, "%19s %49s %29s %79s %19s",
                  id, name, phone, email, status) == 5) {
        if (strcmp(id, memberId) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

static int assignmentExists(const char *trainerId, const char *memberId)
{
    FILE *file;
    char storedTrainerId[20];
    char storedMemberId[20];

    file = fopen(ASSIGNMENTS_FILE, "r");

    if (file == NULL) {
        return 0;
    }

    while (fscanf(file, "%19s %19s",
                  storedTrainerId, storedMemberId) == 2) {
        if (strcmp(storedTrainerId, trainerId) == 0 &&
            strcmp(storedMemberId, memberId) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

/* ---------- Register trainer ---------- */

void registerTrainer(void)
{
    Trainer trainer;
    FILE *file;
    clearScreen();
    printf("\n========== REGISTER TRAINER ==========\n");

    readLine("Enter trainer ID: ",
             trainer.trainerId, sizeof(trainer.trainerId));

    if (!validSingleWord(trainer.trainerId)) {
        printf("Invalid trainer ID.\n");
        return;
    }

    if (trainerIdExists(trainer.trainerId)) {
        printf("A trainer with this ID already exists.\n");
        return;
    }

    if (!inputTrainerDetails(&trainer)) {
        return;
    }

    strcpy(trainer.status, "ACTIVE");

    file = fopen(TRAINERS_FILE, "a");

    if (file == NULL) {
        printf("Error opening trainer file.\n");
        return;
    }

    fprintf(file, "%s %s %s %s %s\n",
            trainer.trainerId,
            trainer.fullName,
            trainer.phone,
            trainer.specialization,
            trainer.status);

    fclose(file);

    printf("\nTrainer registered successfully!\n");
}

/* ---------- View trainers ---------- */

void viewTrainers(void)
{
    FILE *file;
    Trainer trainer;
    int count = 0;
    
    clearScreen();
    // printf("\n=============== ALL TRAINERS ===============\n");
    printf("╔════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                             ALL TRAINERS                               ║\n");
    printf("║                                                                        ║\n");
    printf("╠════════════╦══════════════════╦══════════════╦══════════════╦══════════╣\n");
    printf("║ Trainer ID ║     Full Name    ║   Phone      ║Specialization║  Status  ║\n");
    printf("╠════════════╬══════════════════╬══════════════╬══════════════╬══════════╣\n");



    file = fopen(TRAINERS_FILE, "r");

    if (file == NULL) {
        printf("Could not open trainer file.\n");
        return;
    }

    while (readTrainer(file, &trainer)) {
        displayTrainer(&trainer);
        count++;
    }
    printf("╚════════════╩══════════════════╩══════════════╩══════════════╩══════════╝\n");

    fclose(file);

    if (count == 0) {
        printf("No trainers found.\n");
    } else {
        printf("Total trainers: %d\n", count);
    }
}

/* ---------- Search trainer ---------- */

void searchTrainer(void)
{
    char trainerId[10];
    Trainer trainer;

    clearScreen();
    printf("\n=============== SEARCH TRAINER ===============\n");

    readLine("Enter trainer ID: ", trainerId, sizeof(trainerId));

    if (findTrainer(trainerId, &trainer)) {
        displayTrainer(&trainer);
    } else {
        printf("Trainer not found.\n");
    }
}

/* ---------- Update trainer details ---------- */

void updateTrainer(void)
{
    FILE *file;
    FILE *tempFile;
    Trainer trainer;
    char trainerId[10];
    int found = 0;
    int failed = 0;

    clearScreen();
    printf("\n=============== UPDATE TRAINER ===============\n");

    readLine("Enter trainer ID to update: ",
             trainerId, sizeof(trainerId));

    file = fopen(TRAINERS_FILE, "r");

    if (file == NULL) {
        printf("Could not open trainer file.\n");
        return;
    }

    tempFile = fopen(TEMP_FILE, "w");

    if (tempFile == NULL) {
        fclose(file);
        printf("Could not create temporary file.\n");
        return;
    }

    while (readTrainer(file, &trainer)) {
        if (strcmp(trainer.trainerId, trainerId) == 0) {
            found = 1;

            printf("\nCurrent trainer details:\n");
            displayTrainer(&trainer);

            printf("\nEnter updated details:\n");

            if (!inputTrainerDetails(&trainer)) {
                failed = 1;
                break;
            }
        }

        fprintf(tempFile, "%s %s %s %s %s\n",
                trainer.trainerId,
                trainer.fullName,
                trainer.phone,
                trainer.specialization,
                trainer.status);
    }

    fclose(file);
    fclose(tempFile);

    if (failed) {
        remove(TEMP_FILE);
        printf("Update cancelled because the supplied details were invalid.\n");
        return;
    }

    if (!found) {
        remove(TEMP_FILE);
        printf("Trainer not found.\n");
        return;
    }

    /*
       Windows may not replace an existing file with rename().
       Remove the old file before renaming the temporary file.
    */
    if (remove(TRAINERS_FILE) != 0) {
        printf("Could not replace trainer file. Backup data before retrying.\n");
        return;
    }

    if (rename(TEMP_FILE, TRAINERS_FILE) != 0) {
        printf("Could not save updated trainer file.\n");
        return;
    }

    printf("Trainer details updated successfully!\n");
}

/* ---------- Activate or deactivate trainer ---------- */

void changeTrainerStatus(void)
{
    FILE *file;
    FILE *tempFile;
    Trainer trainer;
    char trainerId[10];
    char choice[10];
    int found = 0;

    clearScreen();
    printf("\n========== CHANGE TRAINER STATUS ==========\n");

    readLine("Enter trainer ID: ", trainerId, sizeof(trainerId));

    file = fopen(TRAINERS_FILE, "r");

    if (file == NULL) {
        printf("Could not open trainer file.\n");
        return;
    }

    tempFile = fopen(TEMP_FILE, "w");

    if (tempFile == NULL) {
        fclose(file);
        printf("Could not create temporary file.\n");
        return;
    }

    while (readTrainer(file, &trainer)) {
        if (strcmp(trainer.trainerId, trainerId) == 0) {
            found = 1;

            printf("\nTrainer: %s\n", trainer.fullName);
            printf("Current status: %s\n", trainer.status);
            printf("1. Activate trainer\n");
            printf("2. Deactivate trainer\n");

            readLine("Select status: ", choice, sizeof(choice));

            if (strcmp(choice, "1") == 0) {
                strcpy(trainer.status, "ACTIVE");
            } else if (strcmp(choice, "2") == 0) {
                strcpy(trainer.status, "INACTIVE");
            } else {
                fclose(file);
                fclose(tempFile);
                remove(TEMP_FILE);
                printf("Invalid choice. No changes were saved.\n");
                return;
            }
        }

        fprintf(tempFile, "%s %s %s %s %s\n",
                trainer.trainerId,
                trainer.fullName,
                trainer.phone,
                trainer.specialization,
                trainer.status);
    }

    fclose(file);
    fclose(tempFile);

    if (!found) {
        remove(TEMP_FILE);
        printf("Trainer not found.\n");
        return;
    }

    if (remove(TRAINERS_FILE) != 0) {
        printf("Could not replace trainer file. Backup data before retrying.\n");
        return;
    }

    if (rename(TEMP_FILE, TRAINERS_FILE) != 0) {
        printf("Could not save trainer status.\n");
        return;
    }

    printf("Trainer status updated successfully!\n");
}

/* ---------- Assign member to trainer ---------- */

void assignMemberToTrainer(void)
{
    char trainerId[10];
    char memberId[20];
    Trainer trainer;
    FILE *file;

    clearScreen();
    printf("\n========== ASSIGN MEMBER TO TRAINER ==========\n");

    readLine("Enter trainer ID: ", trainerId, sizeof(trainerId));

    if (!findTrainer(trainerId, &trainer)) {
        printf("Trainer not found.\n");
        return;
    }

    if (strcmp(trainer.status, "ACTIVE") != 0) {
        printf("Cannot assign a member to an inactive trainer.\n");
        return;
    }

    readLine("Enter member ID: ", memberId, sizeof(memberId));

    if (!validSingleWord(memberId)) {
        printf("Invalid member ID.\n");
        return;
    }

    if (!memberExists(memberId)) {
        printf("Member not found. Check %s.\n", MEMBERS_FILE);
        return;
    }

    if (assignmentExists(trainerId, memberId)) {
        printf("This member is already assigned to this trainer.\n");
        return;
    }

    file = fopen(ASSIGNMENTS_FILE, "a");

    if (file == NULL) {
        printf("Could not open assignment file.\n");
        return;
    }

    fprintf(file, "%s %s\n", trainerId, memberId);
    fclose(file);

    printf("Member assigned to trainer successfully!\n");
}

/* ---------- Remove member from trainer ---------- */

void removeMemberFromTrainer(void)
{
    FILE *file;
    FILE *tempFile;
    char trainerId[10];
    char memberId[20];
    char storedTrainerId[20];
    char storedMemberId[20];
    int found = 0;

    clearScreen();
    printf("\n========== REMOVE MEMBER FROM TRAINER ==========\n");

    readLine("Enter trainer ID: ", trainerId, sizeof(trainerId));
    readLine("Enter member ID: ", memberId, sizeof(memberId));

    file = fopen(ASSIGNMENTS_FILE, "r");

    if (file == NULL) {
        printf("Could not open assignment file.\n");
        return;
    }

    tempFile = fopen(ASSIGNMENTS_TEMP_FILE, "w");

    if (tempFile == NULL) {
        fclose(file);
        printf("Could not create temporary assignment file.\n");
        return;
    }

    while (fscanf(file, "%19s %19s",
                  storedTrainerId, storedMemberId) == 2) {
        if (strcmp(storedTrainerId, trainerId) == 0 &&
            strcmp(storedMemberId, memberId) == 0) {
            found = 1;
            continue;
        }

        fprintf(tempFile, "%s %s\n",
                storedTrainerId, storedMemberId);
    }

    fclose(file);
    fclose(tempFile);

    if (!found) {
        remove(ASSIGNMENTS_TEMP_FILE);
        printf("Assignment not found.\n");
        return;
    }

    if (remove(ASSIGNMENTS_FILE) != 0) {
        printf("Could not replace assignment file.\n");
        return;
    }

    if (rename(ASSIGNMENTS_TEMP_FILE, ASSIGNMENTS_FILE) != 0) {
        printf("Could not save updated assignments.\n");
        return;
    }

    printf("Member removed from trainer successfully!\n");
}

/* ---------- View members assigned to trainer ---------- */

void viewTrainerMembers(void)
{
    FILE *file;
    char trainerId[10];
    char storedTrainerId[20];
    char memberId[20];
    int count = 0;

    clearScreen();
    printf("\n========== VIEW TRAINER'S MEMBERS ==========\n");

    readLine("Enter trainer ID: ", trainerId, sizeof(trainerId));

    if (!trainerIdExists(trainerId)) {
        printf("Trainer not found.\n");
        return;
    }

    file = fopen(ASSIGNMENTS_FILE, "r");

    if (file == NULL) {
        printf("Could not open assignment file.\n");
        return;
    }

    printf("\nMembers assigned to trainer %s:\n", trainerId);

    while (fscanf(file, "%19s %19s",
                  storedTrainerId, memberId) == 2) {
        if (strcmp(storedTrainerId, trainerId) == 0) {
            printf("- Member ID: %s\n", memberId);
            count++;
        }
    }

    fclose(file);

    if (count == 0) {
        printf("No members are assigned to this trainer.\n");
    } else {
        printf("Total assigned members: %d\n", count);
    }
}

/* ---------- Trainer menu ---------- */


void trainerMenu(void)
{
    int choice;

    ensureDataDirectory();

    do {
        clearScreen();
        printLogoWithoutl();

        printf("║   [1]  Register Trainer                                                        ║\n");
        printf("║   [2]  View All Trainers                                                       ║\n");
        printf("║   [3]  Search Trainer                                                          ║\n");
        printf("║   [4]  Update Trainer Details                                                  ║\n");
        printf("║   [5]  Activate/Deactivate Trainer                                             ║\n");
        printf("║   [6]  Assign Member to Trainer                                                ║\n");
        printf("║   [7]  Remove Member from Trainer                                              ║\n");
        printf("║   [8]  View a Trainer's Members                                                ║\n");
        printf("║   [9]  Return to Main Menu                                                     ║\n");
        printf("╠════════════════════════════════════════════════════════════════════════════════╝\n");
        printf("║\n");
        printf("╚════Enter choice: ");

        choice = readMenuChoice();

        if (choice == -1) {
            printf(RED "\nInvalid choice! Please enter a number from 1 to 9.\n" RESET);
            pressEnterToContinue();
            continue;
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
                printf(GREEN "\nReturning to main menu...\n" RESET);
                break;
        }

        if (choice != 9) {
            pressEnterToContinue();
        }

    } while (choice != 9);
}