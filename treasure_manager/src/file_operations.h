#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


// Function to read treasure data from a file
int read_treasure_file(const char *filename, char *buffer, size_t size);

// Function to write treasure data to a file
int write_treasure_file(const char *filename, const char *data);

// Function to delete a treasure data file
int delete_treasure_file(const char *filename);

// Function to create a directory
int create_directory(const char *path);

// Function to create a symbolic link
int create_symlink(const char *target, const char *link);

#endif // FILE_OPERATIONS_H