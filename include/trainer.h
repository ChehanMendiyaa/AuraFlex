#ifndef Trainer_h
#define Trainer_h

#include "models.h"     

#define Trainers_FILE "data/trainers.txt"
#define Assingment_FILE "data/trainer_assignments.txt"
#define MAX_TRAINER_MEMBERS 20  

typedef struct {
    int  memberId;
    int  trainerId;
    char sessionDay[10];       
    char sessionTime[6];       
} Assignment;

#endif // Trainer_h