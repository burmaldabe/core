#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED

#include "column.h"

enum command_object {
    command_object_database,
    command_object_table,
    command_object_column
};

struct command_arguments_open {
    const char *name;
    enum command_object obj;
};

struct command_arguments_create {
    const char *name;
    enum command_object obj;
    struct column_metadata md;  /* In case the objects is a column. */
};

struct command_arguments_delete {
    const char *name;
    enum command_object obj;
};

union command_arguments {
    struct command_arguments_open open;
    struct command_arguments_create create;
    struct command_arguments_delete delete;
};

enum command_type {
    command_type_open,
    command_type_create,
    command_type_delete
};

struct command {
    enum command_type t;
    union command_arguments args;
};

#endif /* COMMAND_H_INCLUDED */
