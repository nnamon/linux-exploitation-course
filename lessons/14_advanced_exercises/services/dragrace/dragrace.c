#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void violet(char * up) {
    char ginger[128];
    strcpy(ginger, up);
    puts(ginger);
}

int main() {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    char * pearl = malloc(512);
    memset(pearl, 0, 512);
    printf("Hiiiiiie, I live at %p\n", pearl);
    printf("Entertain me: ");
    fgets(pearl, 512, stdin);
    if (pearl[0] == 82 && pearl[1] == 117 && pearl[2] == 39 &&
        pearl[3] == 80 && pearl[4] == 97 && pearl[5] == 117 &&
        pearl[6] == 108 && pearl[7] == 115) {
        puts("We were living!");
        violet(pearl);
    }
    else {
        puts("Shashay away!\n");
    }
}
