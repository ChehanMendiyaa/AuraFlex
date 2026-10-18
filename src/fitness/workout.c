#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "workout.h"
#include "trainer.h"          /* getMemberTrainer() and trainerExists() */

#define MAX_PLANS      100
#define MAX_EXERCISES  500
#define MEMBERS_FILE   "data/members.txt"
#define PROGRESS_FILE  "data/progress.txt"
#define GOALS_FILE     "data/goals.txt"
#define SECONDS_PER_SET 45        /* estimated time to do one set */

WorkoutPlan wkPlans[MAX_PLANS];
Exercise    wkEx[MAX_EXERCISES];
int         wkPlanCount = 0;
int         wkExCount = 0;
char        wkDays[7][4] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
char        wkGoals[3][20] = {"Muscle_Build", "Weight_Loss", "Weight_Gain"};

/* ---------- helper prototypes ---------- */
int   wkReadInt(char prompt[], int low, int high);
float wkReadFloat(char prompt[], float low, float high);
void  wkReadWord(char prompt[], char out[], int size);
void  wkReadDay(char out[]);
void  wkReadGoal(char out[]);
void  wkReadDate(char out[]);
int   wkConfirm(char question[]);
int   wkIsDate(char s[]);
int   wkIsDay(char s[]);
void  wkLoadAll(void);
void  wkSavePlans(void);
void  wkSaveExercises(void);
int   wkMemberExists(int memberId);
int   wkFindPlan(int id);
int   wkFindExercise(int id);
int   wkPickPlan(void);
int   wkCountExercises(int planId);
void  wkShowMemberLines(char fileName[], int memberId);
void  wkRemovePlanExercises(int planId);

/* ------------------------------------------------------------------ */
/*  Input and validation helpers                                       */
/* ------------------------------------------------------------------ */

int wkReadInt(char prompt[], int low, int high)
{
    int value, result, c;

    while (1) {
        printf("%s", prompt);
        result = scanf("%d", &value);
        if (result == EOF)
            exit(0);                      /* input has ended */
        while ((c = getchar()) != '\n' && c != EOF) { }   /* clear the line */
        if (result == 1 && value >= low && value <= high)
            return value;
        printf("  Enter a number from %d to %d.\n", low, high);
    }
}

float wkReadFloat(char prompt[], float low, float high)
{
    float value;
    int result, c;

    while (1) {
        printf("%s", prompt);
        result = scanf("%f", &value);
        if (result == EOF)
            exit(0);
        while ((c = getchar()) != '\n' && c != EOF) { }
        if (result == 1 && value >= low && value <= high)
            return value;
        printf("  Enter a number from %.0f to %.0f.\n", low, high);
    }
}

/* Reads one word (no spaces) that fits into out[size] */
void wkReadWord(char prompt[], char out[], int size)
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

int wkConfirm(char question[])
{
    char tmp[100];
    wkReadWord(question, tmp, 100);
    return (tmp[0] == 'y' || tmp[0] == 'Y');
}

/* Date written as YYYY-MM-DD */
int wkIsDate(char s[])
{
    int i, month, day;

    if (strlen(s) != 10 || s[4] != '-' || s[7] != '-')
        return 0;
    for (i = 0; i < 10; i++) {
        if (i == 4 || i == 7)
            continue;
        if (s[i] < '0' || s[i] > '9')
            return 0;
    }
    month = (s[5] - '0') * 10 + (s[6] - '0');
    day   = (s[8] - '0') * 10 + (s[9] - '0');
    return (month >= 1 && month <= 12 && day >= 1 && day <= 31);
}

int wkIsDay(char s[])
{
    int i;
    for (i = 0; i < 7; i++)
        if (strcmp(s, wkDays[i]) == 0)
            return 1;
    return 0;
}

void wkReadDay(char out[])
{
    char tmp[100];

    while (1) {
        wkReadWord("Day (Mon Tue Wed Thu Fri Sat Sun): ", tmp, 100);
        if (wkIsDay(tmp))
            break;
        printf("  Type the day exactly like Mon, Tue, Wed...\n");
    }
    strcpy(out, tmp);
}

