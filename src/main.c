#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "file-handler.h"
#include "parser.h"
#include "xml-gen.h"

#define SUCCESS_CODE    0
#define ERROR_CODE      -1
#define ARGS_NUM        2

static FILE *jack_file_handle   = NULL;
static FILE *xml_file_handle    = NULL;

static int open_jack_file(const char *path);
static int create_output_file(char *path);

int main(int argc, char **argv)
{
    if (argc < ARGS_NUM) {
        printf("Invalid project folder argument\n");
        printf("Usage: JackAnalyzer jack_proj_path\n");
        return ERROR_CODE;
    }

    File_handler_jack_proj proj = fh_open_proj(argv[1]);

    if (proj.failed) {
        printf("Couldn't open supplied jack directory\n");
        return ERROR_CODE;
    }

    for (int i = 0; i < proj.jack_files_count; i++) {
        char *file_path = proj.jack_files_paths[i];

        if (open_jack_file(file_path) == ERROR_CODE) {
            return ERROR_CODE;
        }

        if (create_output_file(file_path) == ERROR_CODE) {
            return ERROR_CODE;
        }
            
        Parser_jack_syntax file_syntax = parser_parse(jack_file_handle);
        xml_gen(xml_file_handle, file_syntax);
        parser_free(file_syntax);

        fh_close_file(jack_file_handle);
        fh_close_file(xml_file_handle);
    }

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
    bool did_rename = false;
    char *ext = ".jack";
    char *i_path = path;
    char ch;
    
    while ((ch = *i_path) != '\0') {
        if (strcmp(i_path, ext) == 0) {
            i_path[0] = '.';
            i_path[1] = 'x';
            i_path[2] = 'm';
            i_path[3] = 'l';
            i_path[4] = '\0';
            did_rename = true;
            break;
        }
        i_path += 1;
    }

    if (!did_rename) {
        printf("Unable to create xml path for %s\n", path);
        return ERROR_CODE;
    }

    xml_file_handle = fh_open_file(path, true);

    if (xml_file_handle == NULL) {
        printf(
            "File %s couldn't be created.\n", 
            path
        );
        return ERROR_CODE;
    }

    return SUCCESS_CODE;
}

