CFLAGS = -Wall -c99

all:
	$(CC) $(CFLAGS) vice.c test.c -o vice
