#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "fs.h"

char **list_directory(char *path) {
char *output;
char **files;
    DIR *dir;
size_t path_len;
int i;
int ent_len;
struct dirent *ent;
    i = 0;
    path_len = 0;
    files = malloc(MAX_FILES * sizeof(*files));
memset(files, 0, MAX_FILES * sizeof(*files));
if (path != NULL) {
        path_len = strlen(path);
if (path_len > 0) {
            dir = opendir(path);
if (dir) {
while ((ent = readdir(dir)) != NULL) {
                    ent_len = strlen(ent->d_name);
files[i] = malloc((ent_len + 1) * sizeof(char));
memset(files[i], 0, ent_len + 1);
strcat(files[i], ent->d_name);
if (i >= MAX_FILES - 1) {
files[i] = malloc(sizeof(char) * 62);
strncat(files[i], "Error: more than 1024 files to display. Some will be skipped.", 62);
closedir(dir);
return files;
                    }
                    i++;
                }
closedir(dir);
            } else {
                output = malloc((24 + path_len) * sizeof(char));
memset(output, 0, (24 + path_len) * sizeof(char));
strncat(output, "Error: Cannot open dir ", 24);
strncat(output, path, path_len);
files[0] = output;
            }
        }
    }
if (dir == NULL && path_len > 0) {
        output = malloc((36 + path_len) * sizeof(char));
memset(output, 0, (36 + path_len) * sizeof(char));
strncat(output, "Error: Invalid directory specified ", 36);
strncat(output, path, path_len);
files[0] = output;
    }
return files;
}
char *read_file(char *path) {
    FILE* fd;
char buffer[BUFFER_SIZE];
char *output;
long file_len;
int n;
if (path == NULL) {
printf("filesystem::read_file - Provided path is null!\n");
return NULL;
    }
    fd = fopen(path, "rb");
if (fd == NULL) {
printf("filesystem::read_file - Cannot read file\n");
return NULL;
    }
fseek(fd, 0, SEEK_END);
    file_len = ftell(fd);
rewind(fd);
    output = malloc(sizeof(char) * (file_len + 1));
    n = fread(output, 1, file_len, fd);
output[file_len] = '\0';
fclose(fd);
return output;
}
int write_file(const char *data, const char *path) {
    FILE *fd;
int output;
    fd = fopen(path, "wb+");
if (fd == NULL) {
printf("filesystem::write_file - Cannot open path %s\n", path);
return -1;
    }
    output = fputs(data, fd);
if (output == EOF) {
printf("filesystem::write_file - Write error on %s\n", path);
return -2;
    }
fclose(fd);
return output;
}