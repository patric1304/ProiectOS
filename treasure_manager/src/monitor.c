#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include "treasure_hunt.h"

#define CMD_FILE "command.txt"

volatile sig_atomic_t stop_requested = 0;
int pipe_fd[2]; // Pipe for communication

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

        char response[1024] = {0}; // Buffer to store the response

        if (strcmp(command, "list_hunts") == 0) {
            snprintf(response, sizeof(response), "Listing all hunts...\n");
            DIR *dir = opendir("../hunt");
            if (!dir) {
                strncat(response, "Failed to open hunt directory.\n", sizeof(response) - strlen(response) - 1);
            } else {
                struct dirent *entry;
                while ((entry = readdir(dir)) != NULL) {
                    if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                        strncat(response, entry->d_name, sizeof(response) - strlen(response) - 1);
                        strncat(response, "\n", sizeof(response) - strlen(response) - 1);
                    }
                }
                closedir(dir);
            }
        } else if (strcmp(command, "list_treasures") == 0) {
            snprintf(response, sizeof(response), "Listing all treasures...\n");

            char hunt_id[256];
            printf("Enter hunt ID: ");
            if (scanf("%255s", hunt_id) != 1) {
                strncat(response, "Error: Failed to read hunt ID from keyboard.\n", sizeof(response) - strlen(response) - 1);
            } else {
                list(hunt_id); // Call the list function
                strncat(response, "Treasures listed successfully.\n", sizeof(response) - strlen(response) - 1);
            }
        } else if (strcmp(command, "view_treasure") == 0) {
            snprintf(response, sizeof(response), "Viewing a specific treasure...\n");
            char hunt_id[256], treasure_id[256];
            printf("Enter hunt ID: ");
            scanf("%s", hunt_id);
            printf("Enter treasure ID: ");
            scanf("%s", treasure_id);
            view(hunt_id, treasure_id);
            strncat(response, "Treasure viewed successfully.\n", sizeof(response) - strlen(response) - 1);
        } else if (strcmp(command, "help") == 0) {
            snprintf(response, sizeof(response),
                     "Available commands:\n"
                     "  list_hunts       - List all hunts\n"
                     "  list_treasures   - List all treasures in a hunt\n"
                     "  view_treasure    - View details of a specific treasure\n"
                     "  help             - Display this help message\n");
        } else {
            snprintf(response, sizeof(response), "Unknown command: %s\n", command);
        }

        // Send the response through the pipe
        write(pipe_fd[1], response, strlen(response));
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

int main() {
    printf("Monitor process started. Waiting for commands...\n");

    if (pipe(pipe_fd) == -1) {
        perror("Failed to create pipe");
        exit(EXIT_FAILURE);
    }

    setup_signal_handlers();

    while (!stop_requested) {
        wait_for_signal();
    }

    close(pipe_fd[0]);
    close(pipe_fd[1]);

    printf("Monitor process stopping...\n");
    usleep(500000);
    printf("Monitor process stopped.\n");

    return 0;
}