void wkReadGoal(char out[])
{
    int choice;

    printf("Goal: 1) Muscle_Build  2) Weight_Loss  3) Weight_Gain\n");
    choice = wkReadInt("Choice: ", 1, 3);
    strcpy(out, wkGoals[choice - 1]);
}

void wkReadDate(char out[])
{
    char tmp[100];

    while (1) {
        wkReadWord("Date created (YYYY-MM-DD): ", tmp, 100);
        if (wkIsDate(tmp))
            break;
        printf("  Use the format YYYY-MM-DD, e.g. 2026-10-18\n");
    }
    strcpy(out, tmp);
}

/* ------------------------------------------------------------------ */
/*  File handling                                                      */
/* ------------------------------------------------------------------ */

/* Reads both files into wkPlans and wkEx */
void wkLoadAll(void)
{
    FILE *fp;

    wkPlanCount = 0;
    fp = fopen(WORKOUT_FILE, "r");
    if (fp != NULL) {
        while (wkPlanCount < MAX_PLANS &&
               fscanf(fp, "%d %d %d %39s %29s %9s %10s",
                      &wkPlans[wkPlanCount].planId, &wkPlans[wkPlanCount].memberId,
                      &wkPlans[wkPlanCount].trainerId, wkPlans[wkPlanCount].planName,
                      wkPlans[wkPlanCount].goal, wkPlans[wkPlanCount].dayOfWeek,
                      wkPlans[wkPlanCount].createdDate) == 7) {
            wkPlans[wkPlanCount].active = 1;
            wkPlanCount++;
        }
        fclose(fp);
    }

    wkExCount = 0;
    fp = fopen(EXERCISE_FILE, "r");
    if (fp != NULL) {
        while (wkExCount < MAX_EXERCISES &&
               fscanf(fp, "%d %d %39s %d %d %f %d",
                      &wkEx[wkExCount].exerciseId, &wkEx[wkExCount].planId,
                      wkEx[wkExCount].name, &wkEx[wkExCount].sets,
                      &wkEx[wkExCount].reps, &wkEx[wkExCount].weightKg,
                      &wkEx[wkExCount].restSeconds) == 7) {
            wkEx[wkExCount].active = 1;
            wkExCount++;
        }
        fclose(fp);
    }
}

void wkSavePlans(void)
{
    FILE *fp;
    int i;

    fp = fopen(WORKOUT_FILE, "w");
    if (fp == NULL) {
        printf("  Error: cannot write %s\n", WORKOUT_FILE);
        return;
    }
    for (i = 0; i < wkPlanCount; i++)
        fprintf(fp, "%d %d %d %s %s %s %s\n", wkPlans[i].planId, wkPlans[i].memberId,
                wkPlans[i].trainerId, wkPlans[i].planName, wkPlans[i].goal,
                wkPlans[i].dayOfWeek, wkPlans[i].createdDate);
    fclose(fp);
}

void wkSaveExercises(void)
{
    FILE *fp;
    int i;

    fp = fopen(EXERCISE_FILE, "w");
    if (fp == NULL) {
        printf("  Error: cannot write %s\n", EXERCISE_FILE);
        return;
    }
    for (i = 0; i < wkExCount; i++)
        fprintf(fp, "%d %d %s %d %d %.1f %d\n", wkEx[i].exerciseId, wkEx[i].planId,
                wkEx[i].name, wkEx[i].sets, wkEx[i].reps, wkEx[i].weightKg,
                wkEx[i].restSeconds);
    fclose(fp);
}

/* members.txt belongs to the Member module. Each line must START with the
   member ID. We read that number and skip the rest of the line. */
int wkMemberExists(int memberId)
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

