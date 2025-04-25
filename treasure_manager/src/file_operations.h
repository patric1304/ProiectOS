#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


int read_treasure_file(const char *filename, char *buffer, size_t size);

int write_treasure_file(const char *filename, const char *data);

int delete_treasure_file(const char *filename);

int create_directory(const char *path);

int create_symlink(const char *target, const char *link);

#endif 
