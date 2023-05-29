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
}

void parser_init() {
    advance();
}

static void match_factor() {
    if (sym.type != CONSTANT)
        proc_error(PARSER_ERROR, "expect CONSTANT but get type #%d", sym.type);
    advance();
}

static void match_unary() {
    match_factor();
}

static void match_rest6() {
    if (sym.type == MUTIPLE) {
        advance();
        match_unary();
        match_rest6();
    }
    else if (sym.type == DIVIDE) {
        advance();
        match_unary();
        match_rest6();
    }
}

static void match_term() {
    match_unary();
    match_rest6();
}

static void match_rest5() {
    if (sym.type == PLUS) {
        advance();
        match_term();
        match_rest5();
    }
    else if (sym.type == MINUS) {
        advance();
        match_term();
        match_rest5();
    }
}

static void match_expr() {
    match_term();
    match_rest5();
}

static void advance() {
    sym = get_next_token();
}
