#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "treasure_hunt.h"
#include "file_operations.h"
#include "logger.h"

#define MAX_TREASURES 100

static Treasure treasures[MAX_TREASURES];
static int treasure_count = 0;

void add_treasure(const char *hunt_id, Treasure treasure, int userNumber, char users[][MAX_LENGTH]) {
    char treasure_file[256], log_file[256];

    // Initialize the users array and user_count
    treasure.user_count = 0;
    for (int i = 0; i < userNumber && i < MAX_USERS; i++) {
        strncpy(treasure.users[i], users[i], MAX_LENGTH - 1);
        treasure.users[i][MAX_LENGTH - 1] = '\0'; // Ensure null-termination
        treasure.user_count++;
    }

    // Create the treasure file
    snprintf(treasure_file, sizeof(treasure_file), "../hunt/%s/treasure_%s.dat", hunt_id, treasure.id);
    FILE *file = fopen(treasure_file, "wb");
    if (!file) {
        perror("Error creating treasure file");
        return;
    }
    fwrite(&treasure, sizeof(Treasure), 1, file);
    fclose(file);

    // Log the action
    snprintf(log_file, sizeof(log_file), "../hunt/%s/%s_logs.txt", hunt_id, hunt_id);
    char log_entry[256];
    snprintf(log_entry, sizeof(log_entry), "Added treasure '%s' with value %d and %d users", treasure.id, treasure.value, treasure.user_count);
    log_action(log_file, log_entry);

    printf("Treasure '%s' added to hunt '%s' with %d users.\n", treasure.id, hunt_id, treasure.user_count);
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
    snprintf(treasure_file, sizeof(treasure_file), "../hunt/%s/treasure_%s.dat", hunt_id, treasure_id);

    // Attempt to delete the treasure file
    if (remove(treasure_file) == 0) {
        printf("Treasure '%s' removed from hunt '%s'.\n", treasure_id, hunt_id);

        // Log the action
        snprintf(log_file, sizeof(log_file), "../hunt/%s/%s_logs.txt", hunt_id, hunt_id);
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
    snprintf(hunt_path, sizeof(hunt_path), "../hunt/%s", hunt_id);
    if (create_directory("../hunt") == 0 && create_directory(hunt_path) == 0) {
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

            // Ensure the symbolic link directory exists
            create_directory("../log");

            // Create a symbolic link to the logs directory
            snprintf(symlink_path, sizeof(symlink_path), "../log/final_logs_%s", hunt_id);
            if (create_symlink(log_path, symlink_path) == 0) {
                printf("Hunt '%s' created successfully with logs.\n", hunt_id);
            } else {
                perror("Error creating symbolic link");
            }
        }
    }
}

