#ifndef PARSING_H_INCLUDED
#define PARSING_H_INCLUDED

#include "command.h"

enum parsing_status {
    parsing_success,
    parsing_command_not_specified,
    parsing_unknown_command,
    parsing_object_not_specified,
    parsing_unknown_object,
    parsing_name_not_specified,
    parsing_type_not_specified,
    parsing_unknown_type,
    parsing_parameter_not_specified,
    parsing_invalid_parameter_value
};

struct parsing_error_details {
    const char *cmd_name;
    const char *arg_name;
};

enum parsing_status parsing_parse_command(char line[], struct command *out,
    struct parsing_error_details *err);

#endif /* PARSING_H_INCLUDED */
