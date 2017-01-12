#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct musics {
    char brand[12];
    int vol;
    char brand_message[100];
};

char * easter_egg = "d8dae94c2028d44c6cfe7834348df6659096102c3412988ebc1ff033bdbfb319";

void change_vinyl(struct musics * musics_obj) {
    int length = strlen(musics_obj->brand);
    printf("Please enter vinyl to change to: ");
    fread(musics_obj->brand, length, 1, stdin);
    printf("Musical brand changed to: %s\n", musics_obj->brand);
}

void change_volumes(struct musics * musics_obj) {
    --musics_obj->vol;
    printf("Changed volume. Current level: %d\n", musics_obj->vol);
}

void make_louder(struct musics * musics_obj) {
    ++musics_obj->vol;
    printf("Much louder\n");
}

void do_getdown(struct musics * musics_obj) {
    printf("And now a message from our sponsors:\n");
    printf(musics_obj->brand_message);
    puts("\n");
    if (musics_obj->vol) {
        printf("Sorry, you're not allowed to do the get down unless its quiet\n");
    }
    else {
        puts("Ohhhhhhh you got it alllll the way down;ed");
        exit(0);
    }
}

void musics_simulator() {
    // Zero the diaper struct
    struct musics musics_profile;
    memset(&musics_profile, 0, sizeof(musics_profile));

    // Display a list of diaper brands to set
    printf("Pick a brand:\n");
    printf("1. Eargasms\n");
    printf("2. Sunny\n");
    printf("3. Follow Rabbit\n");
    printf("4. Memememe\n");
    printf("5. Chilled ZZZZ\n");
    printf("> ");

    // Get number from user
    int choice = 0;
    scanf("%d", &choice);

    // Check that number is within the range
    if (choice <= 0 || choice > 5) {
        printf("Invalid Musics brand\n");
        exit(1);
    }

    // Initialise the diapers with the parameters
    switch(choice) {
        case 1:
            memcpy(musics_profile.brand, "Eargasms", 8);
            strcpy(musics_profile.brand_message, "Happy ears!");
            break;
        case 2:
            memcpy(musics_profile.brand, "Sun", 3);
            strcpy(musics_profile.brand_message, "Sunshines and sparkles");
            break;
        case 3:
            memcpy(musics_profile.brand, "Follow Rabbit", 12);
            strcpy(musics_profile.brand_message, "One pill will make you smarter, one pill makes you tall. Ask Alice. When she's ten feet tall.");
            break;
        case 4:
            memcpy(musics_profile.brand, "Memememe", 8);
            strcpy(musics_profile.brand_message, "Best songs of your life!");
            break;
        case 5:
            memcpy(musics_profile.brand, "Chilled ZZZZ", 12);
            strcpy(musics_profile.brand_message, "zz");
            break;
    }
    musics_profile.vol = 256;    // Wetness

    char cont = 1;
    while (cont) {
        if (musics_profile.vol) {
            printf("The volume's too loud!!!\n");
        }
        else {
            printf("Silence is the best..\n");
        }
        printf("0) Change Vinyl\n");
        printf("1) Lower Z Volume\n");
        printf("2) Leave\n");
        printf("> ");
        scanf("%d", &choice);
        switch(choice) {
            case 0:
                change_vinyl(&musics_profile);
                break;
            case 1:
                change_volumes(&musics_profile);
                break;
            case 2:
                do_getdown(&musics_profile);
                break;
            case 42:
                make_louder(&musics_profile);
                break;
            default:
                printf("Invalid choice\n");
                break;
        }
        choice = -1;
    }
}

int main() {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    puts("Welcome to the Mystery Jukebox Machine\n");
    musics_simulator();
}