/* Prints every line of a file that starts with this member ID */
void wkShowMemberLines(char fileName[], int memberId)
{
    FILE *fp;
    char rest[200];
    int id, count = 0;

    fp = fopen(fileName, "r");
    if (fp == NULL) {
        printf("  (no data yet)\n");
        return;
    }
    while (fscanf(fp, "%d", &id) == 1) {
        if (fgets(rest, 200, fp) == NULL)
            rest[0] = '\0';
        if (id == memberId) {
            printf("  %d%s", id, rest);
            if (rest[0] == '\0' || rest[strlen(rest) - 1] != '\n')
                printf("\n");
            count++;
        }
    }
    fclose(fp);
    if (count == 0)
        printf("  (no records for this member)\n");
}

/* ------------------------------------------------------------------ */
/*  Searching helpers                                                  */
/* ------------------------------------------------------------------ */

int wkFindPlan(int id)
{
    int i;
    for (i = 0; i < wkPlanCount; i++)
        if (wkPlans[i].planId == id)
            return i;
    return -1;
}

int wkFindExercise(int id)
{
    int i;
    for (i = 0; i < wkExCount; i++)
        if (wkEx[i].exerciseId == id)
            return i;
    return -1;
}

int wkCountExercises(int planId)
{
    int i, count = 0;
    for (i = 0; i < wkExCount; i++)
        if (wkEx[i].planId == planId)
            count++;
    return count;
}

/* Loads the files, asks for a plan ID and returns its position (or -1) */
int wkPickPlan(void)
{
    int id, pos;

    wkLoadAll();
    if (wkPlanCount == 0) {
        printf("  No workout plans found. Create a plan first.\n");
        return -1;
    }
    id = wkReadInt("Enter plan ID: ", 1, 999999);
    pos = wkFindPlan(id);
    if (pos < 0)
        printf("  No plan with ID %d.\n", id);
    return pos;
}

/* Removes all exercises of one plan from wkEx */
void wkRemovePlanExercises(int planId)
{
    int i, k = 0;

    for (i = 0; i < wkExCount; i++) {
        if (wkEx[i].planId != planId) {
            wkEx[k] = wkEx[i];
            k++;
        }
    }
    wkExCount = k;
}

/* ------------------------------------------------------------------ */
/*  Workout plans                                                      */
/* ------------------------------------------------------------------ */

void createWorkoutPlan(void)
{
    WorkoutPlan p;
    int i;

    wkLoadAll();
    if (wkPlanCount >= MAX_PLANS) {
        printf("  The plan list is full.\n");
        return;
    }

    printf("\n--- Create Workout Plan ---\n");
    p.memberId = wkReadInt("Member ID: ", 1, 999999);
    if (!wkMemberExists(p.memberId)) {
        printf("  Member %d was not found in %s.\n", p.memberId, MEMBERS_FILE);
        return;
    }

    p.trainerId = getMemberTrainer(p.memberId);   /* from the trainer module */
    if (p.trainerId < 0) {
        printf("  This member has no trainer yet.\n");
        printf("  Assign a trainer first (Trainer Management).\n");
        return;
    }
    printf("  Trainer for this member: %d\n", p.trainerId);

    wkReadWord("Plan name (use _ instead of spaces): ", p.planName, sizeof(p.planName));
    wkReadGoal(p.goal);
    wkReadDay(p.dayOfWeek);
    wkReadDate(p.createdDate);
    p.active = 1;

    p.planId = 1;                         /* new ID = highest ID + 1 */
    for (i = 0; i < wkPlanCount; i++)
        if (wkPlans[i].planId >= p.planId)
            p.planId = wkPlans[i].planId + 1;

    wkPlans[wkPlanCount] = p;             /* structure-to-structure assignment */
    wkPlanCount++;
    wkSavePlans();
    printf("  Plan created. Plan ID = %d (now add exercises to it)\n", p.planId);
}

