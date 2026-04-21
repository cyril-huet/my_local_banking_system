#ifndef DATABASE_H
#define DATABASE_H

#include "account.h"

int db_account_exists(int id);

int db_create_account(int id, char *login, char *name, char *password);
int db_login(char *login, char *password, int *id, char *name);

float db_get_balance(int id);

void db_deposit(int id, float amount);
int db_withdraw(int id, float amount);
int db_transfer(int from, int to, float amount);

void db_show_history(int id);
void db_export_csv();

#endif