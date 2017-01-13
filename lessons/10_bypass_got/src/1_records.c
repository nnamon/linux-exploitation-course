#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

struct record {
    char name[24];
    char * album;
};

int main() {
    // Print Title
    puts("This is a Jukebox");

    // Create the struct record
    struct record now_playing;
    strcpy(now_playing.name, "Simple Minds");
    now_playing.album = (char *) malloc(sizeof(char) * 24);
    strcpy(now_playing.album, "Breakfast");
    printf("Now Playing: %s (%s)\n", now_playing.name, now_playing.album);

    // Read some user data
    read(0, now_playing.name, 28);
    printf("Now Playing: %s (%s)\n", now_playing.name, now_playing.album);

    // Overwrite the album
    read(0, now_playing.album, 4);
    printf("Now Playing: %s (%s)\n", now_playing.name, now_playing.album);

    // Print the name again
    puts(now_playing.name);
}
