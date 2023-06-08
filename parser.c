#include "compiler.h"
#include "tokenizer.h"
#include <stdio.h>

static Token sym;
static void advance(void);
static void ensure(int expecttype, const char* name);

static char *match_factor(void);
static char *match_unary(void);
static char *match_rest6(char *rest6_in);
static char *match_term(void);
static char *match_rest5(char* rest5_in);
static char *match_expr(void);
static void match_stmts(void);
static void match_rest0(void);
static void match_stmt(void);
static char *match_loc(void);
static char *match_resta(char* resta_inArray);
static void match_elist(void);
static void match_rest1(void);
static void match_bool(void);
static void match_equality(void);
static void match_rest4(void);
static void match_rel(void);
static void match_rop_expr(void);

static unsigned int tempcnt;
static char *newtemp();
static char *make_num(int number);

void parser_match() {
    match_stmts();
    debug_print("[Complete]\n");
}

void parser_init() {
    advance();
}

static char *match_factor() {
    char *factor_place = NULL;
    if (sym.type == LPAREN) {
        advance();
        debug_print("factor -> (expr)\n");
        match_expr();
        factor_place = match_expr();
        ensure(RPAREN, "RPAREN");
    } else if (sym.type == IDENTIFIER) {
        debug_print("factor -> loc\n");
        factor_place = match_loc();
    } else if (sym.type == CONSTANT) {
        debug_print("factor -> <num>\n");
        factor_place = make_num(*(int *)sym.val);
        advance();
    } else {
        proc_error(PARSER_ERROR, "expect LPAREN, IDENTIFIER or CONSTANT but got type #%d", sym.type);
    }
    return factor_place;
}

static char *match_unary() {
    char *unary_place;
    debug_print("unary -> factor\n");
    unary_place =  match_factor();
    return unary_place;
}

static char *match_rest6(char *rest6_in) {
    char *rest6_place;
    if (sym.type == MUTIPLE) {
        debug_print("rest6 -> *unary rest6\n");
        advance();
        char *unary_place = match_unary();
        char *rest6s_in = newtemp();
        emit("*", rest6_in, unary_place, rest6s_in);
        rest6_place = match_rest6(rest6s_in);
    }
    else if (sym.type == DIVIDE) {
        debug_print("rest6 -> /unary rest6\n");
        advance();
        char *unary_place = match_unary();
        char *rest6s_in = newtemp();
        emit("/", rest6_in, unary_place, rest6s_in);
        rest6_place = match_rest6(rest6s_in);
    }
    else {
        debug_print("rest6 -> <epsilon>\n");
        rest6_place = rest6_in;
    }
    return rest6_place;
}

static char *match_term() {
    char *term_place;
    debug_print("term -> unary rest6\n");
    char *rest6_in = match_unary();
    term_place = match_rest6(rest6_in);
    return term_place;
}

static char *match_rest5(char* rest5_in) {
    char *rest5_place;
    if (sym.type == PLUS) {
        debug_print("rest5 -> +term rest5\n");
        advance();
        char *term_place = match_term();
        char *rest5s_in = newtemp();
        emit("+", rest5_in, term_place, rest5s_in);
        rest5_place = match_rest5(rest5s_in);
    }
    else if (sym.type == MINUS) {
        debug_print("rest5 -> -term rest5\n");
        advance();
        char *term_place = match_term();
        char *rest5s_in = newtemp();
        emit("-", rest5_in, term_place, rest5s_in);
        rest5_place = match_rest5(rest5s_in);
    }
    else {
        debug_print("rest5 -> <epsilon>\n");
        rest5_place = rest5_in;
    }
    return rest5_place;
}

static char *match_expr() {
    char *expr_place = NULL;
    debug_print("expr -> term rest5\n");
    char *term_place = match_term();
    expr_place = match_rest5(term_place);
    return expr_place;
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
        char *loc_place = match_loc();
        ensure(ASSIGN, "ASSIGN");
        char *expr_place = match_expr();
        ensure(SEMI, "SEMI");
        emit("=", expr_place, "-", loc_place);
    } else if (sym.type == IF) {
        advance();
        debug_print("stmt -> if(bool) stmt else stmt\n");
        ensure(LPAREN, "LPAREN");
        match_bool();
        ensure(RPAREN, "RPAREN");
        match_stmt();
        ensure(ELSE, "ELSE");
        match_stmt();
    } else if (sym.type == WHILE) {
        advance();
        debug_print("stmt -> while(bool) stmt\n");
        ensure(LPAREN, "LPAREN");
        match_bool();
        ensure(RPAREN, "RPAREN");
        match_stmt();
    } else {
        proc_error(PARSER_ERROR, "expect IDENTIFIER, IF or WHILE but got type #%d", sym.type);
    }
}

static char *match_loc() {
    char *loc_place = NULL;
    if (sym.type != IDENTIFIER)
        proc_error(PARSER_ERROR, "expect IDENTIFIER but got type #%d", sym.type);
    debug_print("loc -> <id> resta\n");
    char *resta_inArray = (char *)sym.val;
    advance();
    loc_place = match_resta(resta_inArray);
    return loc_place;
}

static char *match_resta(char* resta_inArray) {
    char *resta_place = NULL;
    if (sym.type == LBRACKET) {
        advance();
        debug_print("resta -> [elist]\n");
        match_elist();
        ensure(RBRACKET, "RBRACKET");
    } else {
        debug_print("resta -> <epsilon>\n");
        resta_place = resta_inArray;
    }
    return resta_place;
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
 * 检测当前 Token sym，是否为 expecttype。
 * 正确则移动至下一个 Token，否则进入错误处理。
 */
static void ensure(int expecttype, const char* name) {
    if (sym.type != expecttype)
        proc_error(PARSER_ERROR, "expect %s but got type #%d", name, sym.type);
    advance();
}

/**
 * 取出下一个 Token，置于 sym 中。
 */
static void advance() {
    sym = get_next_token();
}

/**
 * 生成一个临时变量的名字。
 */
static char *newtemp() {
    char *p= bufp;
    int num = sprintf(bufp, "t%d", ++tempcnt);
    bufp += num + 1;
    if ((char *)buf + MAX_BUF <= bufp)
        proc_error(PROG_ERROR, "buf out of memory");
    return p;
}

/**
 * 将数字转为字符串，并放入缓存区。
 */
static char *make_num(int number) {
    char *p= bufp;
    int num = sprintf(bufp, "%d", number);
    bufp += num + 1;
    if ((char *)buf + MAX_BUF <= bufp)
        proc_error(PROG_ERROR, "buf out of memory");
    return p;
}
