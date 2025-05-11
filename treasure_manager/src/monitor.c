#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/wait.h>
#include "treasure_hunt.h"

#define CMD_FILE "command.txt"

void handle_signal(int signal) {
    if (signal == SIGUSR1) {
        char command[256];
        FILE *cmd_file = fopen(CMD_FILE, "r");
        if (!cmd_file) {
            perror("Failed to open command file");
            return;
        }

        if (fgets(command, sizeof(command), cmd_file)) {
            command[strcspn(command, "\n")] = '\0'; // Remove newline

            // Parse the command
            char cmd[256], arg1[256], arg2[256], arg3[256];
            int num_args = sscanf(command, "%s %s %s %s", cmd, arg1, arg2, arg3);

            char response[1024] = {0}; // Buffer for the response

            if (strcmp(cmd, "create_hunt") == 0 && num_args == 2) {
                create_hunt(arg1);
                snprintf(response, sizeof(response), "Hunt '%s' created successfully.\n", arg1);
            } else if (strcmp(cmd, "add_treasure") == 0 && num_args >= 4) {
                Treasure treasure;
                strncpy(treasure.id, arg2, MAX_LENGTH - 1);
                treasure.id[MAX_LENGTH - 1] = '\0';
                treasure.value = atoi(arg3);

                char users[MAX_USERS][MAX_LENGTH] = {0}; // No users initially
                add_treasure(arg1, treasure, 0, users);
                snprintf(response, sizeof(response), "Treasure '%s' added to hunt '%s'.\n", arg2, arg1);
            } else if (strcmp(cmd, "list_treasures") == 0 && num_args == 2) {
                int temp_fd[2];
                pipe(temp_fd);
                pid_t pid = fork();
                if (pid == 0) {
                    dup2(temp_fd[1], STDOUT_FILENO);
                    close(temp_fd[0]);
                    close(temp_fd[1]);
                    list(arg1);
                    exit(0);
                } else {
                    close(temp_fd[1]);
                    read(temp_fd[0], response, sizeof(response) - 1);
                    close(temp_fd[0]);
                    waitpid(pid, NULL, 0);
                }
            } else if (strcmp(cmd, "view_treasure") == 0 && num_args == 3) {
                int temp_fd[2];
                pipe(temp_fd);
                pid_t pid = fork();
                if (pid == 0) {
                    dup2(temp_fd[1], STDOUT_FILENO);
                    close(temp_fd[0]);
                    close(temp_fd[1]);
                    view(arg1, arg2);
                    exit(0);
                } else {
                    close(temp_fd[1]);
                    read(temp_fd[0], response, sizeof(response) - 1);
                    close(temp_fd[0]);
                    waitpid(pid, NULL, 0);
                }
            } else if (strcmp(cmd, "remove_hunt") == 0 && num_args == 2) {
                remove_hunt(arg1);
                snprintf(response, sizeof(response), "Hunt '%s' removed successfully.\n", arg1);
            } else if (strcmp(cmd, "add_user_to_treasure") == 0 && num_args == 4) {
                add_user_to_treasure(arg1, arg2, arg3);
                snprintf(response, sizeof(response), "User '%s' added to treasure '%s' in hunt '%s'.\n", arg3, arg2, arg1);
            } else if (strcmp(cmd, "help") == 0) {
                // Redirect the output of help to the response buffer
                int temp_fd[2];
                pipe(temp_fd);
                pid_t pid = fork();
                if (pid == 0) {
                    // Child process
                    dup2(temp_fd[1], STDOUT_FILENO); // Redirect stdout to the pipe
                    close(temp_fd[0]);
                    close(temp_fd[1]);
                    help(); // Call the help method
                    exit(0);
                } else {
                    // Parent process
                    close(temp_fd[1]);
                    read(temp_fd[0], response, sizeof(response) - 1);
                    close(temp_fd[0]);
                    waitpid(pid, NULL, 0);
                }
            } else {
                snprintf(response, sizeof(response), "Unknown command: %s\n", command);
            }

            // Write response to stdout (redirected pipe)
            write(STDOUT_FILENO, response, strlen(response));
        }

        fclose(cmd_file);
    } else if (signal == SIGUSR2) {
        printf("Monitor stopping...\n");
        exit(0);
    }
}

int main() {
    signal(SIGUSR1, handle_signal);
    signal(SIGUSR2, handle_signal);

    printf("Monitor started. Waiting for commands...\n");

    while (1) {
        pause(); // Wait for signals
    }

    return 0;
}
