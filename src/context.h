#ifndef CONTEXT_H_INCLUDED
#define CONTEXT_H_INCLUDED

#include <stdio.h>
#include <dirent.h>

enum context_error {
    context_database_not_opened,
    context_system_error
};

/* So far, the command context is very primitive and doesn't cache/hold
   anything in memory. */
struct context {
    DIR *opened_database;
    DIR *opened_table;
};

#define CONTEXT_INIT(SELF) \
    { (SELF)->opened_database = NULL; (SELF)->opened_table = NULL; }
void context_free(const struct context *self);

int context_open_database(struct context *self, const char name[],
    enum context_error *err);

int context_open_table(struct context *self, const char name[],
    enum context_error *err);

#endif /* CONTEXT_H_INCLUDED */
