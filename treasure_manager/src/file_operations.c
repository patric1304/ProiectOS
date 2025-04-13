#define _WIN32_WINNT 0x0600 // Enable CreateSymbolicLinkA support

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
        #ifdef _WIN32
        if (_mkdir(path) != 0) { 
            perror("Error creating directory");
            return -1;
        }
        #else
        if (mkdir(path, 0777) != 0) {
            perror("Error creating directory");
            return -1;
        }
        #endif
    }
    return 0;
}

#ifdef _WIN32
#include <windows.h>
#include <stdio.h>

int create_symlink(const char *target, const char *link) {
    DWORD flags = 0;

    DWORD attributes = GetFileAttributesA(target);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        fprintf(stderr, "Target does not exist: %s\n", target);
        return -1;
    }

    if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
        flags = SYMBOLIC_LINK_FLAG_DIRECTORY;
    }

    if (!CreateSymbolicLinkA(link, target, flags)) {
        DWORD err = GetLastError();
        fprintf(stderr, "Failed to create symbolic link (error code: %lu). Run as administrator or enable developer mode.\n", err);
        return -1;
    }

    return 0;
}

#else 

#include <unistd.h>
#include <stdio.h>

int create_symlink(const char *target, const char *link) {
    if (symlink(target, link) != 0) {
        perror("Failed to create symbolic link");
        return -1;
    }
    return 0;
}
#endif

