CC = gcc
CFLAGS = -fPIC -c

build:
	$(CC) $(CFLAGS) so_stdio.c -o so_stdio.o
	$(CC) -shared so_stdio.o -o libso_stdio.so
clean:
	rm -f *.so *.o
