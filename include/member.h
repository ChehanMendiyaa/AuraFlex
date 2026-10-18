
#ifndef MEMBER_H
#define MEMBER_H

typedef struct {
    char memberId[10];
    char fullName[50];
    char phone[20];
    char email[60];
    char status[15];
} Member;

void memberMenu(void);
void registerMember(void);
void viewMembers(void);
void searchMember(void);
void updateMember(void);
void freezeMember(void);
void unfreezeMember(void);

#endif
