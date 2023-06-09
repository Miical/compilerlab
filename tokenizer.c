#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "compiler.h"
#include "tokenizer.h"

static FILE* source;
static char ch;
static int tokenlen;
static char str_token[MAX_STR_LEN];

static void get_char(void);
static void get_BC(void);
static void concat(void);
static bool is_letter(void);
static bool is_digit(void);
static int reserve(void);
static void retract(void);
static char* insert_id(void);
static int* insert_const(void);

char symbols_table[MAX_TABLE_ITEMS][MAX_STR_LEN];
int constants_table[MAX_TABLE_ITEMS];
int symbols_table_len, constants_table_len;

void tokenizer_init(const char *filename) {
    source = fopen(filename, "r");
    if (source == NULL) {
        proc_error(TOKENIZER_ERROR, "can't open file '%s'", filename);
    }
    symbols_table_len = 0, constants_table_len = 0, tokenlen = 0;
    str_token[0] = '\0';
    ch = ' ';
}

Token get_next_token() {
    int code; void *value;
    tokenlen = 0;
    get_char(); get_BC();
    if (is_letter()) {
        while (is_letter() || is_digit()) {
            concat(); get_char();
        }
        retract();
        code = reserve();
        if (code == 0) {
            value = (void *)insert_id();
            return (Token){IDENTIFIER, value};
        } else {
            return (Token){code, NULL};
        }
    } else if (is_digit()) {
        while (is_digit()) {
            concat(); get_char();
        }
        retract();
        value = (void *)insert_const();
        return (Token) {CONSTANT, value};
    } else {
        switch(ch) {
            case '+':
                get_char();
                if (ch == '+') {
                    return (Token){INCREMENT, NULL};
                } else {
                    retract();
                    return (Token){PLUS, NULL};
                }
            case '-':
                get_char();
                if (ch == '-') {
                    return (Token){DECREMENT, NULL};
                } else {
                    retract();
                    return (Token){MINUS, NULL};
                }
            case '*':
                return (Token){MUTIPLE, NULL};
            case '/':
                return (Token){DIVIDE, NULL};
            case '%':
                return (Token){MOD, NULL};
            case '=':
                get_char();
                if (ch == '=') {
                    return (Token){EQ, NULL};
                } else {
                    retract();
                    return (Token){ASSIGN, NULL};
                }
            case '>':
                get_char();
                if (ch == '=') {
                    return (Token){GEQ, NULL};
                } else {
                    retract();
                    return (Token){GT, NULL};
                }
            case '<':
                get_char();
                if (ch == '=') {
                    return (Token){LEQ, NULL};
                } else {
                    retract();
                    return (Token){LT, NULL};
                }
            case '!':
                get_char();
                if (ch == '=') {
                    return (Token){NEQ, NULL};
                } else {
                    retract();
                    return (Token){NOT, NULL};
                }
            case '&':
                get_char();
                if (ch == '&') {
                    return (Token){AND, NULL};
                } else {
                    proc_error(TOKENIZER_ERROR, "expect '&' but got '%c'", ch);
                }
            case '|':
                get_char();
                if (ch == '|') {
                    return (Token){OR, NULL};
                } else {
                    proc_error(TOKENIZER_ERROR, "expect '|' but got '%c'", ch);
                }
            case '(':
                return (Token){LPAREN, NULL};
            case ')':
                return (Token){RPAREN, NULL};
            case ';':
                return (Token){SEMI, NULL};
            case ',':
                return (Token){COMMA, NULL};
            case '{':
                return (Token){LCURLY, NULL};
            case '}':
                return (Token){RCURLY, NULL};
            case '[':
                return (Token){LBRACKET, NULL};
            case ']':
                return (Token){RBRACKET, NULL};
            default:
                break;
        }
    }
    if (ch != EOF) {
        proc_error(TOKENIZER_ERROR, "unrecognized character '%c'", ch);
    }
    return (Token){0, NULL};
}

/**
 * 将下一输入字符读到ch中，搜索指示器前移一字符位置。
 */
static void get_char() {
    ch = fgetc(source);
}

/**
 * 检查ch中的字符是否为空白。若是，则调用get_char直至ch中进入一个空白字符。
 */
static void get_BC() {
    while (isblank((int)ch) || ch == '\n') {
        get_char();
    }
}

/**
 * 将ch中的字符连接到strToken之后。
 */
static void concat() {
    str_token[tokenlen++] = ch;
    str_token[tokenlen] = '\0';
}

/**
 * 判断ch中的字符是否为字母。
 */
static bool is_letter() {
    return isalpha((int)ch);
}

/**
 * 判断ch中的字符是否为数字。
 */
static bool is_digit() {
    return isdigit((int)ch);
}

/**
 * 对strToken中的字符串查找保留字表，若它是一个保留字则返回它的编码，否则返回0值。
 */
static int reserve() {
    if (!strcmp(str_token, "int")) return INT;
    else if (!strcmp(str_token, "else")) return ELSE;
    else if (!strcmp(str_token, "if")) return IF;
    else if (!strcmp(str_token, "while")) return WHILE;
    else return 0;
}

/**
 * 将搜索指示器回调一个字符位置，将ch置为空白字符。
 */
static void retract() {
    ch = ' ';
    if (fseek(source, -sizeof(char), SEEK_CUR))
        proc_error(TOKENIZER_ERROR, "retract at the beginning of the file");
}

/**
 * 将strToken中的标识符插人符号表，返回符号表指针。
 */
static char* insert_id() {
    for (int i = 0; i < symbols_table_len; i++)
        if (!strcmp(symbols_table[i], str_token))
            return (char*)&symbols_table[i];
    strcpy(symbols_table[symbols_table_len++], str_token);
    return (char*)&symbols_table[symbols_table_len - 1];
}

/**
 * 将strToken中的常数插人常数表，返回常数表指针。
 */
static int* insert_const() {
    int num = atoi(str_token);
    for (int i = 0; i < constants_table_len; i++)
        if (constants_table[i] == num)
            return &constants_table[i];
    constants_table[constants_table_len++] = num;
    return &constants_table[constants_table_len - 1];
}

