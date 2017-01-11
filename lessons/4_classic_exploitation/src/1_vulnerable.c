#include <unistd.h>
#include <stdio.h>

void vuln() {
    char buffer[16];
    read(0, buffer, 100);
    puts(buffer);
}

int main() {
    vuln();
}
