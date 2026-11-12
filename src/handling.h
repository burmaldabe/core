#ifndef HANDLING_H_INCLUDED
#define HANDLING_H_INCLUDED

#include "command.h"
#include "context.h"

enum handling_status {
    handling_success,
    handling_database_not_opened,
    handling_system_error,
    handling_table_not_opened,
    handling_trying_to_open_column
};

enum handling_status handling_handle_command(struct context *ctx,
    const struct command *cmd);

#endif /* HANDLING_H_INCLUDED */
