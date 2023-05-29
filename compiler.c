#include "compiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/**
 * 错误处理。
 */
void proc_error(ErrorType error_type, const char* format, ...) {
    va_list args;
    va_start(args, format);

    switch(error_type) {
    case TOKENIZER_ERROR:
        fprintf(stderr, "TokenizerError: ");
        break;
    case PARSER_ERROR:
        fprintf(stderr, "ParserError: ");
        break;
    default:
        break;
    }

    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");

    va_end(args);
    exit(EXIT_FAILURE);
}
