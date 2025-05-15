#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "treasure_manager.h"
#include "file_operations.h"
#include "logger.h"

#define MAX_TREASURES 100
#define MAX_PATH 4096 

static Treasure treasures[MAX_TREASURES];
static int treasure_count = 0;

void add_treasure(const char *hunt_id, Treasure treasure, int userNumber, char users[][MAX_LENGTH]) {
    char treasure_file[MAX_PATH], log_file[MAX_PATH];

    treasure.user_count = 0;
    for (int i = 0; i < userNumber && i < MAX_USERS; i++) {
        strncpy(treasure.users[i], users[i], MAX_LENGTH - 1);
        treasure.users[i][MAX_LENGTH - 1] = '\0'; 
        treasure.user_count++;
    }

    snprintf(treasure_file, sizeof(treasure_file), "../hunt/%s/treasure_%s.dat", hunt_id, treasure.id);
    FILE *file = fopen(treasure_file, "wb");
    if (!file) {
        perror("Error creating treasure file");
        return;
    }
    fwrite(&treasure, sizeof(Treasure), 1, file);
    fclose(file);

    snprintf(log_file, sizeof(log_file), "../hunt/%s/logs/log.txt", hunt_id);
    char log_entry[512];
    snprintf(log_entry, sizeof(log_entry), "Added treasure '%s' with value %d and %d users\n", treasure.id, treasure.value, treasure.user_count);
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
    char treasure_file[MAX_PATH], log_file[MAX_PATH];
    snprintf(treasure_file, sizeof(treasure_file), "../hunt/%s/treasure_%s.dat", hunt_id, treasure_id);
    if (remove(treasure_file) == 0) {
        printf("Treasure '%s' removed from hunt '%s'.\n", treasure_id, hunt_id);

        snprintf(log_file, sizeof(log_file), "../hunt/%s/logs/log.txt", hunt_id);
        char log_entry[256];
        snprintf(log_entry, sizeof(log_entry), "Removed treasure '%s'\n", treasure_id);
        log_action(log_file, log_entry);
    } else {
        perror("Error removing treasure");
    }
}

void create_hunt(const char *hunt_id) {
    char hunt_path[MAX_PATH], log_path[MAX_PATH], symlink_path[MAX_PATH];
    int written = snprintf(hunt_path, sizeof(hunt_path), "../hunt/%s", hunt_id);
    if (written < 0 || written >= sizeof(hunt_path)) {
        fprintf(stderr, "Path too long for hunt_path buffer\n");
        return;
    }
    if (create_directory("../hunt") == 0 && create_directory(hunt_path) == 0) {
        written = snprintf(log_path, sizeof(log_path), "%s/logs", hunt_path);
        if (written < 0 || written >= sizeof(log_path)) {
            fprintf(stderr, "Path too long for log_path buffer\n");
            return;
        }
        if (create_directory(log_path) == 0) {
            char log_file[MAX_PATH];
            written = snprintf(log_file, sizeof(log_file), "%s/log.txt", log_path);
            if (written < 0 || written >= sizeof(log_file)) {
                fprintf(stderr, "Path too long for log_file buffer\n");
                return;
            }
            FILE *file = fopen(log_file, "w");
            if (file) {
                fprintf(file, "Hunt '%s' created.\n", hunt_id);
                fclose(file);
            } else {
                perror("Error creating log file");
                return;
            }

            create_directory("../log");

            written = snprintf(symlink_path, sizeof(symlink_path), "../log/final_logs_%s", hunt_id);
            if (written < 0 || written >= sizeof(symlink_path)) {
                fprintf(stderr, "Path too long for symlink_path buffer\n");
                return;
            }
            if (create_symlink(log_path, symlink_path) == 0) {
                printf("Hunt '%s' created successfully with logs.\n", hunt_id);
            } else {
                perror("Error creating symbolic link");
            }
        }
    }
}

