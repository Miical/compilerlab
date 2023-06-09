#ifndef COMPILER_H_
#define COMPILER_H_

char *printbuf(const char* format, ...);

extern unsigned int nextquad;
void emit(const char* first, const char* second,
    const char* third, const char* fourth);
void backpatch_quad(int id, char *addr);

typedef enum{
    TOKENIZER_ERROR,
    PARSER_ERROR,
    PROG_ERROR
} ErrorType;

void debug_print(const char* format, ...);
void proc_error(ErrorType error_type, const char* format, ...);

#endif
