#include "parsing.h"

#include <string.h>
#include <stdio.h>

const char delim[] = " \t\n";
const char open_cmd_name[] = "open";
const char create_cmd_name[] = "create";
const char delete_cmd_name[] = "delete";

const char database_obj_name[] = "database";
const char table_obj_name[] = "table";
const char column_obj_name[] = "column";

static void fill_error_details(const char cmd_name[],
    const char arg_name[], struct parsing_error_details *err)
{
    if (err != NULL) {
        err->cmd_name = cmd_name;
        err->arg_name = arg_name;
    }
}

/* Returns -1 if the command is not identified. */
static enum command_type identify_command(const char name[])
{
    if (0 == strcmp(name, open_cmd_name))
        return command_type_open;
    if (0 == strcmp(name, create_cmd_name))
        return command_type_create;
    return -1;
}

/* Returns -1 if the object is not identified. */
static enum command_object identify_object(const char name[])
{
    if (0 == strcmp(name, database_obj_name))
        return command_object_database;
    if (0 == strcmp(name, table_obj_name))
        return command_object_table;
    if (0 == strcmp(name, column_obj_name))
        return command_object_column;
    return -1;
}

/* Returns -1 if the column type is not identified. */
static enum column_type identify_column_type(const char name[])
{
    if (0 == strcmp(name, "small-integer"))
        return column_type_small_integer;
    if (0 == strcmp(name, "integer"))
        return column_type_integer;
    if (0 == strcmp(name, "big-integer"))
        return column_type_big_integer;
    if (0 == strcmp(name, "rational"))
        return column_type_rational;
    if (0 == strcmp(name, "big-rational"))
        return column_type_big_rational;
    if (0 == strcmp(name, "boolean"))
        return column_type_boolean;
    if (0 == strcmp(name, "char"))
        return column_type_char;
    return -1;
}

static enum parsing_status parse_command_name(char line[],
    enum command_type *out, struct parsing_error_details *err)
{
    const char *cmd_name;
    enum command_type cmd;

    cmd_name = strtok(line, delim);
    if (cmd_name == NULL) {
        fill_error_details(NULL, NULL, err);
        return parsing_command_not_specified;
    }
    cmd = identify_command(cmd_name);
    if ((int) cmd == -1) {
        fill_error_details(cmd_name, NULL, err);
        return parsing_unknown_command;
    }
    *out = cmd;
    return parsing_success;
}

static enum parsing_status parse_object_name(const char cmd_name[],
    enum command_object *out, struct parsing_error_details *err)
{
    const char *obj_name;
    enum command_object obj;

    obj_name = strtok(NULL, delim);
    if (obj_name == NULL) {
        fill_error_details(cmd_name, NULL, err);
        return parsing_object_not_specified;
    }
    obj = identify_object(obj_name);
    if ((int) obj == -1) {
        fill_error_details(cmd_name, obj_name, err);
        return parsing_unknown_object;
    }
    *out = obj;
    return parsing_success;
}

static enum parsing_status parse_open_command(
    struct command_arguments_open *out, struct parsing_error_details *err)
{
    const char *name;
    enum command_object obj;
    enum parsing_status s;

    s = parse_object_name(open_cmd_name, &obj, err);
    if (s != parsing_success)
        return s;
    name = strtok(NULL, delim);
    if (name == NULL) {
        fill_error_details(open_cmd_name, NULL, err);
        return parsing_name_not_specified;
    }
    out->name = name;
    out->obj = obj;
    return parsing_success;
}

static enum parsing_status parse_parameter(const char type_name[],
    unsigned int *out, struct parsing_error_details *err)
{
    const char *raw;
    unsigned int value;

    raw = strtok(NULL, delim);
    if (raw == NULL) {
        fill_error_details(create_cmd_name, type_name, err);
        return parsing_parameter_not_specified;
    }
    if (sscanf(raw, "%d", &value) != 1) {
        fill_error_details(create_cmd_name, raw, err);
        return parsing_invalid_parameter_value;
    }
    *out = value;
    return parsing_success;
}

static enum parsing_status parse_column_metadata(
    struct column_metadata *out, struct parsing_error_details *err)
{
    const char *type_name;
    enum column_type t;
    enum parsing_status s;

    type_name = strtok(NULL, delim);
    if (type_name == NULL) {
        fill_error_details(create_cmd_name, NULL, err);
        return parsing_type_not_specified;
    }
    t = identify_column_type(type_name);
    if ((int) t == -1) {
        fill_error_details(create_cmd_name, type_name, err);
        return parsing_unknown_type;
    }
    if (t == column_type_char) {
        s = parse_parameter(type_name, &out->parameter, err);
        if (s != parsing_success)
            return s;
    } else
        out->parameter = 0;
    out->t = t;
    return parsing_success;
}

static enum parsing_status parse_create_command(
    struct command_arguments_create *out, struct parsing_error_details *err)
{
    const char *name;
    enum command_object obj;
    enum parsing_status s;

    s = parse_object_name(create_cmd_name, &obj, err);
    if (s != parsing_success)
        return s;
    name = strtok(NULL, delim);
    if (name == NULL) {
        fill_error_details(create_cmd_name, NULL, err);
        return parsing_name_not_specified;
    }
    if (obj == command_object_column) {
        s = parse_column_metadata(&out->md, err);
        if (s != parsing_success)
            return s;
    }
    out->name = name;
    out->obj = obj;
    return parsing_success;
}

static enum parsing_status parse_delete_command(
    struct command_arguments_delete *out, struct parsing_error_details *err)
{
    const char *name;
    enum command_object obj;
    enum parsing_status s;

    s = parse_object_name(create_cmd_name, &obj, err);
    if (s != parsing_success)
        return s;
    name = strtok(NULL, delim);
    if (name == NULL) {
        fill_error_details(create_cmd_name, NULL, err);
        return parsing_name_not_specified;
    }
    out->name = name;
    out->obj = obj;
    return parsing_success;
}

enum parsing_status parsing_parse_command(char line[], struct command *out,
    struct parsing_error_details *err)
{
    enum command_type t;
    enum parsing_status s;

    s = parse_command_name(line, &t, err);
    if (s != parsing_success)
        return s;
    switch (t) {
    case command_type_open:
        s = parse_open_command(&out->args.open, err);
        break;
    case command_type_create:
        s = parse_create_command(&out->args.create, err);
        break;
    case command_type_delete:
        s = parse_delete_command(&out->args.delete, err);
        break;
    }
    if (s != parsing_success)
        return s;
    out->t = t;
    return parsing_success;
}
