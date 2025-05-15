#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treasure_manager.h"
#include "file_operations.h"
#include "logger.h"
#include "utils/utils.h"


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: treasure_manager <command> [arguments]\n");
        display_help();
        return 1;
    }

    const char *command = argv[1];

    if (strcmp(command, "create_hunt") == 0 && argc == 3) {
        create_hunt(argv[2]);
    } else if (strcmp(command, "add_treasure") == 0) {
        if (argc < 9) { 
            printf("Insufficient arguments for add_treasure command.\n");
            display_help();
            return 1;
        }

        const char *hunt_id = argv[2];
        const char *treasure_id = argv[3];

        int userNumber = argc - 8;
        if (userNumber > MAX_USERS) {
            printf("Too many users. Maximum allowed is %d.\n", MAX_USERS);
            return 1;
        }

        char users[MAX_USERS][MAX_LENGTH];
        for (int i = 0; i < userNumber; i++) {
            strncpy(users[i], argv[4 + i], MAX_LENGTH - 1);
            users[i][MAX_LENGTH - 1] = '\0'; 
        }

        Treasure treasure;
        strncpy(treasure.id, treasure_id, sizeof(treasure.id) - 1);
        treasure.id[sizeof(treasure.id) - 1] = '\0';
        treasure.latitude = atof(argv[4 + userNumber]);
        treasure.longitude = atof(argv[5 + userNumber]);
        strncpy(treasure.description, argv[6 + userNumber], sizeof(treasure.description) - 1);
        treasure.description[sizeof(treasure.description) - 1] = '\0';
        treasure.value = atoi(argv[7 + userNumber]);
        add_treasure(hunt_id, treasure, userNumber, users);
    } else if (strcmp(command, "add_user") == 0 && argc == 5) {
        const char *hunt_id = argv[2];
        const char *treasure_id = argv[3];
        const char *username = argv[4];
        add_user_to_treasure(hunt_id, treasure_id, username);
    } else if (strcmp(command, "remove_treasure") == 0 && argc == 4) {
        const char *hunt_id = argv[2];
        const char *treasure_id = argv[3];
        remove_treasure(hunt_id, treasure_id);
    } else if (strcmp(command, "remove_hunt") == 0 && argc == 3) {
        const char *hunt_id = argv[2];
        remove_hunt(hunt_id);
    } else if (strcmp(command, "view_treasure") == 0 && argc == 4) {
        const char *hunt_id = argv[2];
        const char *treasure_id = argv[3];
        view(hunt_id, treasure_id);
    } else if (strcmp(command, "list_treasures") == 0 && argc == 3) {
        const char *hunt_id = argv[2];
        list(hunt_id);
    } else if (strcmp(command, "help") == 0) {
        display_help();
    } else {
        printf("Unknown command or invalid arguments.\n");
        display_help();
    }

    return 0;
}