#include "account.h"

#include <stdio.h>
#include <string.h>

Account create_account(int id, char *name, char *password)
{
    Account account;
    account.id = id;
    strcpy(account.name, name);
    strcpy(account.password, password);
    account.balance = 0.0;
    return account;
}

void deposit(Account *account, float amount)
{
    if (amount > 0)
    {
        account->balance += amount;
    }
}

int withdraw(Account *account, float amount)
{
    if (amount > 0 && account->balance >= amount)
    {
        account->balance -= amount;
        return 1;
    }
    return 0;
}

int transfer(Account *src, Account *dst, float amount)
{
    if (amount > 0 && src->balance >= amount)
    {
        src->balance -= amount;
        dst->balance += amount;
        return 1;
    }
    return 0;
}

void print_account(Account account)
{
    printf("ID: %d\nName: %s\nBalance: %.2f\n", account.id, account.name,
           account.balance);
}

Account *login(Account accounts[], int count, int id, char *password)
{
    for (int i = 0; i < count; i++)
    {
        if (accounts[i].id == id && strcmp(accounts[i].password, password) == 0)
        {
            return &accounts[i];
        }
    }
    return NULL;
}