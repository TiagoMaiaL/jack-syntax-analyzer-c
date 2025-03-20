#include <stdio.h>
#include "file-handler.h"
#include "tokenizer.h"

#define SUCCESS_CODE    0
#define ERROR_CODE      -1
#define ARGS_NUM        2

int main(int argc, char **argv)
{
    if (argc < ARGS_NUM) {
        printf("Invalid source file argument\n");
        printf("Usage: JackAnalyzer source.jack\n");
        return ERROR_CODE;
    }

    FILE *file_handle;
    const char *source_file_path = argv[1];

    file_handle = file_open(source_file_path);

    if (file_handle == NULL) {
        printf(
            "File %s couldn't be opened.\n", 
            source_file_path
        );
        return ERROR_CODE;
    }

    Tokenizer_atom atom;

    tokenizer_start(file_handle);
    atom = tokenizer_next();

    printf("atom val = %s\n", atom.value);
    printf("atom type = %d\n", atom.type);
    printf("atom symbol = %d\n", atom.symbol);

    return SUCCESS_CODE;
}

