#include <stdio.h>
#include "compiler.h"
#include "tokenizer.h"
#include "parser.h"
#include "list.h"
#define UNUSED_VALUE(v) (void)(v);

typedef struct {
    char *place;
    char *offset;
} place_offset;

typedef struct {
    List truelist;
    List falselist;
} bool_list;

typedef struct {
    char *array;
    char *offset;
} array_offset;

static Token sym;
static void advance(void);
static void ensure(int expecttype, const char* name);

static char *match_factor(void);
static char *match_unary(void);
static char *match_rest6(char *rest6_in);
static char *match_term(void);
static char *match_rest5(char* rest5_in);
static char *match_expr(void);
static List match_stmts(void);
static List match_rest0(List rest0_inNextlist);
static List match_stmt(void);
static place_offset match_loc(void);
static place_offset match_resta(char* resta_inArray);
static array_offset match_elist(char* elist_inArray);
static array_offset match_rest1(char* rest1_inArray,
    int rest1_inNdim, char* rest1_inPlace);
static bool_list match_bool(void);
static bool_list match_equality(void);
static bool_list match_rest4(List rest4_inTruelist, List rest4_inFalselist);
static bool_list match_rel(void);
static bool_list match_rop_expr(char* rop_expr_inPlace);
static int match_m(void);
static List match_n(void);

static unsigned int tempcnt;
static char *newtemp();
static char *make_num(int number);
static char *make_arrayelem(char* place, char *offset);
static char *limit(char *array, int j);

