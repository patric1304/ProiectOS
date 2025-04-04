#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"

#define LOG_FILE "logs/treasure_manager.log"

// Function to initialize the logger
void init_logger() {
    FILE *file = fopen(LOG_FILE, "a");
    if (file == NULL) {
        perror("Failed to open log file");
        exit(EXIT_FAILURE);
    }
    fclose(file);
}

// Function to log a message
void log_message(const char *message) {
    FILE *file = fopen(LOG_FILE, "a");
    if (file == NULL) {
        perror("Failed to open log file");
        return;
    }
    fprintf(file, "%s\n", message);
    fclose(file);
}

// Function to log an error message
void log_error(const char *error_message) {
    log_message(error_message);
}

// Function to log an operation with a specific action
void log_operation(const char *action, const char *details) {
    char log_entry[256];
    snprintf(log_entry, sizeof(log_entry), "Action: %s, Details: %s", action, details);
    log_message(log_entry);
}

// Function to log an action to a specific log file
void log_action(const char *log_file, const char *action) {
    FILE *file = fopen(log_file, "a");
    if (file) {
        fprintf(file, "%s\n", action);
        fclose(file);
    } else {
        perror("Error writing to log file");
    }
}