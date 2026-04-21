#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "database.h"

#define RESET "\033[0m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define CYAN "\033[36m"

void clear_input()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void input_password(char *password)
{
    struct termios old, new;
    tcgetattr(STDIN_FILENO, &old);
    new = old;

    new.c_lflag &= ~(ECHO); // désactive affichage normal
    tcsetattr(STDIN_FILENO, TCSANOW, &new);

    int i = 0;
    char ch;

    while ((ch = getchar()) != '\n' && i < 49)
    {
        if (ch == 127 || ch == 8)
        {
            if (i > 0)
            {
                i--;
                printf("\b \b"); // efface un *
            }
        }
        else
        {
            password[i++] = ch;
            printf("*"); // affiche *
        }
        fflush(stdout);
    }
    password[i] = '\0';
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
    printf("\n");
}
void user_menu(int id, char *name)
{
    int choice, to;
    float amount;

    while (1)
    {
        printf(CYAN "\n--- Welcome %s 👋 ---\n" RESET, name);
        printf("1. Balance\n");
        printf("2. Deposit\n");
        printf("3. Withdraw\n");
        printf("4. Transfer\n");
        printf("5. History\n");
        printf("6. Export CSV\n");
        printf("7. Reset DB\n");
        printf("0. Logout\n");
        printf("> ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            printf(GREEN "Balance: %.2f\n" RESET, db_get_balance(id));
            break;

        case 2:
            printf("Amount: ");
            scanf("%f", &amount);
            db_deposit(id, amount);
            printf(GREEN "Deposit OK\n" RESET);
            break;

        case 3:
            printf("Amount: ");
            scanf("%f", &amount);
            if (db_withdraw(id, amount))
            {
                printf(GREEN "Withdraw OK\n" RESET);
            }
            else
            {
                printf(RED "Not enough money\n" RESET);
            }
            break;

        case 4:
            printf("To (ID): ");
            scanf("%d", &to);
            printf("Amount: ");
            scanf("%f", &amount);

            if (db_transfer(id, to, amount))
            {
                printf(GREEN "Transfer OK\n" RESET);
            }
            else
            {
                printf(RED "Transfer failed\n" RESET);
            }
            break;

        case 5:
            db_show_history(id);
            break;

        case 6:
            db_export_csv();
            break;
        case 7:
            remove("accounts.db");
            remove("transactions.db");
            printf("Database reset\n");
            break;

        case 0:
            return;

        default:
            printf("Invalid choice\n");
        }
    }
}

int main()
{
    int choice;
    int id;
    char login[50];
    char name[50];
    char password[50];

    while (1)
    {
        printf(CYAN "\n=== BANK SYSTEM ===\n" RESET);
        printf("1. Create account\n");
        printf("2. Login\n");
        printf("0. Exit\n");
        printf("> ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            printf("ID: ");
            scanf("%d", &id);

            printf("Login: ");
            scanf("%s", login);

            printf("Name: ");
            scanf("%s", name);

            printf("Password: ");
            clear_input();
            input_password(password);

            if (db_create_account(id, login, name, password))
            {
                printf(GREEN "Account created\n" RESET);
            }
            else
            {
                printf(RED "Error creating account\n" RESET);
            }
            break;

        case 2: {
            int attempts = 3;
            printf("Login: ");
            scanf("%s", login);
            while (attempts--)
            {
                printf("Password: ");
                clear_input();
                input_password(password);

                if (db_login(login, password, &id, name))
                {
                    printf(GREEN "Login OK\n" RESET);
                    user_menu(id, name);
                    break;
                }
                else
                {
                    printf(RED "Wrong credentials (%d left)\n" RESET, attempts);
                }
            }
            break;
        }

        case 0:
            printf("Bye 👋\n");
            return 0;

        default:
            printf("Invalid choice\n");
        }
    }

    return 0;
}