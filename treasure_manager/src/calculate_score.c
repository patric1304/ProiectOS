#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "treasure_hunt.h"

typedef struct {
    char user[MAX_LENGTH];
    int score;
} UserScore;

void calculate_scores(const char *hunt_path) {
    DIR *dir = opendir(hunt_path);
    if (!dir) {
        perror("Failed to open hunt directory");
        return;
    }

    UserScore user_scores[MAX_USERS] = {0};
    int user_count = 0;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "treasure_", 9) == 0) {
            char treasure_file[256];
            snprintf(treasure_file, sizeof(treasure_file), "%s/%s", hunt_path, entry->d_name);

            FILE *file = fopen(treasure_file, "rb");
            if (!file) {
                perror("Failed to open treasure file");
                continue;
            }

            Treasure treasure;
            if (fread(&treasure, sizeof(Treasure), 1, file) == 1) {
                for (int i = 0; i < treasure.user_count; i++) {
                    int found = 0;
                    for (int j = 0; j < user_count; j++) {
                        if (strcmp(user_scores[j].user, treasure.users[i]) == 0) {
                            user_scores[j].score += treasure.value;
                            found = 1;
                            break;
                        }
                    }
                    if (!found && user_count < MAX_USERS) {
                        strncpy(user_scores[user_count].user, treasure.users[i], MAX_LENGTH - 1);
                        user_scores[user_count].user[MAX_LENGTH - 1] = '\0';
                        user_scores[user_count].score = treasure.value;
                        user_count++;
                    }
                }
            }

            fclose(file);
        }
    }

    for (int i = 0; i < user_count; i++) {
        printf("User: %s, Score: %d\n", user_scores[i].user, user_scores[i].score);
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hunt_path>\n", argv[0]);
        return 1;
    }

    calculate_scores(argv[1]);
    return 0;
}