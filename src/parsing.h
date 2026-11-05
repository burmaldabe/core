#ifndef PARSING_H_INCLUDED
#define PARSING_H_INCLUDED

#include "command.h"

enum parsing_error {
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
    const char *command_name;
    const char *argument_name;
    enum parsing_error code;
};

int parsing(char line[], struct command *output,
    struct parsing_error_details *err);

#endif /* PARSING_H_INCLUDED */
