#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

int recursive_delete(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return -1;
    }

    struct dirent *entry;
    struct stat statbuf;
    char fullpath[4096];
    int ret = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        if (lstat(fullpath, &statbuf) == -1) {
            perror("lstat");
            ret = -1;
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            if (recursive_delete(fullpath) == -1)
                ret = -1;
        } else {
            if (unlink(fullpath) == -1) {
                perror("unlink");
                ret = -1;
            }
        }
    }

    closedir(dir);

    if (ret == 0 && rmdir(path) == -1) {
        perror("rmdir");
        ret = -1;
    }

    return ret;
}
