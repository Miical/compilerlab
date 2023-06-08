#include <stdio.h>
#include "compiler.h"
#include "tokenizer.h"
#include "parser.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Arguments Error.\n");
        return 0;
    }

    compiler_init();
    tokenizer_init(argv[1]);
    parser_init();

    parser_match();
    output_quads();

    return 0;
}
