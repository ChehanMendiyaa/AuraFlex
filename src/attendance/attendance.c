
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "attendance.h"
#include "system.h"
#include "ui.h"

#define ATTENDANCE_FILE "data/attendance.txt"
#define MEMBERS_FILE "data/members.txt"
#define TEMP_FILE "data/attendance_temp.txt"

typedef struct {
    char attendanceId[20];
    char memberId[10];
    char date[11];
    char checkIn[10];
    char checkOut[10];
} Attendance;

/* Wait for Enter, handling the newline left by scanf */
static void pressEnterToContinue(void)
{
    int ch;

    /* Clear any remaining characters from the previous input */
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }

    printf("\nPress Enter to return to the previous menu...");

    /* Wait for the user to press Enter */
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

/* Get today's date in YYYY-MM-DD format */
static void getToday(char date[])
{
    time_t now = time(NULL);
    struct tm *current = localtime(&now);

    if (current == NULL) {
        strcpy(date, "0000-00-00");
        return;
    }

    strftime(date, 11, "%Y-%m-%d", current);
}

/* Get current time in HH:MM format */
static void getCurrentTime(char timeText[])
{
    time_t now = time(NULL);
    struct tm *current = localtime(&now);

    if (current == NULL) {
        strcpy(timeText, "00:00");
        return;
    }

    strftime(timeText, 10, "%H:%M", current);
}

/* Check whether a member exists and is active */
static int memberExists(const char *memberId)
{
    FILE *file = fopen(MEMBERS_FILE, "r");
    char id[10], name[50], phone[20], email[60], status[15];

    if (file == NULL) {
        printf("Error: Cannot open members.txt.\n");
        return -1;
    }

    while (fscanf(file, "%9s %49s %19s %59s %14s",
                  id, name, phone, email, status) == 5) {
        if (strcmp(id, memberId) == 0) {
            fclose(file);
            return strcmp(status, "ACTIVE") == 0 ? 1 : 0;
        }
    }

    fclose(file);
    return 0;
}

/* Read one attendance record */
static int readAttendance(FILE *file, Attendance *record)
{
    return fscanf(file, "%19s %9s %10s %9s %9s",
                  record->attendanceId,
                  record->memberId,
                  record->date,
                  record->checkIn,
                  record->checkOut) == 5;
}

/* Generate the next attendance ID */
static void generateAttendanceId(char id[])
{
    FILE *file = fopen(ATTENDANCE_FILE, "r");
    Attendance record;
    int highest = 0;
    int number;

    if (file != NULL) {
        while (readAttendance(file, &record)) {
            if (sscanf(record.attendanceId, "A%d", &number) == 1) {
                if (number > highest) {
                    highest = number;
                }
            }
        }

        fclose(file);
    }

    sprintf(id, "A%03d", highest + 1);
}

