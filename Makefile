CC=gcc
CFLAGS=-Wall -g -O2
BINARIES=compiler
OBJS=tokenizer.o parser.o compiler.o

all: ${BINARIES}

compiler: tokenizer.o parser.o compiler.o list.o
	${CC} ${CFLAGS} tokenizer.o parser.o compiler.o list.o -o compiler

tokenizer.o: tokenizer.h tokenizer.c compiler.h
parser.o: parser.h parser.c list.h tokenizer.h compiler.h
compiler.o: compiler.h compiler.c tokenizer.h parser.h
list.o: list.h list.c compiler.h

clean:
	rm -f $(BINARIES) *.o