void parser_match() {
    match_stmts();
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
        place_offset loc = match_loc();
        if (loc.offset == NULL) {
            factor_place = loc.place;
        } else {
            factor_place = newtemp();
            emit("=[]", make_arrayelem(loc.place, loc.offset), "-", factor_place);
        }
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

static List match_stmts() {
    List stmts_nextlist = NULL;
    debug_print("stmts -> stmt rest0\n");
    List stmt_nextlist =match_stmt();
    List rest0_inNextlist = stmt_nextlist;
    List rest0_nextlist = match_rest0(rest0_inNextlist);
    stmts_nextlist = rest0_nextlist;
    return stmts_nextlist;
}

static List match_rest0(List rest0_inNextlist) {
    List rest0_nextlist = NULL;
    if (sym.type == IDENTIFIER || sym.type == IF || sym.type == WHILE) {
        debug_print("rest0 -> stmt rest0\n");
        int m_quad = match_m();
        backpatch(rest0_inNextlist, make_num(m_quad));
        List stmt_nextlist = match_stmt();
        List rest0s_inNextlist = stmt_nextlist;
        List rest0s_nextlist = match_rest0(rest0s_inNextlist);
        rest0_nextlist = rest0s_nextlist;
    } else {
        debug_print("rest0 -> <epsilon>\n");
        rest0_nextlist = rest0_inNextlist;
    }
    return rest0_nextlist;
}

static List match_stmt() {
    List stmt_nextlist = NULL;
    if (sym.type == IDENTIFIER) {
        debug_print("stmt -> loc = expr;\n");
        place_offset loc = match_loc();
        ensure(ASSIGN, "ASSIGN");
        char *expr_place = match_expr();
        ensure(SEMI, "SEMI");
        if (loc.offset == NULL) {
            emit("=", expr_place, "-", loc.place);
        } else {
            emit("[]=", expr_place, "-", make_arrayelem(loc.place, loc.offset));
            stmt_nextlist = NULL;
        }
    } else if (sym.type == IF) {
        advance();
        debug_print("stmt -> if(bool) m stmt n else m stmt\n");
        ensure(LPAREN, "LPAREN");
        bool_list bool = match_bool();
        ensure(RPAREN, "RPAREN");
        int m1_quad = match_m();
        List stmt1_nextlist = match_stmt();
        List n_nextlist = match_n();
        ensure(ELSE, "ELSE");
        int m2_quad = match_m();
        List stmt2_nextlist = match_stmt();
        backpatch(bool.truelist, make_num(m1_quad));
        backpatch(bool.falselist, make_num(m2_quad));
        stmt_nextlist = merge(merge(stmt1_nextlist, n_nextlist), stmt2_nextlist);
    } else if (sym.type == WHILE) {
        advance();
        debug_print("stmt -> while(bool) stmt\n");
        ensure(LPAREN, "LPAREN");
        int m1_quad = match_m();
        bool_list bool = match_bool();
        ensure(RPAREN, "RPAREN");
        int m2_quad = match_m();
        List stmt1_nextlist = match_stmt();
        backpatch(stmt1_nextlist, make_num(m1_quad));
        backpatch(bool.truelist, make_num(m2_quad));
        stmt_nextlist = bool.falselist;
        emit("j", "-", "-", make_num(m1_quad));
    } else {
        proc_error(PARSER_ERROR, "expect IDENTIFIER, IF or WHILE but got type #%d", sym.type);
    }
    return stmt_nextlist;
}

static place_offset match_loc() {
    place_offset loc;
    if (sym.type != IDENTIFIER)
        proc_error(PARSER_ERROR, "expect IDENTIFIER but got type #%d", sym.type);
    debug_print("loc -> <id> resta\n");
    char *resta_inArray = (char *)sym.val;
    advance();
    place_offset resta = match_resta(resta_inArray);
    loc.place = resta.place;
    loc.offset = resta.offset;
    return loc;
}

static place_offset match_resta(char* resta_inArray) {
    place_offset resta;
    if (sym.type == LBRACKET) {
        advance();
        debug_print("resta -> [elist]\n");
        char *elist_inArray = resta_inArray;
        array_offset elist = match_elist(elist_inArray);
        ensure(RBRACKET, "RBRACKET");
        resta.place = newtemp();
        emit("-", elist.array, "C", resta.place);
        resta.offset = newtemp();
        emit("*", "w", elist.offset, resta.offset);
    } else {
        debug_print("resta -> <epsilon>\n");
        resta.place = resta_inArray;
        resta.offset = NULL;
    }
    return resta;
}

static array_offset match_elist(char* elist_inArray) {
    array_offset elist;
    debug_print("elist -> expr rest1\n");
    char *expr_place = match_expr();
    char *rest1_inArray = elist_inArray;
    int rest1_inNdim = 1;
    char* rest1_inPlace = expr_place;
    array_offset rest1 = match_rest1(rest1_inArray, rest1_inNdim, rest1_inPlace);
    elist.array = rest1.array;
    elist.offset = rest1.offset;
    return elist;
}

static array_offset match_rest1(char* rest1_inArray,
    int rest1_inNdim, char* rest1_inPlace) {
    array_offset rest1;
    if (sym.type == COMMA) {
        advance();
        debug_print("rest1 -> ,expr rest1\n");
        char *expr_place = match_expr();
        char *t = newtemp();
        int m = rest1_inNdim + 1;
        emit("*", rest1_inPlace, limit(rest1_inArray, m), t);
        emit("+", t, expr_place, t);
        char *rest1s_inArray = rest1_inArray;
        int rest1s_inNdim = m;
        char *rest1s_inNplace = t;
        array_offset rest1s = match_rest1(rest1s_inArray,
            rest1s_inNdim, rest1s_inNplace);
        rest1.array = rest1s.array;
        rest1.offset = rest1s.offset;
    } else {
        debug_print("rest1 -> <epsilon>\n");
        rest1.array = rest1_inArray;
        rest1.offset = rest1_inPlace;
    }
    return rest1;
}

static bool_list match_bool() {
    bool_list bool;
    debug_print("bool -> equality\n");
    bool_list equality = match_equality();
    bool.truelist = equality.truelist;
    bool.falselist = equality.falselist;
    return bool;
}

static bool_list match_equality() {
    bool_list equality;
    debug_print("equality -> rel rest4\n");
    bool_list rel = match_rel();
    List rest4_inTruelist = rel.truelist;
    List rest4_inFalselist = rel.falselist;
    bool_list rest4 = match_rest4(rest4_inTruelist, rest4_inFalselist);
    equality.truelist = rest4.truelist;
    equality.falselist = rest4.falselist;
    return equality;
}

static bool_list match_rest4(List rest4_inTruelist, List rest4_inFalselist) {
    bool_list rest4 = { NULL, NULL };
    if (sym.type == EQ) {
        advance();
        debug_print("rest4 -> ==rel rest4\n");
        match_rel();
        match_rest4(NULL, NULL);
    } else if (sym.type == NEQ) {
        advance();
        debug_print("rest4 -> !=rel rest4\n");
        match_rel();
        match_rest4(NULL, NULL);
    } else {
        debug_print("rest4 -> <epsilon>\n");
        rest4.truelist = rest4_inTruelist;
        rest4.falselist = rest4_inFalselist;
    }
    return rest4;
}

static bool_list match_rel() {
    bool_list rel;
    debug_print("rel -> expr rop_expr\n");
    char *expr_place = match_expr();
    char *rop_expr_inPlace = expr_place;
    bool_list rop_expr = match_rop_expr(rop_expr_inPlace);
    rel.truelist = rop_expr.truelist;
    rel.falselist = rop_expr.falselist;
    return rel;
}

static bool_list match_rop_expr(char* rop_expr_inPlace) {
    bool_list rop_expr;
    rop_expr.truelist = makelist(nextquad);
    rop_expr.falselist= makelist(nextquad + 1);
    if (sym.type == LT) {
        advance();
        debug_print("rop_expr -> <expr\n");
        char *expr_place = match_expr();
        emit("j<", rop_expr_inPlace, expr_place, "-");
        emit("j", "-", "-", "-");
    } else if (sym.type == LEQ) {
        advance();
        debug_print("rop_expr -> <=expr\n");
        char *expr_place = match_expr();
        emit("j<=", rop_expr_inPlace, expr_place, "-");
        emit("j", "-", "-", "-");
    } else if (sym.type == GT) {
        advance();
        debug_print("rop_expr -> >expr\n");
        char *expr_place = match_expr();
        emit("j>", rop_expr_inPlace, expr_place, "-");
        emit("j", "-", "-", "-");
    } else if (sym.type == GEQ) {
        advance();
        debug_print("rop_expr -> >=expr\n");
        char *expr_place = match_expr();
        emit("j>=", rop_expr_inPlace, expr_place, "-");
        emit("j", "-", "-", "-");
    } else {
        debug_print("rop_expr -> <epsilon>\n");
        emit("jnz", rop_expr_inPlace, "-", "-");
        emit("j", "-", "-", "-");
    }
    return rop_expr;
}

static int match_m(void) {
    int m_quad;
    debug_print("m -> <epsilon>\n");
    m_quad = nextquad;
    return m_quad;
}

static List match_n(void) {
    List n_nextlist;
    debug_print("n -> <epsilon>\n");
    n_nextlist = makelist(nextquad);
    emit("j", "-", "-", "0");
    return n_nextlist;
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
    return printbuf("t%d", ++tempcnt);
}

/**
 * 将数字转为字符串，并放入缓存区。
 */
static char *make_num(int number) {
    return printbuf("%d", number);
}

/**
 * 构造数组引用的字符串，并插入到缓存区。
 */
static char *make_arrayelem(char* place, char *offset) {
    return printbuf("%s[%s]", place, offset);
}

/**
 * 返回array数组的第j维长度。
 */
static char *limit(char *array, int j) {
    UNUSED_VALUE(array);
    return printbuf("n%d", j);
}
