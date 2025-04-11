#include <stdbool.h>
#include <stdio.h>
#include <dirent.h>

typedef struct {
    DIR *handle;
    int jack_files_count;
    char **jack_files_paths;
    bool failed;
} File_handler_jack_proj;

FILE *fh_open_file(const char *path, const bool create);
void fh_write(const char *str, FILE *file);
void fh_close_file(FILE *file);

File_handler_jack_proj fh_open_proj(const char *path);
void fh_close_proj(File_handler_jack_proj *proj);
