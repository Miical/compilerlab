#include <stdio.h>
#include "tokenizer.h"
#include "parser.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Arguments Error.\n");
        return 0;
    }

    tokenizer_init(argv[1]);
    parser_init();

    parser_match();

    return 0;
}
