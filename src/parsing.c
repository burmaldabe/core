#include "parsing.h"

#include <string.h>
#include <stdio.h>

const char delim[] = " \t\n";
const char open_command_name[] = "open";
const char create_command_name[] = "create";

const char database_object_name[] = "database";
const char table_object_name[] = "table";
const char column_object_name[] = "column";

static void fill_error_details(const char command_name[],
    const char argument_name[], enum parsing_error code,
    struct parsing_error_details *err)
{
    if (err != NULL) {
        err->command_name = command_name;
        err->argument_name = argument_name;
        err->code = code;
    }
}

static int identified_command(const char name[], enum command_type *out)
{
    if (0 == strcmp(name, open_command_name))
        *out = command_type_open;
    else if (0 == strcmp(name, create_command_name))
        *out = command_type_create;
    else
        return 1;
    return 0;
}

static int identified_object(const char name[], enum command_object *out)
{
    if (0 == strcmp(name, database_object_name))
        *out = command_object_database;
    else if (0 == strcmp(name, table_object_name))
        *out = command_object_table;
    else if (0 == strcmp(name, column_object_name))
        *out = command_object_column;
    else
        return 1;
    return 0;
}

static int identified_column_type(const char name[], enum column_type *out)
{
    if (0 == strcmp(name, "small-integer"))
        *out = column_type_small_integer;
    else if (0 == strcmp(name, "integer"))
        *out = column_type_integer;
    else if (0 == strcmp(name, "big-integer"))
        *out = column_type_big_integer;
    else if (0 == strcmp(name, "rational"))
        *out = column_type_rational;
    else if (0 == strcmp(name, "big-rational"))
        *out = column_type_big_rational;
    else if (0 == strcmp(name, "boolean"))
        *out = column_type_boolean;
    else if (0 == strcmp(name, "char"))
        *out = column_type_char;
    else
        return 1;
    return 0;
}

static int command_name_parsing(char line[], enum command_type *out,
    struct parsing_error_details *err)
{
    const char *word;
    enum command_type type_code;
    int error_occured;

    word = strtok(line, delim);
    if (word == NULL) {
        fill_error_details(NULL, NULL, parsing_command_not_specified, err);
        return 1;
    }

    error_occured = identified_command(word, &type_code);
    if (error_occured) {
        fill_error_details(word, NULL, parsing_unknown_command, err);
        return 1;
    }

    *out = type_code;
    return 0;
}

static int object_name_parsing(const char command_name[],
    enum command_object *out, struct parsing_error_details *err)
{
    const char *word;
    enum command_object object_code;
    int error_occured;

    word = strtok(NULL, delim);
    if (word == NULL) {
        fill_error_details(command_name, NULL,
            parsing_object_not_specified, err);
        return 1;
    }

    error_occured = identified_object(word, &object_code);
    if (error_occured) {
        fill_error_details(command_name, word, parsing_unknown_object, err);
        return 1;
    }

    *out = object_code;
    return 0;
}

static int open_command_parsing(struct command_arguments_open *out,
    struct parsing_error_details *err)
{
    const char *name;
    enum command_object object_code;
    int error_occured;

    error_occured = object_name_parsing(open_command_name, &object_code,
        err);
    if (error_occured)
        return 1;

    name = strtok(NULL, delim);
    if (name == NULL) {
        fill_error_details(open_command_name, NULL,
            parsing_name_not_specified, err);
        return 1;
    }

    out->name = name;
    out->object_code = object_code;
    return 0;
}

static int column_metadata_parsing(struct column_metadata *out,
    struct parsing_error_details *err)
{
    const char *type_name;
    enum column_type type_code;
    int error_occured;

    type_name = strtok(NULL, delim);
    if (type_name == NULL) {
        fill_error_details(create_command_name, NULL,
            parsing_type_not_specified, err);
        return 1;
    }

    error_occured = identified_column_type(type_name, &type_code);
    if (error_occured) {
        fill_error_details(create_command_name, type_name,
            parsing_unknown_type, err);
        return 1;
    }

    if (type_code == column_type_char) {
        const char *parameter = strtok(NULL, delim);
        unsigned int value;
        if (parameter == NULL) {
            fill_error_details(create_command_name, type_name,
                parsing_parameter_not_specified, err);
            return 1;
        }
        if (sscanf(parameter, "%d", &value) != 1) {
            fill_error_details(create_command_name, parameter,
                parsing_invalid_parameter_value, err);
            return 1;
        }
        out->parameter = value;
    } else
        out->parameter = 0;
    out->type_code = type_code;

    return 0;
}

static int create_command_parsing(struct command_arguments_create *out,
    struct parsing_error_details *err)
{
    const char *name;
    enum command_object object_code;
    int error_occured;

    error_occured = object_name_parsing(create_command_name, &object_code,
        err);
    if (error_occured)
        return 1;

    name = strtok(NULL, delim);
    if (name == NULL) {
        fill_error_details(create_command_name, NULL,
            parsing_name_not_specified, err);
        return 1;
    }
    
    if (object_code == command_object_column) {
        error_occured = column_metadata_parsing(&out->metadata_codes, err);
        if (error_occured)
            return 1;
    }

    out->name = name;
    out->object_code = object_code;
    return 0;
}

int parsing(char line[], struct command *out,
    struct parsing_error_details *err)
{
    enum command_type type_code;
    int error_occured;

    error_occured = command_name_parsing(line, &type_code, err);
    if (error_occured)
        return 1;

    switch (type_code) {
    case command_type_open:
        error_occured = open_command_parsing(
            &out->arguments_passed.open_arguments, err);
        break;
    case command_type_create:
        error_occured = create_command_parsing(
            &out->arguments_passed.create_arguments, err);
        break;
    case command_type_delete:
        break;
    }
    if (error_occured)
        return 1;

    out->type_code = type_code;
    return 0;
}