void viewWorkoutPlans(void)
{
    int choice, id = 0, i, found = 0;

    wkLoadAll();
    if (wkPlanCount == 0) {
        printf("  No workout plans found.\n");
        return;
    }

    printf("\nShow plans: 1) For one member  2) For one trainer  3) All\n");
    choice = wkReadInt("Choice: ", 1, 3);
    if (choice == 1)
        id = wkReadInt("Member ID: ", 1, 999999);
    else if (choice == 2)
        id = wkReadInt("Trainer ID: ", 1, 999999);

    printf("\n%-5s %-7s %-8s %-18s %-13s %-4s %-11s %s\n", "Plan", "Member", "Trainer",
           "Name", "Goal", "Day", "Created", "Exercises");
    printf("------------------------------------------------------------------------------------\n");
    for (i = 0; i < wkPlanCount; i++) {
        if ((choice == 1 && wkPlans[i].memberId != id) ||
            (choice == 2 && wkPlans[i].trainerId != id))
            continue;
        printf("%-5d %-7d %-8d %-18.18s %-13s %-4s %-11s %d\n", wkPlans[i].planId,
               wkPlans[i].memberId, wkPlans[i].trainerId, wkPlans[i].planName,
               wkPlans[i].goal, wkPlans[i].dayOfWeek, wkPlans[i].createdDate,
               wkCountExercises(wkPlans[i].planId));
        found++;
    }
    if (found == 0)
        printf("  No plans found.\n");
    else
        printf("Plans shown: %d\n", found);
}

void updateWorkoutPlan(void)
{
    int pos, choice;

    pos = wkPickPlan();
    if (pos < 0)
        return;

    printf("  Plan: %s | %s | %s | %s\n", wkPlans[pos].planName, wkPlans[pos].goal,
           wkPlans[pos].dayOfWeek, wkPlans[pos].createdDate);
    printf("What do you want to change?\n");
    printf(" 1) Name  2) Goal  3) Day  4) Date\n");
    choice = wkReadInt("Choice: ", 1, 4);

    if (choice == 1)
        wkReadWord("New plan name: ", wkPlans[pos].planName, sizeof(wkPlans[pos].planName));
    else if (choice == 2)
        wkReadGoal(wkPlans[pos].goal);
    else if (choice == 3)
        wkReadDay(wkPlans[pos].dayOfWeek);
    else
        wkReadDate(wkPlans[pos].createdDate);

    wkSavePlans();
    printf("  Plan updated.\n");
}

void deleteWorkoutPlan(void)
{
    int pos, k, count;

    pos = wkPickPlan();
    if (pos < 0)
        return;

    count = wkCountExercises(wkPlans[pos].planId);
    printf("  Plan: %s (%d exercise(s) will also be deleted)\n",
           wkPlans[pos].planName, count);
    if (!wkConfirm("Delete this plan? (y/n): ")) {
        printf("  Cancelled.\n");
        return;
    }

    wkRemovePlanExercises(wkPlans[pos].planId);
    for (k = pos; k < wkPlanCount - 1; k++)   /* move the later plans up */
        wkPlans[k] = wkPlans[k + 1];
    wkPlanCount--;

    wkSavePlans();
    wkSaveExercises();
    printf("  Plan deleted.\n");
}

/* ------------------------------------------------------------------ */
/*  Exercises                                                          */
/* ------------------------------------------------------------------ */

void addExercise(void)
{
    Exercise e;
    int pos, i;

    pos = wkPickPlan();
    if (pos < 0)
        return;
    if (wkExCount >= MAX_EXERCISES) {
        printf("  The exercise list is full.\n");
        return;
    }

    printf("  Adding to plan: %s\n", wkPlans[pos].planName);
    wkReadWord("Exercise name (use _ for spaces, e.g. Bench_Press): ", e.name, sizeof(e.name));
    e.sets        = wkReadInt("Sets (1-20): ", 1, 20);
    e.reps        = wkReadInt("Reps per set (1-100): ", 1, 100);
    e.weightKg    = wkReadFloat("Weight in kg (0 for body weight, max 500): ", 0, 500);
    e.restSeconds = wkReadInt("Rest between sets in seconds (0-600): ", 0, 600);
    e.planId = wkPlans[pos].planId;
    e.active = 1;

    e.exerciseId = 1;                     /* new ID = highest ID + 1 */
    for (i = 0; i < wkExCount; i++)
        if (wkEx[i].exerciseId >= e.exerciseId)
            e.exerciseId = wkEx[i].exerciseId + 1;

    wkEx[wkExCount] = e;
    wkExCount++;
    wkSaveExercises();
    printf("  Exercise added. Exercise ID = %d\n", e.exerciseId);
}

