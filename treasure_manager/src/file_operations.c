#define _WIN32_WINNT 0x0600 // Enable CreateSymbolicLinkA support

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h> // For symlink
#include "file_operations.h"

int write_treasure_data(const char *filename, const char *data) {
    FILE *fp = fopen(filename, "ab");  
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

int read_treasure_data(const char *filename) {
    char buffer[256];
    FILE *fp = fopen(filename, "rb"); 
    if (fp == NULL) {
        perror("Failed to open file for reading");
        return -1;
    }
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer) - 1, fp)) > 0) {
        buffer[bytesRead] = '\0'; 
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

int delete_treasure_file(const char *filename) {
    if (remove(filename) != 0) {
        perror("Failed to delete file");
        return -1;
    }
    return 0;
}

int create_directory(const char *path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) { 
        if (mkdir(path, 0777) != 0) {
            perror("Error creating directory");
            return -1;
        }
    }
    return 0;
}

int create_symlink(const char *target, const char *link) {
    if (symlink(target, link) != 0) {
        perror("Failed to create symbolic link");
        return -1;
    }
    return 0;
}
