CC=gcc
CFLAGS=-Wall -g
BINARIES=tokenizer parser
OBJS=tokenizer.o parser.o compiler.o parsertest.o tokenizertest.o

all: ${BINARIES}

parser: parsertest.o tokenizer.o parser.o compiler.o
	${CC} ${CFLAGS} parsertest.o tokenizer.o parser.o compiler.o -o parser

tokenizer: tokenizertest.o compiler.o tokenizer.o
	${CC} ${CFLAGS} tokenizertest.o compiler.o tokenizer.o -o tokenizer

parsertest.o: parsertest.c
	${CC} ${CFLAGS} -c parsertest.c

tokenizertest.o: tokenizer.c
	${CC} ${CFLAGS} -c tokenizertest.c

tokenizer.o: tokenizer.h tokenizer.c
	${CC} ${CFLAGS} -c tokenizer.c

parser.o: parser.h parser.c
	${CC} ${CFLAGS} -c parser.c

compiler.o: compiler.h compiler.c
	${CC} ${CFLAGS} -c compiler.c

clean:
	rm -f $(BINARIES) ${OBJS}
