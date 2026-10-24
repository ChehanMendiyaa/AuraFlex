#ifndef TRAINER_H
#define TRAINER_H

void trainerMenu(void);
void registerTrainer(void);
void viewTrainers(void);
void searchTrainer(void);
void updateTrainer(void);
void changeTrainerStatus(void);

// Add members to a trainer 
int getMemberTrainer(int memberId);
void assignMemberToTrainer(void);
void removeMemberFromTrainer(void);
void viewTrainerMembers(void);

#endif