#ifndef MODELS_H
#define MODELS_H

typedef struct{
    int id;
    char username[50];
    char password[50];
    char role[20];
}user;

typedef struct{
    int id;
    char name[100];
    int phone[10];
    char email[100];
    
}member;

typedef struct{
    int id;
    char name[100];
    char phone[15];
    char email[100];
    char specialization[30];
    int experienceYears;
    char workDays[30];
    char shiftStart[6];
    char shiftEnd[6];
}trainer;


#endif