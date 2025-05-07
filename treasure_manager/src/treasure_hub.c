#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CMD_FILE "command.txt"
#define RESPONSE_FILE "response.txt"

pid_t monitor_pid = -1;

void start_monitor() {
    if (monitor_pid != -1) {
        printf("Monitor is already running.\n");
        return;
    }

    monitor_pid = fork();
    if (monitor_pid == 0) {
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
    printf("Stopping monitor...\n");
    waitpid(monitor_pid, NULL, 0);

    monitor_pid = -1;
    printf("Monitor stopped.\n");
}

void send_command(const char *command) {
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

    usleep(100000);

    FILE *resp_file = fopen(RESPONSE_FILE, "r");
    if (!resp_file) {
        perror("Failed to open response file");
        return;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), resp_file)) {
        printf("%s", buffer);
    }
    fclose(resp_file);
}

void handle_exit() {
    if (monitor_pid != -1) {
        printf("Error: Monitor is still running. Stop it first.\n");
        return;
    }
    printf("Exiting treasure_hub.\n");
    exit(0);
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

        if (strcmp(command, "start_monitor") == 0) {
            start_monitor();
        } else if (strcmp(command, "stop_monitor") == 0) {
            stop_monitor();
        } else if (strcmp(command, "list_hunts") == 0 || strcmp(command, "list_treasures") == 0 || strcmp(command, "view_treasure") == 0) {
            send_command(command);
        } else if (strcmp(command, "exit") == 0) {
            handle_exit();
        } else if (strcmp(command, "help") == 0) {
            printf("Available commands:\n");
            printf("  start_monitor - Start the monitor process.\n");
            printf("  stop_monitor - Stop the monitor process.\n");
            printf("  list_hunts - List all hunts.\n");
            printf("  list_treasures - List all treasures in a hunt.\n");
            printf("  view_treasure - View details of a specific treasure.\n");
            printf("  exit - Exit the treasure_hub.\n");
            printf("  help - Display this help message.\n");
        } else {
            printf("Unknown command. Available commands: start_monitor, stop_monitor, list_hunts, list_treasures, view_treasure, exit.\n");
        }
    }

    return 0;
}
