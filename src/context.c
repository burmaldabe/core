#include "context.h"

#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

void context_free(const struct context *self)
{
    if (self->opened_database != NULL)
        closedir(self->opened_database);
    if (self->opened_table != NULL)
        closedir(self->opened_table);
}

enum context_status context_open_database(struct context *self,
    const char name[])
{
    DIR* db;

    if (self->opened_database != NULL)
        closedir(self->opened_database);
    db = opendir(name);
    if (db == NULL)
        return context_system_error;
    self->opened_database = db;
    return context_success;
}

enum context_status context_open_table(struct context *self,
    const char name[])
{
    int dbfd, tabfd;
    DIR *table;

    if (self->opened_database == NULL)
        return context_database_not_opened;
    if (self->opened_table != NULL)
        closedir(self->opened_table);
    dbfd = dirfd(self->opened_database);
    if (dbfd == -1)
        return context_system_error;
    tabfd = openat(dbfd, name, O_DIRECTORY);
    if (tabfd == -1)
        return context_system_error;
    table = fdopendir(tabfd);
    if (table == NULL) {
        close(tabfd);
        return context_system_error;
    }
    self->opened_table = table;
    return context_success;
}