void add_user_to_treasure(const char *hunt_id, const char *treasure_id, const char *username) {
    char treasure_file[MAX_PATH], log_file[MAX_PATH];
    snprintf(treasure_file, sizeof(treasure_file), "../hunt/%s/treasure_%s.dat", hunt_id, treasure_id);

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

    for (int i = 0; i < treasure.user_count; i++) {
        if (strcmp(treasure.users[i], username) == 0) {
            printf("User '%s' is already associated with treasure '%s'.\n", username, treasure_id);
            fclose(file);
            return;
        }
    }

    if (treasure.user_count < MAX_USERS) {
        strncpy(treasure.users[treasure.user_count], username, MAX_LENGTH - 1);
        treasure.users[treasure.user_count][MAX_LENGTH - 1] = '\0'; 
        treasure.user_count++;

        fseek(file, 0, SEEK_SET);
        if (fwrite(&treasure, sizeof(Treasure), 1, file) != 1) {
            perror("Error updating treasure data");
        } else {
            printf("User '%s' added to treasure '%s'.\n", username, treasure_id);

            snprintf(log_file, sizeof(log_file), "../hunt/%s/logs/log.txt", hunt_id);
            char log_entry[256];
            snprintf(log_entry, sizeof(log_entry), "Added user '%s' to treasure '%s'\n", username, treasure_id);
            log_action(log_file, log_entry);
        }
    } else {
        printf("Cannot add more users to treasure '%s'. Maximum limit of %d users reached.\n", treasure_id, MAX_USERS);
    }

    fclose(file);
}

void view(const char *hunt_id, const char *treasure_id) {
    char treasure_file[MAX_PATH];
    snprintf(treasure_file, sizeof(treasure_file), "../hunt/%s/treasure_%s.dat", hunt_id, treasure_id);

    FILE *file = fopen(treasure_file, "rb");
    if (!file) {
        perror("Error opening treasure file");
        return;
    }

    Treasure treasure;
    if (fread(&treasure, sizeof(Treasure), 1, file) == 1) {
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
    char hunt_path[MAX_PATH];
    snprintf(hunt_path, sizeof(hunt_path), "../hunt/%s", hunt_id);

    DIR *dir = opendir(hunt_path);
    if (!dir) {
        fprintf(stderr, "Error opening hunt directory '%s': %s\n", hunt_path, strerror(errno));
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (strncmp(entry->d_name, "treasure_", 9) == 0) {
            size_t len = strlen(entry->d_name);
            if (len > 4 && strcmp(entry->d_name + len - 4, ".dat") == 0) {
                char treasure_id[128];
                strncpy(treasure_id, entry->d_name + 9, len - 13);
                treasure_id[len - 13] = '\0';

                view(hunt_id, treasure_id);
            }
        }
    }

    closedir(dir);
}

void list_hunts() {
    DIR *dir = opendir("../hunt");
    if (!dir) {
        printf("No hunts found.\n");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR &&
            strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0) {
            // Count treasures in this hunt
            char hunt_path[512];
            snprintf(hunt_path, sizeof(hunt_path), "../hunt/%s", entry->d_name);
            DIR *hunt_dir = opendir(hunt_path);
            int treasure_count = 0;
            if (hunt_dir) {
                struct dirent *e;
                while ((e = readdir(hunt_dir)) != NULL) {
                    if (strncmp(e->d_name, "treasure_", 9) == 0) {
                        treasure_count++;
                    }
                }
                closedir(hunt_dir);
            }
            printf("Hunt: %s, Treasures: %d\n", entry->d_name, treasure_count);
        }
    }
    closedir(dir);
}

