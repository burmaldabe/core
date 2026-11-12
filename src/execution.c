#include "execution.h"

#include "parsing.h"
#include "handling.h"
#include "context.h"
#include <stdio.h>

#ifndef LINE_BUFFER_SIZE
#define LINE_BUFFER_SIZE 256
#endif

static const char *get_parsing_status_log(enum parsing_status s)
{
    switch (s) {
    case parsing_success:
        return NULL;
    case parsing_command_not_specified:
        return NULL;
    case parsing_unknown_command:
        return "%s: %s: Unknown command\n";
    case parsing_object_not_specified:
        return "%s: %s: Object not specified\n";
    case parsing_unknown_object:
        return "%s: %s: %s: Unknown object\n";
    case parsing_name_not_specified:
        return "%s: %s: Name not specified\n";
    case parsing_type_not_specified:
        return "%s: %s: Type not specified\n"; 
    case parsing_unknown_type:
        return "%s: %s: %s: Unknown type\n";
    case parsing_parameter_not_specified:
        return "%s: %s: %s: Parameter not specified\n";
    case parsing_invalid_parameter_value:
        return "%s: %s: %s: Invalid parameter value\n";
    }
    return NULL;
}

static const char *get_handling_status_log(enum handling_status s)
{
    switch (s) {
    case handling_success:
        return NULL;
    case handling_database_not_opened:
        return "%s: Database not opened\n";
    case handling_table_not_opened:
        return "%s: Table not opened\n";
    case handling_trying_to_open_column:
        return "%s: Trying to open column\n";
    case handling_system_error:
        return NULL;
    }
    return NULL;
}

static void handle_line(char buffer[], int size, struct context *ctx,
    const char log_prefix[])
{
    struct command cmd;
    enum parsing_status ps;
    enum handling_status hs;
    struct parsing_error_details ped;

    if (buffer[size - 1] != 0) {
        fprintf(stderr, "%s: Too long line", log_prefix);
        fflush(stderr);
        buffer[size - 1] = 0;
        return;
    }
    ps = parsing_parse_command(buffer, &cmd, &ped);
    if (ps != parsing_success) {
        fprintf(stderr, get_parsing_status_log(ps), log_prefix,
            ped.cmd_name, ped.arg_name);
        fflush(stderr);
        return;
    }
    hs = handling_handle_command(ctx, &cmd);
    if (hs != handling_success) {
        if (hs == handling_system_error)
            perror(log_prefix);
        else
            fprintf(stderr, get_handling_status_log(hs), log_prefix);
        fflush(stderr);
        return;
    }
}

enum execution_status execution_execute_commands(FILE *commands,
    const char log_prefix[])
{
    char line[LINE_BUFFER_SIZE];
    struct context ctx;

    CONTEXT_INIT(&ctx)
    line[sizeof(line) - 1] = 0;
    while (fgets(line, sizeof(line), commands) != NULL)
        handle_line(line, sizeof(line), &ctx, log_prefix);
    context_free(&ctx);
    return ferror(commands) ? execution_system_error : execution_success;
}
