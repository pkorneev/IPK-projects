CC = gcc
CFLAGS = -std=gnu99 -g -pedantic -Wall -Wextra -02
FILES = hinfosvc.c

hinfosvc: $(FILES)
	$(CC) $(FILES) -o $@

