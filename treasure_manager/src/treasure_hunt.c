#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treasure_hunt.h"
#include "file_operations.h"
#include "logger.h"

#define MAX_TREASURES 100

static Treasure treasures[MAX_TREASURES];
static int treasure_count = 0;

void add_treasure(const char *hunt_id, Treasure treasure) {
    char treasure_file[256], log_file[256];

    // Initialize the users array and user_count
    treasure.user_count = 0;
    for (int i = 0; i < MAX_USERS; i++) {
        treasure.users[i][0] = '\0'; // Initialize each user slot as empty
    }

    // Create the treasure file
    snprintf(treasure_file, sizeof(treasure_file), "hunt/%s/treasure_%s.dat", hunt_id, treasure.id);
    FILE *file = fopen(treasure_file, "wb");
    if (!file) {
        perror("Error creating treasure file");
        return;
    }
    fwrite(&treasure, sizeof(Treasure), 1, file);
    fclose(file);

    // Log the action
    snprintf(log_file, sizeof(log_file), "hunt/%s/logs/log.txt", hunt_id);
    char log_entry[256];
    snprintf(log_entry, sizeof(log_entry), "Added treasure '%s' with value %d", treasure.id, treasure.value);
    log_action(log_file, log_entry);

    printf("Treasure '%s' added to hunt '%s'.\n", treasure.id, hunt_id);
}

void list_treasures() {
    printf("List of Treasures:\n");
    for (int i = 0; i < treasure_count; i++) {
        printf("Id: %s, Description: %s, Value: %d\n", treasures[i].id, treasures[i].description, treasures[i].value);
    }
}

void view_treasure(int index) {
    if (index >= 0 && index < treasure_count) {
        printf("Treasure Details:\n");
        printf("Name: %s\n", treasures[index].id);
        printf("Description: %s\n", treasures[index].description);
        printf("Value: %d\n", treasures[index].value);
    } else {
        fprintf(stderr, "Invalid treasure index.\n");
    }
}

void remove_treasure(const char *hunt_id, const char *treasure_id) {
    char treasure_file[256], log_file[256];

    // Construct the path to the treasure file
    snprintf(treasure_file, sizeof(treasure_file), "hunt/%s/treasure_%s.dat", hunt_id, treasure_id);

    // Attempt to delete the treasure file
    if (remove(treasure_file) == 0) {
        printf("Treasure '%s' removed from hunt '%s'.\n", treasure_id, hunt_id);

        // Log the action
        snprintf(log_file, sizeof(log_file), "hunt/%s/logs/log.txt", hunt_id);
        char log_entry[256];
        snprintf(log_entry, sizeof(log_entry), "Removed treasure '%s'", treasure_id);
        log_action(log_file, log_entry);
    } else {
        perror("Error removing treasure");
    }
}

// Function to create a new hunt
void create_hunt(const char *hunt_id) {
    char hunt_path[256], log_path[256], symlink_path[256];

    // Create the main hunt directory
    snprintf(hunt_path, sizeof(hunt_path), "hunt/%s", hunt_id);
    if (create_directory("hunt") == 0 && create_directory(hunt_path) == 0) {
        // Create the logs directory inside the hunt
        snprintf(log_path, sizeof(log_path), "%s/logs", hunt_path);
        if (create_directory(log_path) == 0) {
            // Create the log file for the hunt
            char log_file[256];
            snprintf(log_file, sizeof(log_file), "%s/log.txt", log_path);
            FILE *file = fopen(log_file, "w");
            if (file) {
                fprintf(file, "Hunt '%s' created.\n", hunt_id);
                fclose(file);
            } else {
                perror("Error creating log file");
                return;
            }

            // Create a symbolic link to the logs directory
            snprintf(symlink_path, sizeof(symlink_path), "log/final_logs_%s", hunt_id);
            if (create_symlink(log_path, symlink_path) == 0) {
                printf("Hunt '%s' created successfully with logs.\n", hunt_id);
            }
        }
    }
}

void add_user_to_treasure(const char *hunt_id, const char *treasure_id, const char *username) {
    char treasure_file[256];
    snprintf(treasure_file, sizeof(treasure_file), "hunt/%s/treasure_%s.dat", hunt_id, treasure_id);

    // Open the treasure file for reading and writing
    FILE *file = fopen(treasure_file, "rb+");
    if (!file) {
        perror("Error opening treasure file");
        return;
    }

    Treasure treasure;
    if (fread(&treasure, sizeof(Treasure), 1, file) != 1) {
        perror("Error reading treasure data");
        fclose(file);
        return;
    }

    // Check if the user is already associated with the treasure
    for (int i = 0; i < treasure.user_count; i++) {
        if (strcmp(treasure.users[i], username) == 0) {
            printf("User '%s' is already associated with treasure '%s'.\n", username, treasure_id);
            fclose(file);
            return;
        }
    }

    // Add the new user to the treasure
    if (treasure.user_count < MAX_USERS) {
        strncpy(treasure.users[treasure.user_count], username, MAX_LENGTH - 1);
        treasure.users[treasure.user_count][MAX_LENGTH - 1] = '\0'; // Ensure null-termination
        treasure.user_count++;

        // Seek back to the beginning of the file and update the treasure
        fseek(file, 0, SEEK_SET);
        if (fwrite(&treasure, sizeof(Treasure), 1, file) != 1) {
            perror("Error updating treasure data");
        } else {
            printf("User '%s' added to treasure '%s'.\n", username, treasure_id);
        }
    } else {
        printf("Cannot add more users to treasure '%s'. Maximum limit of %d users reached.\n", treasure_id, MAX_USERS);
    }

    fclose(file);
}