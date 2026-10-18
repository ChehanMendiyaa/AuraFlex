#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trainer.h"

#define MAX_TRAINERS     100
#define MAX_ASSIGNMENTS  500
#define MEMBERS_FILE     "data/members.txt"

trainer    trList[MAX_TRAINERS];          
Assignment trAssign[MAX_ASSIGNMENTS];
char       trDays[7][4] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

// helper prototypes
int  trReadInt(char prompt[], int low, int high);
void trReadWord(char prompt[], char out[], int size);
int  trConfirm(char question[]);
int  trIsPhone(char s[]);
int  trIsEmail(char s[]);
int  trIsTime(char s[]);
int  trDayIndex(char s[]);
int  trContains(char text[], char key[]);
int  trLoadTrainers(void);
int  trSaveTrainers(int n);
int  trLoadAssignments(void);
int  trSaveAssignments(int n);
int  trMemberExists(int memberId);
int  trFindTrainer(int n, int id);
int  trCountFor(int an, int trainerId);
int  trPickTrainer(int *count);
void trPrintTrainer(trainer *t, int memberCount);

// input and valid help

int trReadInt(char prompt[], int low, int high)
{
    int value, result, c;

    while (1) {
        printf("%s", prompt);
        result = scanf("%d", &value);
        if (result == EOF)
            exit(0);                      
        while ((c = getchar()) != '\n' && c != EOF) { }   
        if (result == 1 && value >= low && value <= high)
            return value;
        printf("  Enter a number from %d to %d.\n", low, high);
    }
}

/* Reads one word (no spaces) that fits into out[size] */
void trReadWord(char prompt[], char out[], int size)
{
    char tmp[100];
    int c, len;

    while (1) {
        printf("%s", prompt);
        if (scanf("%99s", tmp) == EOF)
            exit(0);
        while ((c = getchar()) != '\n' && c != EOF) { }
        len = strlen(tmp);
        if (len < size) {
            strcpy(out, tmp);
            return;
        }
        printf("  Too long (maximum %d characters).\n", size - 1);
    }
}

int trConfirm(char question[])
{
    char tmp[100];
    trReadWord(question, tmp, 100);
    return (tmp[0] == 'y' || tmp[0] == 'Y');
}

/* Phone: exactly 10 digits */
int trIsPhone(char s[])
{
    int i;
    if (strlen(s) != 10)
        return 0;
    for (i = 0; i < 10; i++)
        if (s[i] < '0' || s[i] > '9')
            return 0;
    return 1;
}

/* Email: must have an @ and then a . somewhere after it */
int trIsEmail(char s[])
{
    int i, at = -1, dot = -1, len = strlen(s);

    for (i = 0; i < len; i++) {
        if (s[i] == '@' && at == -1)
            at = i;
        if (s[i] == '.' && at != -1)
            dot = i;
    }
    return (at > 0 && dot > at + 1 && dot < len - 1);
}

/* Time: HH:MM in 24 hour format */
int trIsTime(char s[])
{
    int hour, minute;

    if (strlen(s) != 5 || s[2] != ':')
        return 0;
    if (s[0] < '0' || s[0] > '9' || s[1] < '0' || s[1] > '9' ||
        s[3] < '0' || s[3] > '9' || s[4] < '0' || s[4] > '9')
        return 0;
    hour   = (s[0] - '0') * 10 + (s[1] - '0');
    minute = (s[3] - '0') * 10 + (s[4] - '0');
    return (hour < 24 && minute < 60);
}

/* "Mon" -> 0, "Tue" -> 1 ... Returns -1 if it is not a day */
int trDayIndex(char s[])
{
    int i;
    for (i = 0; i < 7; i++)
        if (strcmp(s, trDays[i]) == 0)
            return i;
    return -1;
}

