#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

int validate_treasure_name(const char *name) {
    if (name == NULL || strlen(name) == 0 || strlen(name) > 100) {
        return 0; 
    }
    return 1;
}
void handle_error(const char *message) {
    fprintf(stderr, "Error: %s\n", message);
    exit(EXIT_FAILURE);
}

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
