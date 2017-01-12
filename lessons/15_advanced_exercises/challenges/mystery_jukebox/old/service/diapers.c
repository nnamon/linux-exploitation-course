#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct diaper {
    char brand[12];
    int wet;
    char brand_message[100];
};

char * easter_egg = "d8dae94c2028d44c6cfe7834348df6659096102c3412988ebc1ff033bdbfb319";

void change_brand(struct diaper * diaper_obj) {
    int length = strlen(diaper_obj->brand);
    printf("Please enter brand to change to: ");
    fread(diaper_obj->brand, length, 1, stdin);
    printf("Diaper brand changed to: %s\n", diaper_obj->brand);
}

void change_diapers(struct diaper * diaper_obj) {
    --diaper_obj->wet;
    printf("Changed diaper. Wetness level: %d\n", diaper_obj->wet);
}

void spill_water(struct diaper * diaper_obj) {
    ++diaper_obj->wet;
    printf("Babby cries\n");
}

void leave_baby(struct diaper * diaper_obj) {
    printf("And now a message from our sponsors:\n");
    printf(diaper_obj->brand_message);
    puts("\n");
    if (diaper_obj->wet) {
        printf("Sorry, you're not allowed to leave your baby unless it's dry!\n");
    }
    else {
        puts("Shhhhh goodnight sleepy baby... go to b;ed");
        exit(0);
    }
}

void diaper_simulator() {
    // Zero the diaper struct
    struct diaper diaper_profile;
    memset(&diaper_profile, 0, sizeof(diaper_profile));

    // Display a list of diaper brands to set
    printf("Pick a brand:\n");
    printf("1. Bumstrat\n");
    printf("2. Sunny\n");
    printf("3. Bunny Rabbit\n");
    printf("4. Memememe\n");
    printf("5. Chilled ZZZZ\n");
    printf("> ");

    // Get number from user
    int choice = 0;
    scanf("%d", &choice);

    // Check that number is within the range
    if (choice <= 0 || choice > 5) {
        printf("Invalid Diaper brand\n");
        exit(1);
    }

    // Initialise the diapers with the parameters
    switch(choice) {
        case 1:
            memcpy(diaper_profile.brand, "Bumstrat", 8);
            strcpy(diaper_profile.brand_message, "Happy bums!");
            break;
        case 2:
            memcpy(diaper_profile.brand, "Sun", 3);
            strcpy(diaper_profile.brand_message, "Sunshines and sparkles");
            break;
        case 3:
            memcpy(diaper_profile.brand, "Bunny Rabbit", 12);
            strcpy(diaper_profile.brand_message, "Hippity hoppity hippity hop hop! Bunny Rabbits the best in town! Get a rabbit for your child!");
            break;
        case 4:
            memcpy(diaper_profile.brand, "Memememe", 8);
            strcpy(diaper_profile.brand_message, "Best sleep of your life!");
            break;
        case 5:
            memcpy(diaper_profile.brand, "Chilled ZZZZ", 12);
            strcpy(diaper_profile.brand_message, "zz");
            break;
    }
    diaper_profile.wet = 256;    // Wetness

    char cont = 1;
    while (cont) {
        if (diaper_profile.wet) {
            printf("Diapers are still wet...\n");
        }
        else {
            printf("Cool! Diapers are dry!\n");
        }
        printf("0) Change Brand\n");
        printf("1) Change Diapers\n");
        printf("2) Leave\n");
        printf("> ");
        scanf("%d", &choice);
        switch(choice) {
            case 0:
                change_brand(&diaper_profile);
                break;
            case 1:
                change_diapers(&diaper_profile);
                break;
            case 2:
                leave_baby(&diaper_profile);
                break;
            case 42:
                spill_water(&diaper_profile);
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

    puts("Welcome to the Diaper Change Simulator\n");
    diaper_simulator();
}
