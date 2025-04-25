//since i am using a windows machine but i want this project to work on linux too
//i am using ifdef in order to be able to test it on both platforms

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <process.h>   // For CreateProcess
#else
#include <unistd.h>    // For usleep
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>  // For waitpid
#endif

#define CMD_FILE "command.txt"
#define RESPONSE_FILE "response.txt"

pid_t monitor_pid = -1;

#ifdef _WIN32
// Windows-specific variables for events
HANDLE monitor_event = NULL;
PROCESS_INFORMATION monitor_process_info;
#endif

void start_monitor() {
    if (monitor_pid != -1) {
        printf("Monitor is already running.\n");
        return;
    }

#ifdef _WIN32
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    if (!CreateProcess(
            "monitor.exe",    // Ensure this is the correct path
            NULL,             // Command line arguments
            NULL,             // Process security attributes
            NULL,             // Thread security attributes
            FALSE,            // Inherit handles
            0,                // Creation flags
            NULL,             // Environment
            NULL,             // Current directory
            &si,              // Startup info
            &pi               // Process information
    )) {
        fprintf(stderr, "Failed to start monitor process: %lu\n", GetLastError());
        return;
    }
    monitor_pid = pi.dwProcessId;
    printf("Monitor started with PID %d.\n", monitor_pid);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
#else
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
#endif
}

void stop_monitor() {
    if (monitor_pid == -1) {
        printf("No monitor is running.\n");
        return;
    }

#ifdef _WIN32
    // Windows: Stop the monitor by setting the event and terminating the process
    SetEvent(monitor_event);  // Signal the monitor to stop
    TerminateProcess(monitor_process_info.hProcess, 0);  // Terminate monitor process
    CloseHandle(monitor_event);  // Close the event handle

    printf("Stopping monitor...\n");
#else
    // Linux: Send SIGUSR2 to stop the monitor process
    kill(monitor_pid, SIGUSR2);
    printf("Stopping monitor...\n");
    waitpid(monitor_pid, NULL, 0);
#endif

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
    printf("Command written to command.txt: %s\n", command); // Debug print

#ifdef _WIN32
    SetEvent(monitor_event);
#else
    if (kill(monitor_pid, SIGUSR1) == -1) {
        perror("Failed to send SIGUSR1");
    } else {
        printf("SIGUSR1 sent to monitor process (PID: %d).\n", monitor_pid); // Debug print
    }
#endif
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

        command[strcspn(command, "\n")] = '\0'; // Remove newline

        if (strcmp(command, "start_monitor") == 0) {
            start_monitor();
        } else if (strcmp(command, "stop_monitor") == 0) {
            stop_monitor();
        } else if (strcmp(command, "list_hunts") == 0 || strcmp(command, "list_treasures") == 0 || strcmp(command, "view_treasure") == 0) {
            send_command(command);
        } else if (strcmp(command, "exit") == 0) {
            handle_exit();
        } else {
            printf("Unknown command. Available commands: start_monitor, stop_monitor, list_hunts, list_treasures, view_treasure, exit.\n");
        }
    }

    return 0;
}
