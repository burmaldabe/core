#include "execution.h"

#include "parsing.h"
#include "handling.h"
#include "context.h"
#include <stdio.h>

#ifndef LINE_BUFFER_SIZE
#define LINE_BUFFER_SIZE 256
#endif

static const char *parsing_error_fmt(enum parsing_error err)
{
    switch (err) {
    case parsing_command_not_specified:
        return "";
    case parsing_unknown_command:
        return "%s: Unknown command\n";
    case parsing_object_not_specified:
        return "%s: Object not specified\n";
    case parsing_unknown_object:
        return "%s: %s: Unknown object\n";
    case parsing_name_not_specified:
        return "%s: Name not specified\n";
    case parsing_type_not_specified:
        return "%s: Type not specified\n"; 
    case parsing_unknown_type:
        return "%s: %s: Unknown type\n";
    case parsing_parameter_not_specified:
        return "%s: %s: Parameter not specified\n";
    case parsing_invalid_parameter_value:
        return "%s: %s: Invalid parameter value\n";
    }
    return NULL;
}

static const char *handling_error_str(enum handling_error err)
{
    switch (err) {
    case handling_database_not_opened:
        return "Database not opened\n";
    case handling_table_not_opened:
        return "Table not opened\n";
    case handling_trying_to_open_column:
        return "Trying to open column\n";
    case handling_system_error:
        return NULL;
    }
    return NULL;
}

int execution_do(FILE *commands)
{
    char line[LINE_BUFFER_SIZE];
    struct context ctx;

    CONTEXT_INIT(&ctx)
    line[LINE_BUFFER_SIZE - 1] = 0;
    while (fgets(line, LINE_BUFFER_SIZE, commands) != NULL) {
        struct command cmd;
        struct parsing_error_details ped;
        enum handling_error he;
        int error_occured;

        if (line[LINE_BUFFER_SIZE - 1] != 0) {
            fputs("Too long line", stderr);
            fflush(stderr);
            line[LINE_BUFFER_SIZE - 1] = 0;
            continue;
        }

        error_occured = parsing(line, &cmd, &ped);
        if (error_occured) {
            fprintf(stderr, parsing_error_fmt(ped.code), ped.command_name,
                ped.argument_name);
            fflush(stderr);
            continue;
        }

        error_occured = handling_do(&ctx, &cmd, &he);
        if (error_occured) {
            if (he == handling_system_error)
                perror(NULL);
            else
                fputs(handling_error_str(he), stderr);
            fflush(stderr);
            continue;
        }
    }
    context_free(&ctx);
    return ferror(commands) ? 1 : 0;
}
