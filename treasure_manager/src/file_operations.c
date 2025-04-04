#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <direct.h>
#include "file_operations.h"

// Function to write treasure data to a file
int write_treasure_data(const char *filename, const char *data) {
    FILE *fp = fopen(filename, "ab");  // Open for appending in binary mode
    if (fp == NULL) {
        perror("Failed to open file for writing");
        return -1;
    }
    if (fwrite(data, 1, strlen(data), fp) != strlen(data)) {
        perror("Failed to write data to file");
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return 0;
}

// Function to read treasure data from a file
int read_treasure_data(const char *filename) {
    char buffer[256];
    FILE *fp = fopen(filename, "rb");  // Open for reading in binary mode
    if (fp == NULL) {
        perror("Failed to open file for reading");
        return -1;
    }
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer) - 1, fp)) > 0) {
        buffer[bytesRead] = '\0'; // Null-terminate the buffer
        printf("%s", buffer);
    }
    if (ferror(fp)) {
        perror("Failed to read data from file");
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return 0;
}

// Function to delete a treasure data file
int delete_treasure_file(const char *filename) {
    if (remove(filename) != 0) {
        perror("Failed to delete file");
        return -1;
    }
    return 0;
}

// Function to create a directory
int create_directory(const char *path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) { // Check if the directory exists
        if (_mkdir(path) != 0) { // Create the directory
            perror("Error creating directory");
            return -1;
        }
    }
    return 0;
}

// Function to create a symbolic link
int create_symlink(const char *target, const char *link) {
    char command[512];
    snprintf(command, sizeof(command), "mklink /D \"%s\" \"%s\"", link, target);
    if (system(command) != 0) { // Execute the command
        perror("Error creating symbolic link");
        return -1;
    }
    return 0;
}