/* Is key found inside text? */
int trContains(char text[], char key[])
{
    int i, j, tl = strlen(text), kl = strlen(key);

    for (i = 0; i + kl <= tl; i++) {
        j = 0;
        while (j < kl && text[i + j] == key[j])
            j++;
        if (j == kl)
            return 1;
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/*  File handling                                                      */
/* ------------------------------------------------------------------ */

/* Reads all trainers from the file into trList. Returns how many. */
int trLoadTrainers(void)
{
    FILE *fp;
    int n = 0;

    fp = fopen(TRAINER_FILE, "r");
    if (fp == NULL)
        return 0;                         /* no file yet = no trainers */

    while (n < MAX_TRAINERS &&
           fscanf(fp, "%d %99s %14s %99s %29s %d", &trList[n].id, trList[n].name,
                  trList[n].phone, trList[n].email, trList[n].specialization,
                  &trList[n].experienceYears) == 6) {
        n++;
    }
    fclose(fp);
    return n;
}

int trSaveTrainers(int n)
{
    FILE *fp;
    int i;

    fp = fopen(TRAINER_FILE, "w");
    if (fp == NULL) {
        printf("  Error: cannot write %s\n", TRAINER_FILE);
        return 0;
    }
    for (i = 0; i < n; i++)
        fprintf(fp, "%d %s %s %s %s %d\n", trList[i].id, trList[i].name,
                trList[i].phone, trList[i].email, trList[i].specialization,
                trList[i].experienceYears);
    fclose(fp);
    return 1;
}

int trLoadAssignments(void)
{
    FILE *fp;
    int n = 0;

    fp = fopen(ASSIGNMENT_FILE, "r");
    if (fp == NULL)
        return 0;

    while (n < MAX_ASSIGNMENTS &&
           fscanf(fp, "%d %d %9s %5s", &trAssign[n].memberId, &trAssign[n].trainerId,
                  trAssign[n].sessionDay, trAssign[n].sessionTime) == 4) {
        n++;
    }
    fclose(fp);
    return n;
}

int trSaveAssignments(int n)
{
    FILE *fp;
    int i;

    fp = fopen(ASSIGNMENT_FILE, "w");
    if (fp == NULL) {
        printf("  Error: cannot write %s\n", ASSIGNMENT_FILE);
        return 0;
    }
    for (i = 0; i < n; i++)
        fprintf(fp, "%d %d %s %s\n", trAssign[i].memberId, trAssign[i].trainerId,
                trAssign[i].sessionDay, trAssign[i].sessionTime);
    fclose(fp);
    return 1;
}

/* members.txt belongs to the Member module. Each line must START with the
   member ID. We read that number and skip the rest of the line. */
int trMemberExists(int memberId)
{
    FILE *fp;
    int id, c, found = 0;

    fp = fopen(MEMBERS_FILE, "r");
    if (fp == NULL)
        return 0;

    while (!found && fscanf(fp, "%d", &id) == 1) {
        if (id == memberId) {
            found = 1;
        } else {
            c = getc(fp);
            while (c != '\n' && c != EOF)
                c = getc(fp);
        }
    }
    fclose(fp);
    return found;
}

/* ------------------------------------------------------------------ */
/*  Searching helpers                                                  */
/* ------------------------------------------------------------------ */

/* Position of the trainer with this ID in trList, or -1 */
int trFindTrainer(int n, int id)
{
    int i;
    for (i = 0; i < n; i++)
        if (trList[i].id == id)
            return i;
    return -1;
}

/* How many of the first an assignments belong to this trainer */
int trCountFor(int an, int trainerId)
{
    int i, count = 0;
    for (i = 0; i < an; i++)
        if (trAssign[i].trainerId == trainerId)
            count++;
    return count;
}

/* Asks for a trainer ID and loads the trainers. Returns the position in
   trList (or -1). The number of trainers is stored in *count. */
int trPickTrainer(int *count)
{
    int n, id, pos;

    n = trLoadTrainers();
    if (n == 0) {
        printf("  No trainers found. Add a trainer first.\n");
        return -1;
    }
    id = trReadInt("Enter trainer ID: ", 1, 999999);
    pos = trFindTrainer(n, id);
    if (pos < 0) {
        printf("  No trainer with ID %d.\n", id);
        return -1;
    }
    *count = n;
    return pos;
}

void trPrintTrainer(trainer *t, int memberCount)
{
    printf("  ID             : %d\n", t->id);
    printf("  Name           : %s\n", t->name);
    printf("  Phone          : %s\n", t->phone);
    printf("  Email          : %s\n", t->email);
    printf("  Specialization : %s\n", t->specialization);
    printf("  Experience     : %d year(s)\n", t->experienceYears);
    printf("  Members        : %d / %d\n", memberCount, MAX_TRAINER_MEMBERS);
}

/* ------------------------------------------------------------------ */
/*  Functions for other modules                                        */
/* ------------------------------------------------------------------ */

int trainerExists(int trainerId)
{
    int n = trLoadTrainers();
    return trFindTrainer(n, trainerId) >= 0;
}

int getMemberTrainer(int memberId)
{
    int i, an = trLoadAssignments();
    for (i = 0; i < an; i++)
        if (trAssign[i].memberId == memberId)
            return trAssign[i].trainerId;
    return -1;
}

int countTrainerMembers(int trainerId)
{
    int an = trLoadAssignments();
    return trCountFor(an, trainerId);
}

/* ------------------------------------------------------------------ */
/*  Trainer records                                                    */
/* ------------------------------------------------------------------ */

void addTrainer(void)
{
    trainer t;
    int n, i;
    char tmp[100];

    n = trLoadTrainers();
    if (n >= MAX_TRAINERS) {
        printf("  The trainer list is full.\n");
        return;
    }

    t.id = 1;                             /* new ID = highest ID + 1 */
    for (i = 0; i < n; i++)
        if (trList[i].id >= t.id)
            t.id = trList[i].id + 1;

    printf("\n--- Add Trainer (ID will be %d) ---\n", t.id);
    trReadWord("Name (use _ instead of spaces): ", t.name, sizeof(t.name));

    while (1) {
        trReadWord("Phone (10 digits): ", tmp, 100);
        if (trIsPhone(tmp))
            break;
        printf("  Phone must be exactly 10 digits.\n");
    }
    strcpy(t.phone, tmp);

    while (1) {
        trReadWord("Email: ", tmp, 100);
        if (trIsEmail(tmp))
            break;
        printf("  Enter an email like name@mail.com\n");
    }
    strcpy(t.email, tmp);

    trReadWord("Specialization (e.g. Strength, Cardio, Yoga): ",
               t.specialization, sizeof(t.specialization));
    t.experienceYears = trReadInt("Years of experience (0-60): ", 0, 60);

    trList[n] = t;                        /* structure-to-structure assignment */
    n++;
    if (trSaveTrainers(n))
        printf("  Trainer added successfully. ID = %d\n", t.id);
}

void viewTrainers(void)
{
    int n, an, i, count, total = 0, busiest = 0;

    n = trLoadTrainers();
    an = trLoadAssignments();
    if (n == 0) {
        printf("  No trainers found.\n");
        return;
    }

    printf("\n%-4s %-22s %-16s %-4s %s\n", "ID", "Name", "Specialization", "Exp", "Members");
    printf("----------------------------------------------------------\n");
    for (i = 0; i < n; i++) {
        count = trCountFor(an, trList[i].id);
        total = total + count;
        if (count > trCountFor(an, trList[busiest].id))
            busiest = i;
        printf("%-4d %-22.22s %-16.16s %-4d %d/%d\n", trList[i].id, trList[i].name,
               trList[i].specialization, trList[i].experienceYears,
               count, MAX_TRAINER_MEMBERS);
    }

    printf("\nTotal trainers : %d\n", n);
    printf("Members assigned: %d (average %.1f per trainer)\n", total, (float)total / n);
    if (total > 0)
        printf("Busiest trainer : %s (%d members)\n", trList[busiest].name,
               trCountFor(an, trList[busiest].id));
}

void searchTrainer(void)
{
    int n, an, choice, id, pos, i, found = 0;
    char key[100];

    n = trLoadTrainers();
    an = trLoadAssignments();
    if (n == 0) {
        printf("  No trainers found.\n");
        return;
    }

    printf("\nSearch by: 1) ID  2) Name  3) Specialization\n");
    choice = trReadInt("Choice: ", 1, 3);

    if (choice == 1) {
        id = trReadInt("Trainer ID: ", 1, 999999);
        pos = trFindTrainer(n, id);
        if (pos >= 0) {
            printf("\n");
            trPrintTrainer(&trList[pos], trCountFor(an, id));
            found = 1;
        }
    } else {
        trReadWord("Search text: ", key, 100);
        for (i = 0; i < n; i++) {
            if ((choice == 2 && trContains(trList[i].name, key)) ||
                (choice == 3 && trContains(trList[i].specialization, key))) {
                printf("\n");
                trPrintTrainer(&trList[i], trCountFor(an, trList[i].id));
                found++;
            }
        }
    }

    if (found == 0)
        printf("  No matching trainer found.\n");
}

void updateTrainer(void)
{
    int n, pos, choice;
    char tmp[100];

    pos = trPickTrainer(&n);
    if (pos < 0)
        return;

    printf("\n");
    trPrintTrainer(&trList[pos], countTrainerMembers(trList[pos].id));
    trLoadTrainers();                     /* reload, the line above used the file too */

    printf("\nWhat do you want to change?\n");
    printf(" 1) Name  2) Phone  3) Email  4) Specialization  5) Experience\n");
    choice = trReadInt("Choice: ", 1, 5);

    if (choice == 1) {
        trReadWord("New name: ", trList[pos].name, sizeof(trList[pos].name));
    } else if (choice == 2) {
        while (1) {
            trReadWord("New phone (10 digits): ", tmp, 100);
            if (trIsPhone(tmp))
                break;
            printf("  Phone must be exactly 10 digits.\n");
        }
        strcpy(trList[pos].phone, tmp);
    } else if (choice == 3) {
        while (1) {
            trReadWord("New email: ", tmp, 100);
            if (trIsEmail(tmp))
                break;
            printf("  Enter an email like name@mail.com\n");
        }
        strcpy(trList[pos].email, tmp);
    } else if (choice == 4) {
        trReadWord("New specialization: ", trList[pos].specialization,
                   sizeof(trList[pos].specialization));
    } else {
        trList[pos].experienceYears = trReadInt("New years of experience (0-60): ", 0, 60);
    }

    if (trSaveTrainers(n))
        printf("  Trainer updated successfully.\n");
}

void deleteTrainer(void)
{
    int n, pos, count, k;

    pos = trPickTrainer(&n);
    if (pos < 0)
        return;

    count = countTrainerMembers(trList[pos].id);
    if (count > 0) {
        printf("  Cannot delete: %d member(s) are still assigned to %s.\n",
               count, trList[pos].name);
        printf("  Unassign them first.\n");
        return;
    }

    printf("  Trainer: %s (%s)\n", trList[pos].name, trList[pos].specialization);
    if (!trConfirm("Delete this trainer? (y/n): ")) {
        printf("  Cancelled.\n");
        return;
    }

    for (k = pos; k < n - 1; k++)         /* move the later trainers up */
        trList[k] = trList[k + 1];
    if (trSaveTrainers(n - 1))
        printf("  Trainer deleted.\n");
}

/* ------------------------------------------------------------------ */
/*  Assignments and schedule                                           */
/* ------------------------------------------------------------------ */

void assignTrainerToMember(void)
{
    int memberId, trainerId, n, an, pos, i;
    char day[10], time[6], tmp[100];

    n = trLoadTrainers();
    if (n == 0) {
        printf("  No trainers found. Add a trainer first.\n");
        return;
    }

    printf("\n--- Assign Member to Trainer ---\n");
    memberId = trReadInt("Member ID: ", 1, 999999);
    if (!trMemberExists(memberId)) {
        printf("  Member %d was not found in %s.\n", memberId, MEMBERS_FILE);
        return;
    }

    an = trLoadAssignments();
    for (i = 0; i < an; i++) {
        if (trAssign[i].memberId == memberId) {
            printf("  Member %d already has trainer %d. Unassign first.\n",
                   memberId, trAssign[i].trainerId);
            return;
        }
    }

    trainerId = trReadInt("Trainer ID: ", 1, 999999);
    pos = trFindTrainer(n, trainerId);
    if (pos < 0) {
        printf("  No trainer with ID %d.\n", trainerId);
        return;
    }
    if (trCountFor(an, trainerId) >= MAX_TRAINER_MEMBERS) {
        printf("  %s already has the maximum of %d members.\n",
               trList[pos].name, MAX_TRAINER_MEMBERS);
        return;
    }
    if (an >= MAX_ASSIGNMENTS) {
        printf("  The assignment list is full.\n");
        return;
    }

    while (1) {                           /* day of the session */
        trReadWord("Session day (Mon Tue Wed Thu Fri Sat Sun): ", day, 10);
        if (trDayIndex(day) >= 0)
            break;
        printf("  Type the day exactly like Mon, Tue, Wed...\n");
    }

    while (1) {                           /* time of the session */
        trReadWord("Session time (HH:MM): ", tmp, 100);
        if (!trIsTime(tmp)) {
            printf("  Use HH:MM, e.g. 07:30\n");
            continue;
        }
        strcpy(time, tmp);
        for (i = 0; i < an; i++) {        /* is this slot already taken? */
            if (trAssign[i].trainerId == trainerId &&
                strcmp(trAssign[i].sessionDay, day) == 0 &&
                strcmp(trAssign[i].sessionTime, time) == 0)
                break;
        }
        if (i == an)
            break;                        /* slot is free */
        printf("  This trainer already has a session on %s at %s.\n", day, time);
    }

    trAssign[an].memberId = memberId;
    trAssign[an].trainerId = trainerId;
    strcpy(trAssign[an].sessionDay, day);
    strcpy(trAssign[an].sessionTime, time);
    if (trSaveAssignments(an + 1))
        printf("  Member %d assigned to %s on %s at %s.\n",
               memberId, trList[pos].name, day, time);
}

void unassignMember(void)
{
    int memberId, an, i, k;

    an = trLoadAssignments();
    if (an == 0) {
        printf("  No assignments found.\n");
        return;
    }
    memberId = trReadInt("Member ID to unassign: ", 1, 999999);

    for (i = 0; i < an; i++) {
        if (trAssign[i].memberId == memberId) {
            printf("  Member %d - trainer %d, %s %s\n", memberId, trAssign[i].trainerId,
                   trAssign[i].sessionDay, trAssign[i].sessionTime);
            if (!trConfirm("Remove this assignment? (y/n): ")) {
                printf("  Cancelled.\n");
                return;
            }
            for (k = i; k < an - 1; k++)
                trAssign[k] = trAssign[k + 1];
            if (trSaveAssignments(an - 1))
                printf("  Member unassigned.\n");
            return;
        }
    }
    printf("  Member %d has no trainer assigned.\n", memberId);
}

void viewTrainerMembers(void)
{
    int n, pos, an, i, count = 0;

    pos = trPickTrainer(&n);
    if (pos < 0)
        return;
    an = trLoadAssignments();

    printf("\nTrainer: %s (%s)\n", trList[pos].name, trList[pos].specialization);
    printf("%-10s %-5s %s\n", "Member ID", "Day", "Time");
    printf("------------------------\n");
    for (i = 0; i < an; i++) {
        if (trAssign[i].trainerId == trList[pos].id) {
            printf("%-10d %-5s %s\n", trAssign[i].memberId,
                   trAssign[i].sessionDay, trAssign[i].sessionTime);
            count++;
        }
    }
    if (count == 0)
        printf("  No members assigned.\n");
    else
        printf("Members: %d / %d\n", count, MAX_TRAINER_MEMBERS);
}

void viewTrainerSchedule(void)
{
    int n, pos, an, d, i, any;

    pos = trPickTrainer(&n);
    if (pos < 0)
        return;
    an = trLoadAssignments();

    printf("\nWeekly schedule - %s\n", trList[pos].name);
    printf("------------------------------------------\n");
    for (d = 0; d < 7; d++) {             /* go through the days of the week */
        printf("%s: ", trDays[d]);
        any = 0;
        for (i = 0; i < an; i++) {
            if (trAssign[i].trainerId == trList[pos].id &&
                strcmp(trAssign[i].sessionDay, trDays[d]) == 0) {
                if (any)
                    printf(", ");
                printf("%s (Member %d)", trAssign[i].sessionTime, trAssign[i].memberId);
                any = 1;
            }
        }
        if (!any)
            printf("-");
        printf("\n");
    }
}

// menu

void trainerMenu(void)
{
    int choice;

    do {
        printf("\n========== TRAINER MANAGEMENT ==========\n");
        printf(" 1. Add trainer\n");
        printf(" 2. View trainers\n");
        printf(" 3. Search trainer\n");
        printf(" 4. Update trainer\n");
        printf(" 5. Delete trainer\n");
        printf(" 6. Assign member to trainer\n");
        printf(" 7. Unassign member\n");
        printf(" 8. View trainer's members\n");
        printf(" 9. View trainer's weekly schedule\n");
        printf(" 0. Back\n");
        choice = trReadInt("Choice: ", 0, 9);

        switch (choice) {
            case 1: addTrainer();            break;
            case 2: viewTrainers();          break;
            case 3: searchTrainer();         break;
            case 4: updateTrainer();         break;
            case 5: deleteTrainer();         break;
            case 6: assignTrainerToMember(); break;
            case 7: unassignMember();        break;
            case 8: viewTrainerMembers();    break;
            case 9: viewTrainerSchedule();   break;
        }
    } while (choice != 0);
}