#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>  
#include <unistd.h>    
#include "logger.h"


#define LOG_FILE "logs/treasure_manager.log"

void init_logger() {

    struct stat st = {0};
    if (stat("logs", &st) == -1) {
        if (mkdir("logs", 0777) != 0) { // Create the logs directory with full permissions
            perror("Failed to create logs directory");
            exit(EXIT_FAILURE);
        }
    }

    FILE *file = fopen(LOG_FILE, "a");
    if (file == NULL) {
        perror("Failed to open log file");
        exit(EXIT_FAILURE);
    }
    fclose(file);
}

void log_message(const char *message) {
    FILE *file = fopen(LOG_FILE, "a");
    if (file == NULL) {
        perror("Failed to open log file");
        return;
    }
    fprintf(file, "%s\n", message);
    fclose(file);
}

void log_error(const char *error_message) {
    log_message(error_message);
}


void log_operation(const char *action, const char *details) {
    char log_entry[256];
    snprintf(log_entry, sizeof(log_entry), "Action: %s, Details: %s", action, details);
    log_message(log_entry);
}

void log_action(const char *log_file, const char *action) {
    FILE *file = fopen(log_file, "a");
    if (file) {
        fprintf(file, "%s\n", action);
        fclose(file);
    } else {
        perror("Error writing to log file");
    }
}
