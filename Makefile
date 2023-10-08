CC=gcc
CFLAGS=-lncurses -lpthread

keyboard_test: keyboard_test.o
	$(CC) -o keyboard_test keyboard_test.o $(CFLAGS)
	echo "Compiled app."

