#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "treasure_hunt.h"

void calculate_scores(const char *hunt_path) {
    DIR *dir = opendir(hunt_path);
    if (!dir) {
        perror("Failed to open hunt directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "treasure_", 9) == 0) {
            printf("Processing treasure: %s\n", entry->d_name);
        }
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