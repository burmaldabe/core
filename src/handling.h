#ifndef HANDLING_H_INCLUDED
#define HANDLING_H_INCLUDED

#include "command.h"
#include "context.h"

enum handling_error {
    handling_database_not_opened,
    handling_table_not_opened,
    handling_trying_to_open_column,
    handling_system_error
};

int handling_do(struct context *ctx, const struct command *cmd,
    enum handling_error *err);

#endif /* HANDLING_H_INCLUDED */
