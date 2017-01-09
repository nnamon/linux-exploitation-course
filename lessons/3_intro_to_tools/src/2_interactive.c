#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void give_shell() {
    system("/bin/sh");
}

int main() {
    // Disable buffering on stdin and stdout to make network connections better.
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    char * password = "TheRealPassword";
    char user_password[200];

    puts("Welcome to the Super Secure Shell");
    printf("Password: ");

    scanf("%199s", user_password);
    if (strcmp(password, user_password) == 0) {
        puts("Correct password!");
        give_shell();
    }
    else {
        puts("Incorrect password!");
    }
}
