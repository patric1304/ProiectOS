#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include "treasure_hunt.h"

#define CMD_FILE "command.txt"

volatile sig_atomic_t stop_requested = 0;

void handle_signal() {
    printf("handle_signal called\n");

    FILE *cmd_file = fopen(CMD_FILE, "r");
    if (!cmd_file) {
        perror("Failed to open command file");
        return;
    }

    char command[256];
    if (fgets(command, sizeof(command), cmd_file)) {
        command[strcspn(command, "\n")] = '\0'; // Remove trailing newline
        FILE *resp_file = fopen("response.txt", "ap");
        if (!resp_file) {
            perror("Failed to open response file");
            fclose(cmd_file);
            return;
        }
        fprintf(resp_file, "Debugging %s\n", command);

        if (strcmp(command, "list_hunts") == 0) {
            fprintf(resp_file, "Listing all hunts...\n");
            DIR *dir = opendir("../hunt");
            if (!dir) {
                fprintf(resp_file, "Failed to open hunt directory.\n");
            } else {
                struct dirent *entry;
                while ((entry = readdir(dir)) != NULL) {
                    if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                        fprintf(resp_file, "Hunt: %s\n", entry->d_name);
                    }
                }
                closedir(dir);
            }
        } else if (strcmp(command, "list_treasures") == 0) {
            fprintf(resp_file, "Listing all treasures...\n");

            // Prompt the user for the hunt ID
            char hunt_id[256];
            printf("Enter hunt ID: ");
            if (scanf("%255s", hunt_id) != 1) {
                fprintf(resp_file, "Error: Failed to read hunt ID from keyboard.\n");
                fclose(resp_file);
                fclose(cmd_file);
                return;
         }
            list(hunt_id);
        } else if (strcmp(command, "view_treasure") == 0) {
            fprintf(resp_file, "Viewing a specific treasure...\n");
            char hunt_id[256], treasure_id[256];
            printf("Enter hunt ID: ");
            scanf("%s", hunt_id);
            printf("Enter treasure ID: ");
            scanf("%s", treasure_id);
            view(hunt_id, treasure_id);
        } else if (strcmp(command, "help") == 0) {
            fprintf(resp_file, "Available commands:\n");
            fprintf(resp_file, "  list_hunts       - List all hunts\n");
            fprintf(resp_file, "  list_treasures   - List all treasures in a hunt\n");
            fprintf(resp_file, "  view_treasure    - View details of a specific treasure\n");
            fprintf(resp_file, "  help             - Display this help message\n");
        } else {
            fprintf(resp_file, "Unknown command: %s\n", command);
        }

        fclose(resp_file);
    }

    fclose(cmd_file);
}

void handle_signal_usr1(int sig) {
    if (sig == SIGUSR1) {
        handle_signal();
    }
}

void handle_signal_usr2(int sig) {
    if (sig == SIGUSR2) {
        stop_requested = 1;
    }
}

void setup_signal_handlers() {
    struct sigaction sa;
    sa.sa_handler = handle_signal_usr1;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("Error setting SIGUSR1 handler");
        exit(EXIT_FAILURE);
    }

    struct sigaction sa_stop;
    sa_stop.sa_handler = handle_signal_usr2;
    sa_stop.sa_flags = 0;
    sigemptyset(&sa_stop.sa_mask);
    if (sigaction(SIGUSR2, &sa_stop, NULL) == -1) {
        perror("Error setting SIGUSR2 handler");
        exit(EXIT_FAILURE);
    }
}

void wait_for_signal() {
    pause();
}

void stop_monitor() {
    stop_requested = 1;
}

int main() {
    printf("Monitor process started. Waiting for commands...\n");

    FILE *resp_file = fopen("response.txt", "w");
    if (!resp_file) {
        perror("Failed to create response file");
        exit(EXIT_FAILURE);
    }
    fclose(resp_file);

    setup_signal_handlers();

    while (!stop_requested) {
        wait_for_signal();
    }

    printf("Monitor process stopping...\n");
    usleep(500000);
    printf("Monitor process stopped.\n");

    stop_monitor();
    return 0;
}
