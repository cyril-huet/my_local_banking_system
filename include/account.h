#ifndef ACCOUNT_H
#define ACCOUNT_H

typedef struct
{
    int id;
    char login[256];
    char name[256];
    char password[256];
    float balance;
} Account;

#endif