void viewPlanExercises(void)
{
    int pos, i, count = 0;

    pos = wkPickPlan();
    if (pos < 0)
        return;

    printf("\nPlan %d: %s (%s, %s)\n", wkPlans[pos].planId, wkPlans[pos].planName,
           wkPlans[pos].goal, wkPlans[pos].dayOfWeek);
    printf("%-4s %-22s %-5s %-5s %-9s %s\n", "ID", "Exercise", "Sets", "Reps", "Weight", "Rest");
    printf("------------------------------------------------------\n");
    for (i = 0; i < wkExCount; i++) {
        if (wkEx[i].planId == wkPlans[pos].planId) {
            printf("%-4d %-22.22s %-5d %-5d %-6.1fkg  %ds\n", wkEx[i].exerciseId,
                   wkEx[i].name, wkEx[i].sets, wkEx[i].reps, wkEx[i].weightKg,
                   wkEx[i].restSeconds);
            count++;
        }
    }
    if (count == 0)
        printf("  No exercises in this plan yet.\n");
}

void updateExercise(void)
{
    int id, pos, choice;

    wkLoadAll();
    if (wkExCount == 0) {
        printf("  No exercises found.\n");
        return;
    }
    id = wkReadInt("Enter exercise ID: ", 1, 999999);
    pos = wkFindExercise(id);
    if (pos < 0) {
        printf("  No exercise with ID %d.\n", id);
        return;
    }

    printf("  %s: %d sets x %d reps, %.1f kg, rest %d s\n", wkEx[pos].name, wkEx[pos].sets,
           wkEx[pos].reps, wkEx[pos].weightKg, wkEx[pos].restSeconds);
    printf("What do you want to change?\n");
    printf(" 1) Name  2) Sets  3) Reps  4) Weight  5) Rest time\n");
    choice = wkReadInt("Choice: ", 1, 5);

    if (choice == 1)
        wkReadWord("New name: ", wkEx[pos].name, sizeof(wkEx[pos].name));
    else if (choice == 2)
        wkEx[pos].sets = wkReadInt("New sets (1-20): ", 1, 20);
    else if (choice == 3)
        wkEx[pos].reps = wkReadInt("New reps (1-100): ", 1, 100);
    else if (choice == 4)
        wkEx[pos].weightKg = wkReadFloat("New weight in kg (0-500): ", 0, 500);
    else
        wkEx[pos].restSeconds = wkReadInt("New rest in seconds (0-600): ", 0, 600);

    wkSaveExercises();
    printf("  Exercise updated.\n");
}

void deleteExercise(void)
{
    int id, pos, k;

    wkLoadAll();
    if (wkExCount == 0) {
        printf("  No exercises found.\n");
        return;
    }
    id = wkReadInt("Enter exercise ID: ", 1, 999999);
    pos = wkFindExercise(id);
    if (pos < 0) {
        printf("  No exercise with ID %d.\n", id);
        return;
    }
    printf("  Exercise: %s\n", wkEx[pos].name);
    if (!wkConfirm("Delete this exercise? (y/n): ")) {
        printf("  Cancelled.\n");
        return;
    }

    for (k = pos; k < wkExCount - 1; k++)
        wkEx[k] = wkEx[k + 1];
    wkExCount--;
    wkSaveExercises();
    printf("  Exercise deleted.\n");
}

