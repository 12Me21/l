CC=gcc
CFLAGS=-Wextra -Wall -pedantic
OBJS=main.o table.o value.o run.o

lang: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) /usr/lib/x86_64-linux-gnu/libgc.so -lm
