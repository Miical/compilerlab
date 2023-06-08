#ifndef COMPILER_H_
#define COMPILER_H_

#define MAX_BUF (1 << 14)
extern char buf[MAX_BUF], *bufp;

extern unsigned int nextquad;
void emit(const char* first, const char* second,
    const char* third, const char* fourth);
void output_quads();

typedef enum{
    TOKENIZER_ERROR,
    PARSER_ERROR,
    PROG_ERROR
} ErrorType;

void debug_print(const char* format, ...);
void proc_error(ErrorType error_type, const char* format, ...);

void compiler_init();

#endif
