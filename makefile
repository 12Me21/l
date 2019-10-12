CC=gcc
CFLAGS=
OBJS=main.o table.o value.o

lang: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -lm
