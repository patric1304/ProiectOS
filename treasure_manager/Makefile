CC = gcc
CFLAGS = -Wall -Wextra -I./src -I./src/utils
SRC = src/main.c src/treasure_hunt.c src/file_operations.c src/logger.c src/utils/utils.c
OBJ = $(SRC:.c=.o)
EXEC = treasure_manager

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)

.PHONY: all clean