#ifndef COMPILER_H_
#define COMPILER_H_

typedef enum{
    TOKENIZER_ERROR,
    PARSER_ERROR
} ErrorType;

void proc_error(ErrorType error_type, const char* format, ...);

#endif
