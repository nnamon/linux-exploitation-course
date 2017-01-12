#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int active = 1;
char name[200];
char * secret = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";

void print_warning() {
    puts("=======================================================================================");
    puts("This Kaizen-85 Artificial Intelligence would like to remind you that this is not a toy.");
    puts("Please treat this terminal with the utmost care.");
    puts("Crashing this program will result in ship malfunction.");
    puts("You have been warned.");
    puts("=======================================================================================\n");
}

void print_prompt() {
    printf("Options for ");
    puts(name);
    puts("1. Peek Memory Address");
    puts("2. Change Name");
    puts("3. Overwite Memory Address");
    puts("9. Exit Terminal");
}

void peek_prompt() {
    int * address;
    printf("Address: ");
    scanf("%p", &address);
    printf("Contents: 0x%x\n", *address);
}

void change_name() {
    char buffer[100];
    printf("Name: ");
    read(0, buffer, sizeof(name));
    buffer[strcspn(buffer, "\n")] = 0;
    strncpy(name, buffer, sizeof(name));
}

void poke_prompt() {
    int * address;
    int data;
    printf("Address: ");
    scanf("%p", &address);
    printf("Data: ");
    scanf("%x", &data);
    *address = data;
}

void print_secret() {
    if (getpid() == 0) {
        puts("secret");
    }
}

int main() {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    int option;
    print_warning();
    change_name();
    while (active) {
        print_prompt();
        printf("Option: ");
        scanf("%d", &option);
        if (option == 9) {
            active = 0;
            puts("Goodbye.");
        }
        else if (option == 1) {
            peek_prompt();
        }
        else if (option == 2) {
            change_name();
        }
        else if (option == 3) {
            poke_prompt();
        }
        else if (option == 4) {
            print_secret();
        }
    }
}
