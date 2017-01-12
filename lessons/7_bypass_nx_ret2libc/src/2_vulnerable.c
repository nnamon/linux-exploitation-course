#include <stdio.h>
#include <stdlib.h>

void vuln() {
    char buffer[64];
    read(0, buffer, 96);
}

int main() {
    vuln();
}