void add_user_to_treasure(const char *hunt_id, const char *treasure_id, const char *username) {
    char treasure_file[256];
    snprintf(treasure_file, sizeof(treasure_file), "../hunt/%s/treasure_%s.dat", hunt_id, treasure_id);

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

void view(const char *hunt_id, const char *treasure_id) {
    char treasure_file[256];
    snprintf(treasure_file, sizeof(treasure_file), "../hunt/%s/treasure_%s.dat", hunt_id, treasure_id);

    FILE *file = fopen(treasure_file, "rb");
    if (!file) {
        perror("Error opening treasure file");
        return;
    }

    printf("Treasures in Hunt '%s':\n", hunt_id);

    Treasure treasure;
    while (fread(&treasure, sizeof(Treasure), 1, file) == 1) {
        printf("ID: %s\n", treasure.id);
        printf("Latitude: %.2f\n", treasure.latitude);
        printf("Longitude: %.2f\n", treasure.longitude);
        printf("Description: %s\n", treasure.description);
        printf("Value: %d\n", treasure.value);
        printf("Users (%d): ", treasure.user_count);
        for (int i = 0; i < treasure.user_count; i++) {
            printf("%s ", treasure.users[i]);
        }
        printf("\n\n");
    }

    fclose(file);
}

void list(const char *hunt_id) {
    char hunt_path[256];
    snprintf(hunt_path, sizeof(hunt_path), "../hunt/%s", hunt_id);

    DIR *dir = opendir(hunt_path);
    if (!dir) {
        fprintf(stderr, "Error opening hunt directory '%s': %s\n", hunt_path, strerror(errno));
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip '.' and '..'
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Check if file starts with "treasure_" and ends with ".dat"
        if (strncmp(entry->d_name, "treasure_", 9) == 0) {
            size_t len = strlen(entry->d_name);
            if (len > 4 && strcmp(entry->d_name + len - 4, ".dat") == 0) {
                // Extract treasure_id from filename (between "treasure_" and ".dat")
                char treasure_id[128];
                strncpy(treasure_id, entry->d_name + 9, len - 13); // 9 for prefix, 4 for suffix
                treasure_id[len - 13] = '\0';

                // Use view() to show treasure details
                view(hunt_id, treasure_id);
            }
        }
    }

    closedir(dir);
}

void remove_hunt(const char *hunt_id) {
    char hunt_path[256];
    snprintf(hunt_path, sizeof(hunt_path), "../hunt/%s", hunt_id);

    // 1) Remove all treasure .dat files
    DIR *d = opendir(hunt_path);
    if (!d) {
        fprintf(stderr, "Error opening '%s': %s\n", hunt_path, strerror(errno));
        return;
    }
    struct dirent *e;
    while ((e = readdir(d)) != NULL) {
        // skip "." and ".."
        if (e->d_name[0]=='.' && (e->d_name[1]=='\0' || (e->d_name[1]=='.'&&e->d_name[2]=='\0')))
            continue;
        if (strncmp(e->d_name, "treasure_", 9)==0) {
            size_t L = strlen(e->d_name);
            if (L>13 && strcmp(e->d_name+L-4, ".dat")==0) {
                char tid[128];
                memcpy(tid, e->d_name+9, L-9-4);
                tid[L-9-4] = '\0';
                remove_treasure(hunt_id, tid);
            }
        }
    }
    closedir(d);

    // 2) Remove logs/ subdirectory (just its files, then rmdir)
    char logs_dir[256];
    snprintf(logs_dir, sizeof(logs_dir), "%s/logs", hunt_path);
    d = opendir(logs_dir);
    if (d) {
        while ((e = readdir(d)) != NULL) {
            if (strcmp(e->d_name, ".")==0 || strcmp(e->d_name, "..")==0) continue;
            char f[512];
            snprintf(f, sizeof(f), "%s/%s", logs_dir, e->d_name);
            if (remove(f)!=0)
                fprintf(stderr, "Failed to remove '%s': %s\n", f, strerror(errno));
        }
        closedir(d);
        if (rmdir(logs_dir)!=0)
            fprintf(stderr, "Failed to rmdir '%s': %s\n", logs_dir, strerror(errno));
    }

    // 3) Remove the external log file (pattern: ../logs/<hunt_id>_log.txt)
    char ext_log[256];
    snprintf(ext_log, sizeof(ext_log), "../logs/%s_log.txt", hunt_id);
    if (remove(ext_log)!=0 && errno!=ENOENT)
        fprintf(stderr, "Failed to remove external log '%s': %s\n", ext_log, strerror(errno));

    // 4) Remove the symlink in the hunt folder (it’s named "<hunt_id>_logs.txt")
    char link_path[256];
    snprintf(link_path, sizeof(link_path), "%s/%s_logs.txt", hunt_path, hunt_id);
    if (remove(link_path)!=0 && errno!=ENOENT)
        fprintf(stderr, "Failed to remove symlink '%s': %s\n", link_path, strerror(errno));

    // 5) Finally, remove the empty hunt directory
    if (rmdir(hunt_path)==0) {
        printf("Hunt '%s' fully removed.\n", hunt_id);
    } else {
        fprintf(stderr, "Could not remove '%s': %s\n", hunt_path, strerror(errno));
    }
}
