CC = gcc
OBJECTS = sndconv.c
LIBS = -lsndfile
CFLAGS = -Wall -O2
BINDIR = $(DESTDIR)/usr/bin
NAME = sndconv

sndconv: $(OBJECTS)
	$(CC) -o $(NAME) $(OBJECTS) $(LIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) $<
