#ifndef TRAINER_H
#define TRAINER_H

#include "models.h"  

#define TRAINER_FILE     "data/trainers.txt"
#define ASSIGNMENT_FILE  "data/trainer_assignments.txt"
#define MAX_TRAINER_MEMBERS 20  

typedef struct {
    int  memberId;
    int  trainerId;
    char sessionDay[10];       
    char sessionTime[6];       
} Assignment;

/* Menu */
void trainerMenu(void);

/* Trainer records */
void addTrainer(void);
void viewTrainers(void);
void searchTrainer(void);
void updateTrainer(void);
void deleteTrainer(void);         

void assignTrainerToMember(void);
void unassignMember(void);
void viewTrainerMembers(void);
void viewTrainerSchedule(void);

int  trainerExists(int trainerId);
int  getMemberTrainer(int memberId); 
int  countTrainerMembers(int trainerId);

#endif