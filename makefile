CC=gcc
CFLAGS=-Wextra -Wall -pedantic -Werror
OBJS=main.o table.o value.o run.o stack.o errors.o operators.o variable.o function.o lexer.o parser.o

lang: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) /usr/lib/x86_64-linux-gnu/libgc.so -lm

remove:
	rm *.o

clean: remove lang
