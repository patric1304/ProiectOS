#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treasure_hunt.h"
#include "file_operations.h"
#include "logger.h"
#include "utils/utils.h"

void display_help() {
    printf("Usage: treasure_manager [command] [options]\n");
    printf("Commands:\n");
    printf("  create_hunt       <hunt_id>                          Create a new treasure hunt\n");
    printf("  add_treasure      <hunt_id> <id> <username> <lat> <lon> <desc> <value>  Add a treasure to a hunt\n");
    printf("  add_user          <hunt_id> <treasure_id> <username> Add a user to a treasure\n");
    printf("  remove_treasure   <hunt_id> <treasure_id>            Remove a treasure from a hunt\n");
    printf("  help                                                  Display this help message\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: treasure_manager <command> [arguments]\n");
        display_help();
        return 1;
    }

    const char *command = argv[1];

    if (strcmp(command, "create_hunt") == 0 && argc == 3) {
        create_hunt(argv[2]);
    } else if (strcmp(command, "add_treasure") == 0 && argc == 9) {
        Treasure treasure;
        strncpy(treasure.id, argv[3], sizeof(treasure.id) - 1);
        treasure.id[sizeof(treasure.id) - 1] = '\0'; // Ensure null-termination

        // Initialize the first user in the users array
        strncpy(treasure.users[0], argv[4], sizeof(treasure.users[0]) - 1);
        treasure.users[0][sizeof(treasure.users[0]) - 1] = '\0'; // Ensure null-termination

        // Set the user_count to 1 since we are adding the first user
        treasure.user_count = 1;

        treasure.latitude = atof(argv[5]);
        treasure.longitude = atof(argv[6]);
        strncpy(treasure.description, argv[7], sizeof(treasure.description) - 1);
        treasure.description[sizeof(treasure.description) - 1] = '\0'; // Ensure null-termination
        treasure.value = atoi(argv[8]);

        add_treasure(argv[2], treasure);
    } else if (strcmp(command, "add_user") == 0 && argc == 5) {
        const char *hunt_id = argv[2];
        const char *treasure_id = argv[3];
        const char *username = argv[4];
        add_user_to_treasure(hunt_id, treasure_id, username);
    } else if (strcmp(command, "remove_treasure") == 0 && argc == 4) {
        const char *hunt_id = argv[2];
        const char *treasure_id = argv[3];
        remove_treasure(hunt_id, treasure_id);
    } else if (strcmp(command, "help") == 0) {
        display_help();
    } else {
        printf("Unknown command or invalid arguments.\n");
        display_help();
    }

    return 0;
}