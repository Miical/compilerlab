#include <stdio.h>
#include "tokenizer.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Arguments Error.\n");
        return 0;
    }

    tokenizer_init(argv[1]);
    token t;
    while ((t = get_next_token()).type != 0) {
        if (t.type == 111) {
            printf("<%d, %s>\n", t.type, (char *)t.val);
        } else if (t.type == 100) {
            printf("<%d, %d>\n", t.type, *(int *)t.val);
        } else {
            printf("<%d, - >\n", t.type);
        }
    }
    return 0;
}
