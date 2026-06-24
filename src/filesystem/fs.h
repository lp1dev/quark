#ifndef FS_H_
#define FS_H_

#define MAX_FILES 1024
#define BUFFER_SIZE 512

char **list_directory(char *path);
char *read_file(char *path);
int write_file(const char *data, const char *path);

#endif /* FS_H_ */
