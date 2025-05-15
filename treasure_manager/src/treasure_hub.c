#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CMD_FILE "command.txt"

pid_t monitor_pid = -1;

void start_monitor() {
    if (monitor_pid != -1) {
        printf("Monitor is already running.\n");
        return;
    }

    monitor_pid = fork();
    if (monitor_pid == 0) {
        // Child process: Start the monitor
        execl("./monitor", "./monitor", NULL);
        perror("Failed to start monitor");
        exit(EXIT_FAILURE);
    } else if (monitor_pid > 0) {
        printf("Monitor started with PID %d.\n", monitor_pid);
    } else {
        perror("Failed to fork");
    }
}

void stop_monitor() {
    if (monitor_pid == -1) {
        printf("No monitor is running.\n");
        return;
    }

    kill(monitor_pid, SIGUSR2);
    waitpid(monitor_pid, NULL, 0);
    monitor_pid = -1;
    printf("Monitor stopped.\n");
}

void send_command_to_monitor(const char *command) {
    if (monitor_pid == -1) {
        printf("Monitor is not running.\n");
        return;
    }

    FILE *cmd_file = fopen(CMD_FILE, "w");
    if (!cmd_file) {
        perror("Failed to open command file");
        return;
    }
    fprintf(cmd_file, "%s\n", command);
    fclose(cmd_file);

    kill(monitor_pid, SIGUSR1);

    printf("Command sent to monitor. Check the monitor output terminal.\n");
}

void calculate_score(const char *hunt_id) {
    int score_pipe[2];
    if (pipe(score_pipe) == -1) {
        perror("Failed to create pipe");
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {
        dup2(score_pipe[1], STDOUT_FILENO);
        close(score_pipe[0]);
        close(score_pipe[1]);
        execl("./calculate_score", "./calculate_score", hunt_id, NULL);
        perror("Failed to execute calculate_score");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        close(score_pipe[1]);
        char buffer[256];
        ssize_t bytes_read;
        while ((bytes_read = read(score_pipe[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';
            printf("%s", buffer);
        }
        close(score_pipe[0]);
        waitpid(pid, NULL, 0);
    } else {
        perror("Failed to fork");
    }
}

void list_hunts() {
    if (monitor_pid == -1) {
        printf("Monitor is not running.\n");
        return;
    }
    send_command_to_monitor("list_hunts");
}

void list_treasures() {
    char hunt_id[128];
    printf("Enter hunt ID: ");
    if (scanf("%127s", hunt_id) != 1) {
        printf("Invalid input.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "list_treasures %s", hunt_id);
    send_command_to_monitor(cmd);
}

void view_treasure() {
    char hunt_id[128], treasure_id[128];
    printf("Enter hunt ID: ");
    if (scanf("%127s", hunt_id) != 1) {
        printf("Invalid input.\n");
        while (getchar() != '\n');
        return;
    }
    printf("Enter treasure ID: ");
    if (scanf("%127s", treasure_id) != 1) {
        printf("Invalid input.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');
    char cmd[256];
    int written = snprintf(cmd, sizeof(cmd), "view_treasure %s %s", hunt_id, treasure_id);
    if (written < 0 || written >= sizeof(cmd)) {
        printf("Command too long, cannot process.\n");
        return;
    }
    send_command_to_monitor(cmd);
}

int main() {
    char command[256];

    printf("Welcome to treasure_hub. Type 'help' for a list of commands.\n");

    while (1) {
        printf("> ");
        if (!fgets(command, sizeof(command), stdin)) {
            break;
        }

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "help") == 0) {
            if (monitor_pid != -1) {
                send_command_to_monitor("help");
            } else {
                printf("Available commands:\n");
                printf("  start_monitor       - Start the monitor process.\n");
                printf("  stop_monitor        - Stop the monitor process.\n");
                printf("  calculate_score     - Calculate the score for a specific hunt.\n");
                printf("  list_hunts          - List all hunts and the number of treasures in each.\n");
                printf("  list_treasures      - List all treasures in a hunt.\n");
                printf("  view_treasure       - Show info about a treasure in a hunt.\n");
                printf("  exit                - Exit the program.\n");
                printf("  <other commands>    - Send a custom command to the monitor.\n");
            }
        } else if (strcmp(command, "start_monitor") == 0) {
            start_monitor();
        } else if (strcmp(command, "stop_monitor") == 0) {
            stop_monitor();
        } else if (strcmp(command, "list_hunts") == 0) {
            list_hunts();
        } else if (strcmp(command, "list_treasures") == 0) {
            list_treasures();
        } else if (strcmp(command, "view_treasure") == 0) {
            view_treasure();
        } else if (strncmp(command, "calculate_score", 15) == 0) {
            char hunt_id[50];
            printf("Enter hunt ID: ");
            scanf("%s", hunt_id);
            calculate_score(hunt_id);
            while (getchar() != '\n'); // clear input buffer
        } else if (strcmp(command, "exit") == 0) {
            if (monitor_pid != -1) {
                printf("Error: Monitor is still running. Please stop the monitor before exiting.\n");
            } else {
                break;
            }
        } else {
            send_command_to_monitor(command);
        }
    }

    return 0;
}
