#include <stdio.h>
#include "file-handler.h"
#include "parser.h"
#include "xml-gen.h"

#define SUCCESS_CODE    0
#define ERROR_CODE      -1
#define ARGS_NUM        3

static FILE *jack_file_handle;
static FILE *xml_file_handle;

static int open_jack_file(const char *path);
static int create_output_file(const char *path);

int main(int argc, char **argv)
{
    if (argc < ARGS_NUM) {
        printf("Invalid source file argument\n");
        printf("Usage: JackAnalyzer source.jack output.xml\n");
        return ERROR_CODE;
    }

    if (open_jack_file(argv[1]) == ERROR_CODE) {
        return ERROR_CODE;
    }

    if (create_output_file(argv[2]) == ERROR_CODE) {
        return ERROR_CODE;
    }

    Parser_jack_syntax file_syntax = parser_parse(jack_file_handle);
    xml_gen(xml_file_handle, file_syntax);

    file_close(jack_file_handle);
    file_close(xml_file_handle);
    
    return SUCCESS_CODE;
}

static int open_jack_file(const char *path)
{
    jack_file_handle = file_open(path, false);

    if (jack_file_handle == NULL) {
        printf(
            "File %s couldn't be opened.\n", 
            path
        );
        return ERROR_CODE;
    }

    return SUCCESS_CODE;
}

static int create_output_file(const char *path)
{
    xml_file_handle = file_open(path, true);

    if (xml_file_handle == NULL) {
        printf(
            "File %s couldn't be created.\n", 
            path
        );
        return ERROR_CODE;
    }

    return SUCCESS_CODE;
}

