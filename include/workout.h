#ifndef WORKOUT_H
#define WORKOUT_H

#define WORKOUT_FILE   "data/workout_plans.txt"
#define EXERCISE_FILE  "data/exercises.txt"

typedef struct {
    int  planId;
    int  memberId;
    int  trainerId;
    char planName[40];         // like Fat Loss 
    char goal[30];             // muscle build, weight loss , weight gain 
    char dayOfWeek[10];        // day this plan need to 
    char createdDate[11];      // year-month-date 
    int  active;
} WorkoutPlan;

typedef struct {
    int   exerciseId;
    int   planId;            
    char  name[40];            // like "bench press"
    int   sets;
    int   reps;
    float weightKg;         
    int   restSeconds;
    int   active;
} Exercise;

void workoutMenu(void);

void createWorkoutPlan(void);
void viewWorkoutPlans(void);       
void updateWorkoutPlan(void);
void deleteWorkoutPlan(void);      

void addExercise(void);
void viewPlanExercises(void);
void updateExercise(void);
void deleteExercise(void);
void showPlanSummary(void);       

void viewMemberFitnessProfile(void);

#endif