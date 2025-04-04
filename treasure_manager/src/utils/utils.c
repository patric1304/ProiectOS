#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

// Function to validate user input for treasure names
int validate_treasure_name(const char *name) {
    if (name == NULL || strlen(name) == 0 || strlen(name) > 100) {
        return 0; // Invalid name
    }
    return 1; // Valid name
}

// Function to handle error messages
void handle_error(const char *message) {
    fprintf(stderr, "Error: %s\n", message);
    exit(EXIT_FAILURE);
}

// Function to clear input buffer
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}