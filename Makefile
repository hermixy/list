
CC = gcc
CFLAGS = -Wall

BINS = list.so

all: $(BINS)

list.so: list.c
	$(CC) $(CFLAGS) -fPIC -shared -o list.so list.c

install:
	cp list.so /usr/local/lib/liblist.so
	cp list.h /usr/local/include
	ldconfig

uninstall:
	rm /usr/local/lib/liblist.so
	rm /usr/local/include/list.h
	ldconfig

clean:
	rm $(BINS) 
