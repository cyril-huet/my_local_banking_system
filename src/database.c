#include "database.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#define DB_FILE "accounts.db"
#define TX_FILE "transactions.db"

typedef struct
{
    int from;
    int to;
    float amount;
    char date[30];
} Transaction;

/* ========================= */
/* ACCOUNT */
/* ========================= */

int db_account_exists(int id)
{
    FILE *f = fopen(DB_FILE, "rb");
    if (!f)
    {
        return 0;
    }

    Account account;
    while (fread(&account, sizeof(Account), 1, f))
    {
        if (account.id == id)
        {
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0;
}

int db_create_account(int id, char *login, char *name, char *password)
{
    if (id <= 0 || strlen(login) == 0 || strlen(password) == 0)
    {
        return 0;
    }

    FILE *file = fopen(DB_FILE, "ab+");
    if (!file)
    {
        return 0;
    }

    Account acc;

    rewind(file);
    while (fread(&acc, sizeof(Account), 1, file))
    {
        if (acc.id == id || strcmp(acc.login, login) == 0)
        {
            printf("ID or login already exists\n");
            fclose(file);
            return 0;
        }
    }

    acc.id = id;
    strcpy(acc.login, login);
    strcpy(acc.name, name);
    strcpy(acc.password, password);
    acc.balance = 0;

    fwrite(&acc, sizeof(Account), 1, file);
    fclose(file);

    return 1;
}

int db_login(char *login, char *password, int *id, char *name)
{
    FILE *f = fopen(DB_FILE, "rb");
    if (!f)
    {
        return 0;
    }

    Account acc;

    while (fread(&acc, sizeof(Account), 1, f))
    {
        if (strcmp(acc.login, login) == 0
            && strcmp(acc.password, password) == 0)
        {
            *id = acc.id;
            strcpy(name, acc.name);
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0;
}

float db_get_balance(int id)
{
    FILE *f = fopen(DB_FILE, "rb");
    if (!f)
    {
        return 0;
    }

    Account acc;

    while (fread(&acc, sizeof(Account), 1, f))
    {
        if (acc.id == id)
        {
            fclose(f);
            return acc.balance;
        }
    }

    fclose(f);
    return 0;
}

/* ========================= */
/* TRANSACTIONS */
/* ========================= */

void db_save_transaction(int from, int to, float amount)
{
    FILE *f = fopen(TX_FILE, "ab");
    if (!f)
    {
        return;
    }

    Transaction tx;
    tx.from = from;
    tx.to = to;
    tx.amount = amount;

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(tx.date, sizeof(tx.date), "%Y-%m-%d %H:%M:%S", tm_info);
    fwrite(&tx, sizeof(Transaction), 1, f);

    fclose(f);
}

void db_deposit(int id, float amount)
{
    if (amount <= 0)
    {
        return;
    }

    FILE *f = fopen(DB_FILE, "rb+");
    if (!f)
    {
        return;
    }

    Account acc;

    while (fread(&acc, sizeof(Account), 1, f))
    {
        if (acc.id == id)
        {
            acc.balance += amount;
            fseek(f, -sizeof(Account), SEEK_CUR);
            fwrite(&acc, sizeof(Account), 1, f);

            db_save_transaction(0, id, amount);
            break;
        }
    }

    fclose(f);
}

int db_withdraw(int id, float amount)
{
    if (amount <= 0)
    {
        return 0;
    }

    FILE *f = fopen(DB_FILE, "rb+");
    if (!f)
    {
        return 0;
    }

    Account acc;

    while (fread(&acc, sizeof(Account), 1, f))
    {
        if (acc.id == id)
        {
            if (acc.balance >= amount)
            {
                acc.balance -= amount;
                fseek(f, -sizeof(Account), SEEK_CUR);
                fwrite(&acc, sizeof(Account), 1, f);

                db_save_transaction(id, 0, amount);
                fclose(f);
                return 1;
            }
            fclose(f);
            return 0;
        }
    }

    fclose(f);
    return 0;
}

int db_transfer(int from, int to, float amount)
{
    if (from == to || amount <= 0)
    {
        return 0;
    }

    if (!db_account_exists(to))
    {
        return 0;
    }

    if (!db_withdraw(from, amount))
    {
        return 0;
    }

    db_deposit(to, amount);
    db_save_transaction(from, to, amount);

    return 1;
}

/* ========================= */
/* HISTORY */
/* ========================= */

void db_show_history(int id)
{
    FILE *f = fopen(TX_FILE, "rb");
    if (!f)
    {
        printf("No transactions\n");
        return;
    }

    Transaction tx;

    printf("\n--- HISTORY ---\n");

    while (fread(&tx, sizeof(Transaction), 1, f))
    {
        if (tx.from == id || tx.to == id)
        {
            if (tx.from == 0)
                printf("[DEPOSIT] +%.2f (%s)\n", tx.amount, tx.date);
            else if (tx.to == 0)
                printf("[WITHDRAW] -%.2f (%s)\n", tx.amount, tx.date);
            else if (tx.from == id)
                printf("[SEND] -> %d : -%.2f (%s)\n", tx.to, tx.amount,
                       tx.date);
            else
                printf("[RECEIVE] <- %d : +%.2f (%s)\n", tx.from, tx.amount,
                       tx.date);
        }
    }

    fclose(f);
}

/* ========================= */
/* CSV */
/* ========================= */

void db_export_csv()
{
    FILE *f = fopen(TX_FILE, "rb");
    if (!f)
    {
        return;
    }

    FILE *csv = fopen("transactions.csv", "w");
    if (!csv)
    {
        return;
    }

    fprintf(csv, "from,to,amount,date\n");

    Transaction tx;
    while (fread(&tx, sizeof(Transaction), 1, f))
    {
        fprintf(csv, "%d,%d,%.2f,%s\n", tx.from, tx.to, tx.amount, tx.date);
    }

    fclose(f);
    fclose(csv);

    printf("Exported to transactions.csv\n");
}