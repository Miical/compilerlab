CC=gcc
CFLAGS=-Wall -g
BINARIES=tokenizer
OBJS=tokenizer.o main.o

tokenizer: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o ${BINARIES}

tokenizer.o: tokenizer.h tokenizer.c
	${CC} -c tokenizer.c

main.o: main.c
	${CC} -c main.c

clean:
	rm -f $(BINARIES) ${OBJS}
