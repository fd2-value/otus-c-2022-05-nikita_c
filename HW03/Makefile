CC = gcc
NAME = hw03
NAMESRC = hw03.c

CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -D_GNU_SOURCE

all:
	$(CC) $(CFLAGS) $(NAMESRC) hashtable.h hashtable.c -o $(NAME)

clean:
	rm -f $(NAME)
