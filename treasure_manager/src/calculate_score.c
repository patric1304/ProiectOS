#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_USERS 10
#define MAX_LENGTH 256
#define MAX_PATH 256

typedef struct {
    char name[MAX_LENGTH];
    int score;
} UserScore;

typedef struct {
    char id[MAX_LENGTH];
    float latitude;
    float longitude;
    char description[MAX_LENGTH];
    int value;
    char users[MAX_USERS][MAX_LENGTH];
    int user_count;
} Treasure;

int load_treasure(const char *filepath, Treasure *treasure) {
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        perror("Failed to open treasure file");
        return 0;
    }

    if (fread(treasure, sizeof(Treasure), 1, file) != 1) {
        perror("Failed to read treasure data");
        fclose(file);
        return 0;
    }

    fclose(file);
    return 1;
}

int find_user_index(UserScore *scores, int count, const char *username) {
    for (int i = 0; i < count; ++i) {
        if (strcmp(scores[i].name, username) == 0)
            return i;
    }
    return -1;
}

void print_scores(UserScore *scores, int count) {
    for (int i = 0; i < count; ++i) {
        printf("%s: %d\n", scores[i].name, scores[i].score);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hunt_folder>\n", argv[0]);
        return 1;
    }

    char path[MAX_PATH];
    snprintf(path, sizeof(path), "../hunt/%s", argv[1]);
    DIR *dir = opendir(path);
    if (!dir) {
        perror("Failed to open hunt folder");
        return 1;
    }

    struct dirent *entry;
    UserScore scores[MAX_USERS];
    int score_count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".dat")) {
            char filepath[MAX_PATH];
            snprintf(filepath, sizeof(filepath), "%s/%s", path, entry->d_name);

            Treasure treasure;
            if (load_treasure(filepath, &treasure)) {
                for (int i = 0; i < treasure.user_count; ++i) {
                    int idx = find_user_index(scores, score_count, treasure.users[i]);
                    if (idx != -1) {
                        scores[idx].score += treasure.value;
                    } else {
                        strncpy(scores[score_count].name, treasure.users[i], MAX_LENGTH - 1);
                        scores[score_count].name[MAX_LENGTH - 1] = '\0';
                        scores[score_count].score = treasure.value;
                        score_count++;
                    }
                }
            } else {
                fprintf(stderr, "Failed to load treasure from file: %s\n", filepath);
            }
        }
    }

    closedir(dir);

    print_scores(scores, score_count);
    return 0;
}