void remove_hunt(const char *hunt_id) {
    char hunt_path[MAX_PATH];
    int written = snprintf(hunt_path, sizeof(hunt_path), "../hunt/%s", hunt_id);
    if (written < 0 || written >= sizeof(hunt_path)) {
        fprintf(stderr, "Path too long for hunt_path buffer\n");
        return;
    }

    DIR *d = opendir(hunt_path);
    if (!d) {
        fprintf(stderr, "Error opening '%s': %s\n", hunt_path, strerror(errno));
        return;
    }

    // Remove all treasures in the hunt directory
    struct dirent *e;
    while ((e = readdir(d)) != NULL) {
        if (e->d_name[0] == '.' && (e->d_name[1] == '\0' || (e->d_name[1] == '.' && e->d_name[2] == '\0')))
            continue;
        if (strncmp(e->d_name, "treasure_", 9) == 0) {
            size_t L = strlen(e->d_name);
            if (L > 13 && strcmp(e->d_name + L - 4, ".dat") == 0) {
                char tid[128];
                memcpy(tid, e->d_name + 9, L - 9 - 4);
                tid[L - 9 - 4] = '\0';
                remove_treasure(hunt_id, tid);
            }
        }
    }
    closedir(d);

    // Remove the logs directory inside the hunt
    char logs_dir[MAX_PATH];
    written = snprintf(logs_dir, sizeof(logs_dir), "%s/logs", hunt_path);
    if (written < 0 || written >= sizeof(logs_dir)) {
        fprintf(stderr, "Path too long for logs_dir buffer\n");
        return;
    }
    d = opendir(logs_dir);
    if (d) {
        while ((e = readdir(d)) != NULL) {
            if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0) continue;
            char f[MAX_PATH];
            written = snprintf(f, sizeof(f), "%s/%s", logs_dir, e->d_name);
            if (written < 0 || written >= sizeof(f)) {
                fprintf(stderr, "Path too long for f buffer\n");
                continue;
            }
            if (remove(f) != 0)
                fprintf(stderr, "Failed to remove '%s': %s\n", f, strerror(errno));
        }
        closedir(d);
        if (rmdir(logs_dir) != 0)
            fprintf(stderr, "Failed to rmdir '%s': %s\n", logs_dir, strerror(errno));
    }

    char ext_log[MAX_PATH];
    written = snprintf(ext_log, sizeof(ext_log), "../logs/%s_log.txt", hunt_id);
    if (written < 0 || written >= sizeof(ext_log)) {
        fprintf(stderr, "Path too long for ext_log buffer\n");
        return;
    }
    if (remove(ext_log) != 0 && errno != ENOENT)
        fprintf(stderr, "Failed to remove external log '%s': %s\n", ext_log, strerror(errno));

    // Remove the symbolic link in the log folder
    char final_logs_symlink[MAX_PATH];
    written = snprintf(final_logs_symlink, sizeof(final_logs_symlink), "../log/final_logs_%s", hunt_id);
    if (written < 0 || written >= sizeof(final_logs_symlink)) {
        fprintf(stderr, "Path too long for final_logs_symlink buffer\n");
        return;
    }
    if (remove(final_logs_symlink) != 0 && errno != ENOENT) {
        fprintf(stderr, "Failed to remove symbolic link '%s': %s\n", final_logs_symlink, strerror(errno));
    }

    // Remove the hunt directory itself
    if (rmdir(hunt_path) == 0) {
        printf("Hunt '%s' fully removed.\n", hunt_id);
    } else {
        fprintf(stderr, "Could not remove '%s': %s\n", hunt_path, strerror(errno));
    }
}

void help() {
    printf("Usage: treasure_manager [command] [options]\n");
    printf("Commands:\n");
    printf("  create_hunt       <hunt_id>                                Create a new treasure hunt\n");
    printf("  add_treasure      <hunt_id> <id> <user1> [user2 ...] <lat> <lon> <desc> <value>  Add a treasure with users\n");
    printf("  add_user          <hunt_id> <treasure_id> <username>       Add a user to a treasure\n");
    printf("  remove_treasure   <hunt_id> <treasure_id>                  Remove a treasure from a hunt\n");
    printf("  remove_hunt       <hunt_id>                                Remove a treasure hunt\n");
    printf("  view_treasure     <hunt_id> <treasure_id>                  View details of a treasure\n");
    printf("  list_treasures    <hunt_id>                                List all treasures in a hunt\n");
    printf("  list_hunts                                                List all hunts\n");
    printf("  help                                                      Display this help message\n");
}
