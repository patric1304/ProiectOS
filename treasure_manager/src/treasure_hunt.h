#ifndef TREASURE_HUNT_H
#define TREASURE_H
#define MAX_LENGTH 256
#define MAX_USERS 10 // Maximum number of users per treasure

#include "file_operations.h"

typedef struct {
    char id[MAX_LENGTH];
    float latitude;
    float longitude;
    char description[MAX_LENGTH];
    int value;
    char users[MAX_USERS][MAX_LENGTH]; // Array to store additional users
    int user_count; // Number of users associated with the treasure
} Treasure;

void create_hunt(const char *hunt_id);
void add_treasure(const char *hunt_id, Treasure treasure, int userNumber, char users[][MAX_LENGTH]);
void add_user_to_treasure(const char *hunt_id, const char *treasure_id, const char *username);
void remove_treasure(const char *hunt_id, const char *treasure_id);	
void list(const char *hunt_id);
void view(const char *hunt_id, const char *treasure_id);
void remove_hunt(const char *hunt_id);
#endif // TREASURE_HUNT_H