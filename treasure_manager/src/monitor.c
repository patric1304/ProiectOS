//since i am using a windows machine but i want this project to work on linux too
//i am using ifdef in order to be able to test it on both platforms


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> 
#include "treasure_hunt.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <signal.h>
#include <unistd.h>
#endif

#define CMD_FILE "command.txt"
#define RESPONSE_FILE "response.txt"

#ifdef _WIN32
volatile int stop_requested = 0;
HANDLE event_handle;
#else
volatile sig_atomic_t stop_requested = 0;
#endif

void handle_signal() {
    
    FILE *cmd_file = fopen(CMD_FILE, "r");
    if (!cmd_file) {
        perror("Failed to open command file");
        return;
    }

    char command[256];
    if (fgets(command, sizeof(command), cmd_file)) {
        command[strcspn(command, "\n")] = '\0';
        FILE *resp_file = fopen(RESPONSE_FILE, "w");
        if (!resp_file) {
            perror("Failed to open response file");
            fclose(cmd_file);
            return;
        }

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
            char hunt_id[256];
            printf("Enter hunt ID: ");
            scanf("%s", hunt_id);
            list(hunt_id);
        } else if (strcmp(command, "view_treasure") == 0) {
            fprintf(resp_file, "Viewing a specific treasure...\n");
            char hunt_id[256], treasure_id[256];
            printf("Enter hunt ID: ");
            scanf("%s", hunt_id);
            printf("Enter treasure ID: ");
            scanf("%s", treasure_id);
            view(hunt_id, treasure_id);
        } else {
            fprintf(resp_file, "Unknown command: %s\n", command);
        }

        fclose(resp_file);
    }

    fclose(cmd_file);
}

#ifdef _WIN32
void setup_event() {
    event_handle = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (event_handle == NULL) {
        perror("CreateEvent failed");
        exit(EXIT_FAILURE);
    }
}

void wait_for_event() {
    DWORD result = WaitForSingleObject(event_handle, INFINITE);

    if (result == WAIT_OBJECT_0) {
        handle_signal();
        ResetEvent(event_handle);
    }
}

void stop_monitor() {
    stop_requested = 1;
    CloseHandle(event_handle);
}
#else
void handle_signal_usr1(int sig) {
    if (sig == SIGUSR1) {
        handle_signal();
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
    sa_stop.sa_handler = SIG_IGN;
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
#endif

int main() {
    printf("Monitor process started. Waiting for commands...\n");

    FILE *resp_file = fopen(RESPONSE_FILE, "w");
    if (!resp_file) {
        perror("Failed to create response file");
        exit(EXIT_FAILURE);
    }
    fclose(resp_file);

#ifdef _WIN32
    setup_event();
#else
    setup_signal_handlers();
#endif

    while (!stop_requested) {
#ifdef _WIN32
        wait_for_event();
#else
        wait_for_signal();
#endif
    }

    printf("Monitor process stopping...\n");

#ifdef _WIN32
    Sleep(500);
#else
    usleep(500000);
#endif

    printf("Monitor process stopped.\n");

    stop_monitor();
    return 0;
}
