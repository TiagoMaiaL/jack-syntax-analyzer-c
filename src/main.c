#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "file-handler.h"
#include "parser.h"
#include "code-gen.h"

#define SUCCESS_CODE    0
#define ERROR_CODE      -1
#define ARGS_NUM        3

static FILE *jack_file_handle       = NULL;
static FILE *code_file_handle       = NULL;

static int open_jack_file(const char *path);
static int create_output_file(char *path);

int main(int argc, char **argv)
{
    if (argc < ARGS_NUM) {
        printf("Invalid project folder argument\n");
        printf("Usage: JackAnalyzer jack_proj_path vm_output_file_path\n");
        return ERROR_CODE;
    }

    File_handler_jack_proj proj = fh_open_proj(argv[1]);

    if (proj.failed) {
        printf("Couldn't open supplied jack directory\n");
        return ERROR_CODE;
    }

    if (create_output_file(argv[2]) == ERROR_CODE) {
        return ERROR_CODE;
    }

    for (int i = 0; i < proj.jack_files_count; i++) {
        char *file_path = proj.jack_files_paths[i];

        if (open_jack_file(file_path) == ERROR_CODE) {
            return ERROR_CODE;
        }

        Parser_jack_syntax file_syntax = parser_parse(jack_file_handle);
        cg_gen_code(code_file_handle, &file_syntax);
        parser_free(file_syntax);

        fh_close_file(jack_file_handle);
    }

    fh_close_file(code_file_handle);
    fh_close_proj(&proj);
    
    return SUCCESS_CODE;
}

static int open_jack_file(const char *path)
{
    jack_file_handle = fh_open_file(path, false);

    if (jack_file_handle == NULL) {
        printf(
            "File %s couldn't be opened.\n", 
            path
        );
        return ERROR_CODE;
    }

    return SUCCESS_CODE;
}

static int create_output_file(char *path)
{
    code_file_handle = fh_open_file(path, true);

    if (code_file_handle == NULL) {
        printf(
            "File %s couldn't be created.\n", 
            path
        );
        return ERROR_CODE;
    }

    return SUCCESS_CODE;
}

