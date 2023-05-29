CC=gcc
CFLAGS=-Wall -g -DDEBUG
BINARIES=tokenizer parser
OBJS=tokenizer.o parser.o compiler.o parsertest.o tokenizertest.o

all: ${BINARIES}

parser: parsertest.o tokenizer.o parser.o compiler.o
	${CC} ${CFLAGS} parsertest.o tokenizer.o parser.o compiler.o -o parser

tokenizer: tokenizertest.o compiler.o tokenizer.o
	${CC} ${CFLAGS} tokenizertest.o compiler.o tokenizer.o -o tokenizer

parsertest.o: parsertest.c
	${CC} -c parsertest.c

tokenizertest.o: tokenizer.c
	${CC} -c tokenizertest.c

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