/* Check whether a member is still checked in today */
static int findTodayAttendance(const char *memberId,
                               Attendance *found)
{
    FILE *file = fopen(ATTENDANCE_FILE, "r");
    Attendance record;
    char today[11];

    if (file == NULL) {
        return 0;
    }

    getToday(today);

    while (readAttendance(file, &record)) {
        if (strcmp(record.memberId, memberId) == 0 &&
            strcmp(record.date, today) == 0 &&
            strcmp(record.checkOut, "-") == 0) {

            *found = record;
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

/* Record member check-in */
void recordCheckIn(void)
{
    FILE *file;
    Attendance record;
    char memberId[10];
    int result;

    clearScreen();
    printf("\n--- Member Check-In ---\n");
    printf("Enter Member ID: ");

    if (scanf("%9s", memberId) != 1) {
        return;
    }

    result = memberExists(memberId);

    if (result == -1) {
        return;
    }

    if (result == 0) {
        printf("Member not found or member is not active.\n");
        return;
    }

    if (findTodayAttendance(memberId, &record)) {
        printf("This member is already checked in today.\n");
        return;
    }

    file = fopen(ATTENDANCE_FILE, "a");

    if (file == NULL) {
        printf("Error: Cannot open attendance.txt.\n");
        return;
    }

    generateAttendanceId(record.attendanceId);
    strcpy(record.memberId, memberId);
    getToday(record.date);
    getCurrentTime(record.checkIn);
    strcpy(record.checkOut, "-");

    fprintf(file, "%s %s %s %s %s\n",
            record.attendanceId,
            record.memberId,
            record.date,
            record.checkIn,
            record.checkOut);

    fclose(file);

    printf("\nCheck-in recorded successfully!\n");
    printf("Attendance ID: %s\n", record.attendanceId);
    printf("Date: %s\n", record.date);
    printf("Check-in Time: %s\n", record.checkIn);
}

/* Record member check-out */
void recordCheckOut(void)
{
    FILE *file, *temp;
    Attendance record;
    char memberId[10];
    char today[11];
    char currentTime[10];
    int found = 0;
    int result;
    int writeError = 0;

    clearScreen();
    printf("\n--- Member Check-Out ---\n");
    printf("Enter Member ID: ");

    if (scanf("%9s", memberId) != 1) {
        return;
    }

    result = memberExists(memberId);

    if (result == -1) {
        return;
    }

    if (result == 0) {
        printf("Member not found or member is not active.\n");
        return;
    }

    file = fopen(ATTENDANCE_FILE, "r");

    if (file == NULL) {
        printf("No attendance records found.\n");
        return;
    }

    temp = fopen(TEMP_FILE, "w");

    if (temp == NULL) {
        fclose(file);
        printf("Error creating temporary file.\n");
        return;
    }

    getToday(today);
    getCurrentTime(currentTime);

    while (readAttendance(file, &record)) {
        if (!found &&
            strcmp(record.memberId, memberId) == 0 &&
            strcmp(record.date, today) == 0 &&
            strcmp(record.checkOut, "-") == 0) {

            strcpy(record.checkOut, currentTime);
            found = 1;
        }

        if (fprintf(temp, "%s %s %s %s %s\n",
                    record.attendanceId,
                    record.memberId,
                    record.date,
                    record.checkIn,
                    record.checkOut) < 0) {
            writeError = 1;
            break;
        }
    }

    if (ferror(file)) {
        writeError = 1;
    }

    fclose(file);

    if (fclose(temp) != 0) {
        writeError = 1;
    }

    if (writeError || !found) {
        remove(TEMP_FILE);

        if (writeError) {
            printf("Error writing attendance records.\n");
        } else {
            printf("No open check-in found for this member today.\n");
        }

        return;
    }

    /*
       Replace the original file after the temporary file
       has been written successfully.
    */
    if (rename(TEMP_FILE, ATTENDANCE_FILE) != 0) {
        printf("Could not replace attendance file.\n");
        printf("Your original attendance file has been kept.\n");
        remove(TEMP_FILE);
        return;
    }

    printf("\nCheck-out recorded successfully!\n");
    printf("Member ID: %s\n", memberId);
    printf("Check-out Time: %s\n", currentTime);
}

/* Search attendance records by member ID */
void searchAttendance(void)
{
    FILE *file;
    Attendance record;
    char memberId[10];
    int found = 0;

    clearScreen();
    printf("\n--- Search Attendance ---\n");
    printf("Enter Member ID: ");

    if (scanf("%9s", memberId) != 1) {
        return;
    }

    file = fopen(ATTENDANCE_FILE, "r");

    if (file == NULL) {
        printf("No attendance records found.\n");
        return;
    }

    printf("\nID       Member    Date         Check-In  Check-Out\n");
    printf("---------------------------------------------------\n");

    while (readAttendance(file, &record)) {
        if (strcmp(record.memberId, memberId) == 0) {
            printf("%-8s %-9s %-11s %-9s %-9s\n",
                   record.attendanceId,
                   record.memberId,
                   record.date,
                   record.checkIn,
                   record.checkOut);

            found = 1;
        }
    }

    fclose(file);

    if (!found) {
        printf("No records found for member %s.\n", memberId);
    }
}

/* View all attendance records */
void viewAttendanceHistory(void)
{
    FILE *file;
    Attendance record;
    int found = 0;

    clearScreen();

    printf("╔═════════════════════════════════════════════════════════════════╗\n");
    printf("║                       Attendance History                        ║\n");
    printf("║                                                                 ║\n");
    printf("╠════════════╦══════════╦══════════════╦══════════════╦═══════════╣\n");
    printf("║   ID       ║ MEMBER   ║     Date     ║   Check-In   ║Check-Out  ║\n");
    printf("╠════════════╬══════════╬══════════════╬══════════════╬═══════════╣\n");

    file = fopen(ATTENDANCE_FILE, "r");

    if (file == NULL) {
        printf("No attendance records found.\n");
        pressEnterToContinue();
        return;
    }

    while (readAttendance(file, &record)) {
        printf("║ %-10.10s ║ %-8.8s ║ %-12.12s ║ %-12.12s ║  %-8.8s ║\n",
               record.attendanceId,
               record.memberId,
               record.date,
               record.checkIn,
               record.checkOut);

        found = 1;
    }
    printf("╚════════════╩══════════╩══════════════╩══════════════╩═══════════╝\n");
    fclose(file);

    if (!found) {
        printf("Attendance file is empty.\n");
    }

    pressEnterToContinue();
}

/* Display today's attendance summary */
void dailyAttendanceSummary(void)
{
    FILE *file;
    Attendance record;
    char today[11];
    int checkIns = 0;
    int checkedOut = 0;
    int stillInside = 0;

    clearScreen();
    getToday(today);

    printf("\n--- Daily Attendance Summary ---\n");
    printf("Date: %s\n\n", today);

    file = fopen(ATTENDANCE_FILE, "r");

    if (file == NULL) {
        printf("No attendance records found.\n");
        pressEnterToContinue();
        return;
    }

    while (readAttendance(file, &record)) {
        if (strcmp(record.date, today) == 0) {
            checkIns++;

            if (strcmp(record.checkOut, "-") == 0) {
                stillInside++;
            } else {
                checkedOut++;
            }
        }
    }

    fclose(file);

    printf("Total Check-Ins: %d\n", checkIns);
    printf("Checked Out: %d\n", checkedOut);
    printf("Currently Checked In: %d\n", stillInside);

    pressEnterToContinue();
}

/* Attendance menu */
void attendanceMenu(void)
{
    int choice;

    do {
        clearScreen();
        printLogoWithoutl();

        printf("║   [1]  Record Check-In                                                         ║\n");
        printf("║   [2]  Record Check-Out                                                        ║\n");
        printf("║   [3]  Search Attendance by Member ID                                          ║\n");
        printf("║   [4]  View Attendance History                                                 ║\n");
        printf("║   [5]  Daily Attendance Summary                                                ║\n");
        printf("║   [6]  Return to Dashboard                                                     ║\n");
        printf("╚════════════════════════════════════════════════════════════════════════════════╝\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            int ch;

            while ((ch = getchar()) != '\n' && ch != EOF) {
            }

            printf("Invalid input. Enter a number.\n");
            pressEnterToContinue();
            continue;
        }

        switch (choice) {
            case 1:
                recordCheckIn();
                break;

            case 2:
                recordCheckOut();
                break;

            case 3:
                searchAttendance();
                break;

            case 4:
                viewAttendanceHistory();
                break;

            case 5:
                dailyAttendanceSummary();
                break;

            case 6:
                printf("Returning to main menu...\n");
                break;

            default:
                printf("Invalid choice. Please try again.\n");
                pressEnterToContinue();
        }

    } while (choice != 6);
}
