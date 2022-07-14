CC = gcc
NAME = hw04
NAMESRC = hw04.c

CFLAGS = -Wall -Wextra -Wpedantic -std=c11

all:
	$(CC) $(CFLAGS) $(NAMESRC) -lcurl memory.h memory.c libs/cJSON.c libs/cJSON.h -o $(NAME)

clean:
	rm -f $(NAME)
