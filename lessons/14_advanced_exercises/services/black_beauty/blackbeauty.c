#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char src_file[40] = "cat /home/blackbeautyuser/blackbeauty.c";

struct ticker_tape {
    long length;
    char * symbol;
    char tape[128];
};

void tape_machine(int user_length) {
    struct ticker_tape user_tape;
    user_tape.length = user_length;
    user_tape.symbol = malloc(16);
    strcpy(user_tape.symbol, "LANA");
    memset(user_tape.tape, 0, 128);
    long position = 0;
    int running = 1;
    int choice = 0;
    int i = 0;
    char value = 0;
    while (running) {
        puts("Ticker Tape Menu:");
        puts("1) Print stock values");
        printf("2) Write value at position %d\n", position);
        puts("3) Seek to position");
        puts("4) Change symbol");
        puts("8) View Source");
        puts("\n9) Exit");
        scanf("%d", &choice);
        if (choice == 9) {
            running = 0;
        }
        else if (choice == 1) {
            puts(user_tape.symbol);
            for (i = 0; i < user_tape.length; i++) {
                printf("#%d\n", user_tape.tape[i]);
            }
        }
        else if (choice == 2) {
            if (position < user_tape.length) {
                printf("Enter value: ");
                scanf("%d", &value);
                *(user_tape.tape + position) = value;
                ++position;
            }
            else {
                puts("End of tape.");
            }
        }
        else if (choice == 3) {
            printf("Enter position: ");
            scanf("%ld", &position);
        }
        else if (choice == 4) {
            printf("Change symbol: ");
            read(0, user_tape.symbol, 16);
        }
        else if (choice == 8) {
            system(src_file);
        }
    }
}

int main() {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    puts("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
    puts("% Nothing my sparrow, blue.  %");
    puts("%     Oh what can I do?      %");
    puts("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
    puts("% Stock Ticker Tape Program  %");
    puts("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");

    int user_length = 0;
    printf("Please enter your ticker tape length: ");
    scanf("%d", &user_length);
    if (user_length > 128 || user_length < 0) {
        puts("We don't have enough space to store your ticker tape.");
    }
    else {
        tape_machine(user_length);
    }
}
