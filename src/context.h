#ifndef CONTEXT_H_INCLUDED
#define CONTEXT_H_INCLUDED

#include <stdio.h>
#include <dirent.h>

/* So far, the command context is very primitive and doesn't cache/hold
   anything in memory. */
struct context {
    DIR *own_opened_database;
    DIR *own_opened_table;
};

enum context_status {
    context_success,
    context_database_not_opened,
    context_system_error
};

#define CONTEXT_INIT(SELF) \
{ \
    (SELF)->opened_database = NULL; \
    (SELF)->opened_table = NULL; \
}
void context_free(const struct context *self);

enum context_status context_open_database(struct context *self,
    const char name[]);
enum context_status context_open_table(struct context *self,
    const char name[]);

#endif /* CONTEXT_H_INCLUDED */
