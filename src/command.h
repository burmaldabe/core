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
    enum command_object object_code;
};

struct command_arguments_create {
    const char *name;
    enum command_object object_code;
    struct column_metadata metadata_codes;
};

struct command_arguments_delete {
    const char *name;
    enum command_object object_code;
};

union command_arguments {
    struct command_arguments_open open_arguments;
    struct command_arguments_create create_arguments;
    struct command_arguments_delete delete_arguments;
};

enum command_type {
    command_type_open,
    command_type_create,
    command_type_delete
};

struct command  {
    enum command_type type_code;
    union command_arguments arguments_passed;
};

#endif /* COMMAND_H_INCLUDED */
