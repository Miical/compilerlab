#include "compiler.h"
#include "tokenizer.h"

static Token sym;
static void advance(void);
static void match_factor(void);
static void match_unary(void);
static void match_rest6(void);
static void match_term(void);
static void match_rest5(void);
static void match_expr(void);
static void match_stmts(void);
static void match_rest0(void);
static void match_stmt(void);
static void match_loc(void);
static void match_resta(void);
static void match_elist(void);
static void match_rest1(void);
static void match_bool(void);
static void match_equality(void);
static void match_rest4(void);
static void match_rel(void);
static void match_rop_expr(void);

void parser_match() {
    match_stmts();
    debug_print("[Complete]\n");
}

void parser_init() {
    advance();
}

static void match_factor() {
    if (sym.type == LPAREN) {
        advance();
        debug_print("factor -> (expr)\n");

        match_expr();

        if (sym.type != RPAREN)
            proc_error(PARSER_ERROR, "expect RPAREN but got type #%d", sym.type);
        advance();
    } else if (sym.type == IDENTIFIER) {
        debug_print("factor -> loc\n");
        match_loc();
    } else if (sym.type == CONSTANT) {
        debug_print("factor -> <num>\n");
        advance();
    } else {
        proc_error(PARSER_ERROR, "expect LPAREN, IDENTIFIER or CONSTANT but got type #%d", sym.type);
    }
}

static void match_unary() {
    debug_print("unary -> factor\n");
    match_factor();
}

static void match_rest6() {
    if (sym.type == MUTIPLE) {
        debug_print("rest6 -> *unary rest6\n");
        advance();
        match_unary();
        match_rest6();
    }
    else if (sym.type == DIVIDE) {
        debug_print("rest6 -> /unary rest6\n");
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
        debug_print("rest5 -> +term rest5\n");
        advance();
        match_term();
        match_rest5();
    }
    else if (sym.type == MINUS) {
        debug_print("rest5 -> -term rest5\n");
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

static void match_stmts() {
    debug_print("stmts -> stmt rest0\n");
    match_stmt();
    match_rest0();
}

static void match_rest0() {
    if (sym.type == IDENTIFIER || sym.type == IF || sym.type == WHILE) {
        debug_print("rest0 -> stmt rest0\n");
        match_stmt();
        match_rest0();
    } else {
        debug_print("rest0 -> <epsilon>\n");
    }
}

static void match_stmt() {
    if (sym.type == IDENTIFIER) {
        debug_print("stmt -> loc = expr;\n");
        match_loc();

        if (sym.type != ASSIGN)
            proc_error(PARSER_ERROR, "expect ASSIGN but got type #%d", sym.type);
        advance();

        match_expr();

        if (sym.type != SEMI)
            proc_error(PARSER_ERROR, "expect SEMI but got type #%d", sym.type);
        advance();
    } else if (sym.type == IF) {
        advance();
        debug_print("stmt -> if(bool) stmt else stmt\n");

        if (sym.type != LPAREN)
            proc_error(PARSER_ERROR, "expect LPAREN but got type #%d", sym.type);
        advance();

        match_bool();

        if (sym.type != RPAREN)
            proc_error(PARSER_ERROR, "expect RPAREN but got type #%d", sym.type);
        advance();

        match_stmt();

        if (sym.type != ELSE)
            proc_error(PARSER_ERROR, "expect ELSE but got type #%d", sym.type);
        advance();

        match_stmt();
    } else if (sym.type == WHILE) {
        advance();
        debug_print("stmt -> while(bool) stmt\n");

        if (sym.type != LPAREN)
            proc_error(PARSER_ERROR, "expect LPAREN but got type #%d", sym.type);
        advance();

        match_bool();

        if (sym.type != RPAREN)
            proc_error(PARSER_ERROR, "expect RPAREN but got type #%d", sym.type);
        advance();

        match_stmt();
    } else {
        proc_error(PARSER_ERROR, "expect IDENTIFIER, IF or WHILE but got type #%d", sym.type);
    }
}

static void match_loc() {
    if (sym.type != IDENTIFIER)
        proc_error(PARSER_ERROR, "expect IDENTIFIER but got type #%d", sym.type);
    debug_print("loc -> <id> resta\n");
    advance();
    match_resta();
}

static void match_resta() {
    if (sym.type == LBRACKET) {
        advance();
        debug_print("resta -> [elist]\n");
        match_elist();

        if (sym.type != RBRACKET)
            proc_error(PARSER_ERROR, "expect RBRACKET but got type #%d", sym.type);
        advance();
    } else {
        debug_print("resta -> <epsilon>\n");
    }
}

static void match_elist() {
    debug_print("elist -> expr rest1\n");
    match_expr();
    match_rest1();
}

static void match_rest1() {
    if (sym.type == COMMA) {
        advance();
        debug_print("rest1 -> ,expr rest1\n");
        match_expr();
        match_rest1();
    } else {
        debug_print("rest1 -> <epsilon>\n");
    }
}

static void match_bool() {
    debug_print("bool -> equality\n");
    match_equality();
}

static void match_equality() {
    debug_print("equality -> rel rest4\n");
    match_rel();
    match_rest4();
}

static void match_rest4() {
    if (sym.type == EQ) {
        advance();
        debug_print("rest4 -> ==rel rest4\n");
        match_rel();
        match_rest4();
    } else if (sym.type == NEQ) {
        advance();
        debug_print("rest4 -> !=rel rest4\n");
        match_rel();
        match_rest4();
    } else {
        debug_print("rest4 -> <epsilon>\n");
    }
}

static void match_rel() {
    debug_print("rel -> expr rop_expr\n");
    match_expr();
    match_rop_expr();
}

static void match_rop_expr() {
    if (sym.type == LT) {
        advance();
        debug_print("rop_expr -> <expr\n");
        match_expr();
    } else if (sym.type == LEQ) {
        advance();
        debug_print("rop_expr -> <=expr\n");
        match_expr();
    } else if (sym.type == GT) {
        advance();
        debug_print("rop_expr -> >expr\n");
        match_expr();
    } else if (sym.type == GEQ) {
        advance();
        debug_print("rop_expr -> >=expr\n");
        match_expr();
    } else {
        debug_print("rop_expr -> <epsilon>\n");
    }
}

/**
 * 取出下一个 Token，置于 sym 中。
 */
static void advance() {
    sym = get_next_token();
}
