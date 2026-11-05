#include "context.h"

#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>

void context_free(const struct context *self)
{
    if (self->opened_database != NULL)
        closedir(self->opened_database);
    if (self->opened_table != NULL)
        closedir(self->opened_table);
}

int context_open_database(struct context *self, const char name[],
    enum context_error *err)
{
    DIR* db;
    if (self->opened_database != NULL)
        closedir(self->opened_database);
    db = opendir(name);
    if (db == NULL) {
        if (err != NULL)
            *err = context_system_error;
        return 1;
    }
    self->opened_database = db;
    return 0;
}

int context_open_table(struct context *self, const char name[],
    enum context_error *err)
{
    int dbfd, tabfd;
    DIR *table;

    if (self->opened_database == NULL) {
        if (err != NULL)
            *err = context_database_not_opened;
        return 1;
    }

    if (self->opened_table != NULL)
        closedir(self->opened_table);

    dbfd = dirfd(self->opened_database);
    if (dbfd == -1) {
        if (err != NULL)
            *err = context_system_error;
        return 1;
    }

    tabfd = openat(dbfd, name, O_DIRECTORY);
    if (tabfd == -1) {
        if (err != NULL)
            *err = context_system_error;
        return 1;
    }

    table = fdopendir(tabfd);
    if (table == NULL) {
        if (err != NULL)
            *err = context_system_error;
        return 1;
    }

    self->opened_table = table;
    return 0;
}
