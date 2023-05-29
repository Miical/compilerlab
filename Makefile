CC=gcc
CFLAGS=-Wall -g
BINARIES=tokenizer
OBJS=tokenizer.o parser.o compiler.o main.o

tokenizer: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o ${BINARIES}

tokenizer.o: tokenizer.h tokenizer.c
	${CC} -c tokenizer.c

parser.o: parser.h parser.c
	${CC} -c parser.c

compiler.o: compiler.h compiler.c
	${CC} -c compiler.c

main.o: main.c
	${CC} -c main.c

clean:
	rm -f $(BINARIES) ${OBJS}
