CC = gcc
CFLAGS = -g -Wall -std=c99 -fsanitize=address -O2

spchk: spchk.o 
	$(CC) $(CFLAGS) spchk.o -o spchk

clean:
	rm -f 
