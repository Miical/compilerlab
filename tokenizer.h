#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#define MAX_STR_LEN 128
#define MAX_TABLE_ITEMS 128

typedef struct {
    int type;
    void *val;
} token;

extern char symbols_table[MAX_TABLE_ITEMS][MAX_STR_LEN];
extern int constants_table[MAX_TABLE_ITEMS];

void tokenizer_init(const char *filename);
token get_next_token();

#endif
