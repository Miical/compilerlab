#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#define MAX_STR_LEN 128
#define MAX_TABLE_ITEMS 128

#define INCREMENT 56
#define PLUS 41
#define DECREMENT 57
#define MINUS 42
#define MUTIPLE 43
#define DIVIDE 44
#define MOD 45
#define EQ 51
#define ASSIGN 46
#define GEQ 48
#define GT 47
#define LEQ 50
#define LT 49
#define NEQ 52
#define NOT 55
#define AND 53
#define OR 54
#define LPAREN 81
#define RPAREN 82
#define SEMI 84
#define COMMA 85
#define LCURLY 86
#define RCURLY 87
#define LBRACKET 88
#define RBRACKET 89
#define INT 5
#define ELSE 15
#define IF 17
#define WHILE 20
#define IDENTIFIER 111
#define CONSTANT 100

typedef struct {
    int type;
    void *val;
} Token;

extern char symbols_table[MAX_TABLE_ITEMS][MAX_STR_LEN];
extern int constants_table[MAX_TABLE_ITEMS];

void tokenizer_init(const char *filename);
Token get_next_token();

#endif
