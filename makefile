CC=gcc
CFLAGS=-Wextra -Wall -pedantic
OBJS=main.o table.o value.o run.o stack.o errors.o operators.o variable.o function.o

lang: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) /usr/lib/x86_64-linux-gnu/libgc.so -lm
