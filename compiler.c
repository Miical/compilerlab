#include "compiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define MAX_BUF (1 << 14)
char buf[MAX_BUF], *bufp;

typedef struct {
    const char *first;
    const char *second;
    const char *third;
    const char *fourth;
} Quad;

#define MAX_QUADS (1 << 10)
Quad quads[MAX_QUADS];
unsigned int nextquad;

void compiler_init() {
    bufp = buf;
    nextquad = 0;
}

/**
 * 将内容打印至缓冲区并返回位置。
 */
char *printbuf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    char *p= bufp;
    int num = vsprintf(bufp, format, args);
    bufp += num + 1;
    if ((char *)buf + MAX_BUF <= bufp)
        proc_error(PROG_ERROR, "buf out of memory");

    va_end(args);
    return p;
}

/**
 * 提交一条四元式。
 */
void emit(const char* first, const char* second,
          const char* third, const char* fourth) {
    if (nextquad == MAX_QUADS)
        proc_error(PROG_ERROR, "quads out of memory");
    quads[nextquad].first = first;
    quads[nextquad].second = second;
    quads[nextquad].third = third;
    quads[nextquad].fourth = fourth;
    nextquad++;
}

/**
 * 用addr回填一个四元式。
 */
void backpatch_quad(int id, char *addr) {
    quads[id].fourth = addr;
}

/**
 * 输出所有的四元式。
 */
void output_quads() {
    for (int i = 0; i < nextquad; i++) {
        printf("%d:\t%s, %s, %s, %s\n", i, quads[i].first, quads[i].second,
            quads[i].third, quads[i].fourth);
    }
}

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
    case PROG_ERROR:
        fprintf(stderr, "ProgError: ");
        break;
    default:
        break;
    }

    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");

    va_end(args);
    exit(EXIT_FAILURE);
}

/**
 * 处于调试模式时输出。
 */
void debug_print(const char* format, ...) {
    #ifdef DEBUG
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    #endif
}
