#include <stdio.h>

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

    char *source_file = argv[1];

    // TODO: Add module to open source file with read permissions.
    // TODO: Add module to tokenize opened file.

    return SUCCESS_CODE;
}

