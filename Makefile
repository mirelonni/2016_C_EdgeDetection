CC = gcc
CFLAGS = -Wall -g

build:imgp.c
	$(CC) imgp.c -o imgp $(CFLAGS)

clean:
	rm -f imgp *.bmp *.o
