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
#define FIFO_PATH "monitor_pipe"

int fifo_fd = -1; // Global file descriptor for the FIFO

void handle_signal(int signal) {
    if (signal == SIGUSR1) {
        char command[256];
        FILE *cmd_file = fopen(CMD_FILE, "r");
        if (!cmd_file) {
            perror("Failed to open command file");
            return;
        }

        if (fgets(command, sizeof(command), cmd_file)) {
            command[strcspn(command, "\n")] = '\0';
            char cmd[256], arg1[256], arg2[256], arg3[256], arg4[256];
            int num_args = sscanf(command, "%s %s %s %s %s", cmd, arg1, arg2, arg3, arg4);

            char response[1024] = {0}; // Buffer for the response

            if (strcmp(cmd, "create_hunt") == 0 && num_args == 2) {
                create_hunt(arg1);
                snprintf(response, sizeof(response), "Hunt '%s' created successfully.\n", arg1);
            } else if (strcmp(cmd, "add_treasure") == 0) {
                Treasure treasure;
                char users[MAX_USERS][MAX_LENGTH] = {0};
                int user_count = 0;

                char *token = strtok(command, " ");
                int arg_index = 0;
                char *args[256]; 

                while (token != NULL && arg_index < 256) {
                    args[arg_index++] = token;
                    token = strtok(NULL, " ");
                }

                if (arg_index < 8) { 
                    snprintf(response, sizeof(response), "Invalid add_treasure command format.\n");
                    if (fifo_fd != -1) {
                        write(fifo_fd, response, strlen(response));
                    }
                    return;
                }

                strncpy(treasure.id, args[2], MAX_LENGTH - 1);
                treasure.id[MAX_LENGTH - 1] = '\0';
                int i = 3;
                while (i < arg_index - 4 && user_count < MAX_USERS) {
                    strncpy(users[user_count], args[i], MAX_LENGTH - 1);
                    users[user_count][MAX_LENGTH - 1] = '\0';
                    user_count++;
                    i++;
                }

                treasure.latitude = atof(args[i++]);
                treasure.longitude = atof(args[i++]);
                strncpy(treasure.description, args[i++], MAX_LENGTH - 1);
                treasure.description[MAX_LENGTH - 1] = '\0';
                treasure.value = atoi(args[i]);

                add_treasure(args[1], treasure, user_count, users);
                snprintf(response, sizeof(response),
                         "Treasure '%s' added to hunt '%s' with value %d, description '%s', latitude %.2f, longitude %.2f, and %d users.\n",
                         treasure.id, args[1], treasure.value, treasure.description, treasure.latitude, treasure.longitude, user_count);

            } else if (strcmp(cmd, "remove_treasure") == 0 && num_args == 3) {
                remove_treasure(arg1, arg2);
                snprintf(response, sizeof(response), "Treasure '%s' removed from hunt '%s'.\n", arg2, arg1);

            } else if (strcmp(cmd, "add_user") == 0 && num_args == 4) {
                add_user_to_treasure(arg1, arg2, arg3);
                snprintf(response, sizeof(response), "User '%s' added to treasure '%s' in hunt '%s'.\n", arg3, arg2, arg1);
                
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

            } else if (strcmp(cmd, "list_hunts") == 0) {
                int temp_fd[2];
                pipe(temp_fd);
                pid_t pid = fork();
                if (pid == 0) {
                    dup2(temp_fd[1], STDOUT_FILENO);
                    close(temp_fd[0]);
                    close(temp_fd[1]);
                    list_hunts(); 
                    exit(0);
                } else {
                    close(temp_fd[1]);
                    read(temp_fd[0], response, sizeof(response) - 1);
                    close(temp_fd[0]);
                    waitpid(pid, NULL, 0);
                }

            } else if (strcmp(cmd, "help") == 0) {
                // Redirect the output of help to the response buffer
                int temp_fd[2];
                pipe(temp_fd);
                pid_t pid = fork();
                if (pid == 0) {
                    // Child process
                    dup2(temp_fd[1], STDOUT_FILENO); 
                    close(temp_fd[0]);
                    close(temp_fd[1]);
                    help(); 
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

            if (fifo_fd != -1) {
                write(fifo_fd, response, strlen(response));
            }
        }

        fclose(cmd_file);
    } else if (signal == SIGUSR2) {
        printf("Monitor stopping...\n");
        if (fifo_fd != -1) close(fifo_fd);
        exit(0);
    }
}

int main() {
    // Open the FIFO for writing and keep it open
    fifo_fd = open(FIFO_PATH, O_WRONLY);
    if (fifo_fd == -1) {
        perror("Failed to open FIFO for writing at startup");
        exit(EXIT_FAILURE);
    }

    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("Monitor started. Waiting for commands...\n");

    while (1) {
        pause();
    }

    if (fifo_fd != -1) close(fifo_fd);
    return 0;
}
