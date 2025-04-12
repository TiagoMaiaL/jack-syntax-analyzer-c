#include <stdlib.h>
#include <string.h>
#include "file-handler.h"

static bool is_jack_file(const char *path);

FILE *fh_open_file(const char *path, const bool create)
{
    return fopen(path, create ? "w" : "r");
}

void fh_write(const char *str, FILE *file)
{
    fprintf(file, "%s", str);
}

void fh_close_file(FILE *file)
{
    fclose(file);
}

File_handler_jack_proj fh_open_proj(const char *path)
{
    File_handler_jack_proj jack_proj;
    jack_proj.handle = opendir(path);
    jack_proj.jack_files_count = 0;
    jack_proj.jack_files_paths = NULL;
    jack_proj.failed = false;

    if (jack_proj.handle == NULL) {
        if (is_jack_file(path)) {
            jack_proj.jack_files_count = 1;
            jack_proj.jack_files_paths = malloc(sizeof(char *));
            jack_proj.jack_files_paths[0] = (char *)path;
        } else {
            jack_proj.failed = true;
        }
        return jack_proj;
    }

    struct dirent *entry;
    while ((entry = readdir(jack_proj.handle)) != NULL) {
        if (entry->d_type == DT_REG && is_jack_file(entry->d_name)) {
            jack_proj.jack_files_count += 1;
        }
    }

    if (jack_proj.jack_files_count == 0) {
        closedir(jack_proj.handle);
        jack_proj.handle = NULL;
        jack_proj.failed = true;
        return jack_proj;
    }

    jack_proj.jack_files_paths = malloc(sizeof(char *) * jack_proj.jack_files_count);
    rewinddir(jack_proj.handle);
    entry = NULL;
    int i = 0;

    while ((entry = readdir(jack_proj.handle)) != NULL) {
        if (!is_jack_file(entry->d_name)) {
            continue;
        }

        int path_size = 2;
        path_size += strlen(path);
        path_size += 1; // slash '/'
        path_size += strlen(entry->d_name);
        path_size += 1;

        char *full_path = malloc(sizeof(char) * path_size);
        sprintf(full_path, "./%s/%s", path, entry->d_name);

        jack_proj.jack_files_paths[i] = full_path;

        i++;
    }

    return jack_proj;
}

void fh_close_proj(File_handler_jack_proj *proj)
{
    if (proj->handle != NULL)
        closedir(proj->handle);

    proj->handle = NULL;
}

static bool is_jack_file(const char *path)
{
    char *ext = ".jack";
    char *name = (char *)path;
    char ch;

    while ((ch = *name) != '\0') {
        if (strcmp(name, ext) == 0) {
            return true;
        }
        name += 1;
    }

    return false;
}