/* Totals for one plan: sets, volume (sets x reps x kg) and estimated time */
void showPlanSummary(void)
{
    int pos, i, count = 0, totalSets = 0, totalSeconds = 0, heaviest = -1;
    float volume = 0;

    pos = wkPickPlan();
    if (pos < 0)
        return;

    for (i = 0; i < wkExCount; i++) {
        if (wkEx[i].planId != wkPlans[pos].planId)
            continue;
        count++;
        totalSets = totalSets + wkEx[i].sets;
        volume = volume + wkEx[i].sets * wkEx[i].reps * wkEx[i].weightKg;
        totalSeconds = totalSeconds + wkEx[i].sets * (SECONDS_PER_SET + wkEx[i].restSeconds);
        if (heaviest == -1 || wkEx[i].weightKg > wkEx[heaviest].weightKg)
            heaviest = i;
    }

    printf("\n--- Summary of plan %d: %s ---\n", wkPlans[pos].planId, wkPlans[pos].planName);
    if (count == 0) {
        printf("  No exercises in this plan yet.\n");
        return;
    }
    printf("  Exercises          : %d\n", count);
    printf("  Total sets         : %d\n", totalSets);
    printf("  Total volume       : %.1f kg (sets x reps x weight)\n", volume);
    printf("  Estimated duration : %d minutes\n", (totalSeconds + 59) / 60);
    printf("  Heaviest exercise  : %s (%.1f kg)\n", wkEx[heaviest].name, wkEx[heaviest].weightKg);
}

/* ------------------------------------------------------------------ */
/*  Trainer view of a member's fitness data                            */
/* ------------------------------------------------------------------ */

void viewMemberFitnessProfile(void)
{
    int memberId, trainerId, i, count = 0;

    memberId = wkReadInt("Member ID: ", 1, 999999);
    if (!wkMemberExists(memberId)) {
        printf("  Member %d was not found in %s.\n", memberId, MEMBERS_FILE);
        return;
    }
    wkLoadAll();
    trainerId = getMemberTrainer(memberId);

    printf("\n===== Fitness profile: member %d =====\n", memberId);
    if (trainerId < 0)
        printf("Trainer: not assigned\n");
    else
        printf("Trainer: %d\n", trainerId);

    printf("\nWorkout plans:\n");
    for (i = 0; i < wkPlanCount; i++) {
        if (wkPlans[i].memberId == memberId) {
            printf("  Plan %d: %s (%s, %s) - %d exercise(s)\n", wkPlans[i].planId,
                   wkPlans[i].planName, wkPlans[i].goal, wkPlans[i].dayOfWeek,
                   wkCountExercises(wkPlans[i].planId));
            count++;
        }
    }
    if (count == 0)
        printf("  (no plans yet)\n");

    printf("\nProgress records:\n");
    wkShowMemberLines(PROGRESS_FILE, memberId);
    printf("\nGoals:\n");
    wkShowMemberLines(GOALS_FILE, memberId);
}

/* ------------------------------------------------------------------ */
/*  Menu                                                               */
/* ------------------------------------------------------------------ */

void workoutMenu(void)
{
    int choice;

    do {
        printf("\n========== WORKOUT PLANS ==========\n");
        printf(" 1. Create workout plan\n");
        printf(" 2. View workout plans\n");
        printf(" 3. Update workout plan\n");
        printf(" 4. Delete workout plan\n");
        printf(" 5. Add exercise to a plan\n");
        printf(" 6. View a plan's exercises\n");
        printf(" 7. Update exercise\n");
        printf(" 8. Delete exercise\n");
        printf(" 9. Plan summary\n");
        printf("10. Member fitness profile (for trainers)\n");
        printf(" 0. Back\n");
        choice = wkReadInt("Choice: ", 0, 10);

        switch (choice) {
            case 1:  createWorkoutPlan();        break;
            case 2:  viewWorkoutPlans();         break;
            case 3:  updateWorkoutPlan();        break;
            case 4:  deleteWorkoutPlan();        break;
            case 5:  addExercise();              break;
            case 6:  viewPlanExercises();        break;
            case 7:  updateExercise();           break;
            case 8:  deleteExercise();           break;
            case 9:  showPlanSummary();          break;
            case 10: viewMemberFitnessProfile(); break;
        }
    } while (choice != 0);
}