#include "compiler.h"
#include "tokenizer.h"

static Token sym;
static void advance(void);
static void match_factor();
static void match_unary();
static void match_rest6();
static void match_term();
static void match_rest5();
static void match_expr();

void parser_match() {
    match_expr();
    debug_print("[Complete]\n");
}

void parser_init() {
    advance();
}

static void match_factor() {
    if (sym.type != CONSTANT)
        proc_error(PARSER_ERROR, "expect CONSTANT but get type #%d", sym.type);
    debug_print("factor -> num\n");
    advance();
}

static void match_unary() {
    debug_print("unary -> factor\n");
    match_factor();
}

static void match_rest6() {
    if (sym.type == MUTIPLE) {
        debug_print("rest6 -> * unary rest6\n");
        advance();
        match_unary();
        match_rest6();
    }
    else if (sym.type == DIVIDE) {
        debug_print("rest6 -> / unary rest6\n");
        advance();
        match_unary();
        match_rest6();
    }
    else {
        debug_print("rest6 -> <epsilon>\n");
    }
}

static void match_term() {
    debug_print("term -> unary rest6\n");
    match_unary();
    match_rest6();
}

static void match_rest5() {
    if (sym.type == PLUS) {
        debug_print("rest5 -> + term rest5\n");
        advance();
        match_term();
        match_rest5();
    }
    else if (sym.type == MINUS) {
        debug_print("rest5 -> - term rest5\n");
        advance();
        match_term();
        match_rest5();
    }
    else {
        debug_print("rest5 -> <epsilon>\n");
    }
}

static void match_expr() {
    debug_print("expr -> term rest5\n");
    match_term();
    match_rest5();
}

/**
 * 取出下一个 Token，置于 sym 中。
 */
static void advance() {
    sym = get_next_token();
}
