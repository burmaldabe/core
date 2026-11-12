#include "handling.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

static enum handling_status convert_context_status(enum context_status cs)
{
    enum handling_status hs; 
    switch (s) {
    case context_success:
        hs = handling_success;
    case context_database_not_opened:
        hs = handling_database_not_opened;
    case context_system_error:
        hs = handling_system_error;
    }
    return hs;
}

static enum handling_status handle_open(struct context *ctx,
    const struct command_arguments_open *args)
{
    enum context_status cs;
    switch(args->obj) {
    case command_object_database:
        cs = context_open_database(ctx, args->name);
        break;
    case command_object_table:
        cs = context_open_table(ctx, args->name);
        break;
    case command_object_column:
        return handling_trying_to_open_column;
    }
    return convert_context_status(cs);
}

static enum handling_status handle_create_database(const char name[])
{
    int err_occurs = mkdir(name, 0777);
    if (err_occurs)
        return handling_system_error;
    return handling_success;
}

static int create_directory_at(DIR *parent, const char name[])
{
    int fd = dirfd(parent);
    if (fd == -1)
        return 1;
    return mkdirat(fd, name, 0777);
}

static enum handling_status handle_create_table(const struct context *ctx,
    const char name[])
{
    int err_occurs;
    if (ctx->opened_database == NULL)
        return handling_database_not_opened;
    err_occurs = create_directory_at(ctx->opened_database, name);
    if (err_occurs)
        return handling_system_error;
    return handling_success;
}

static int handle_create_column(const struct context *ctx, const char name[],
    struct column_metadata md)
{
    int err_occurs, tabfd, values_fd, colfd, bytes_written;

    if (ctx->opened_table == NULL)
        return handling_table_not_opened;

    err_occurs = create_directory_at(ctx->opened_table, name);
    if (err_occurs)
        return handling_system_error;
    tabfd = dirfd(ctx->opened_table);
    if (tabfd == -1)
        return handling_system_error;
    colfd = openat(tabfd, name, O_DIRECTORY);
    if (colfd == -1)
        return handling_system_error;
    values_fd = openat(colfd, "values", O_CREAT | O_RDWR, 0666);
    if (values_fd == -1) {
        close(colfd);
        return handling_system_error;
    }
    bytes_written = write(values_fd, &md, sizeof(md));
    if (bytes_written == -1) {
        close(colfd);
        close(values_fd);
        return handling_system_error;
    }
    err_occurs = fdatasync(values_fd);
    if (err_occurs) {
        close(colfd);
        close(values_fd);
        return handling_system_error;
    }
    close(values_fd);
    close(colfd);
    return handling_success;
}

static enum handling_status handle_create(const struct context *ctx,
    const struct command_arguments_create *args)
{
    enum handling_status s;
    switch (args->obj) {
    case command_object_database:
        s = handle_create_database(args->name);
        break;
    case command_object_table:
        s = handle_create_table(ctx, args->name);
        break;
    case command_object_column:
        s = handle_create_column(ctx, args->name, args->md);
        break;
    }
    return s;
}

static enum handling_status handle_delete_database(const char name[])
{
    enum file_system_status s = file_system_recursive_delete(name);
    switch (s) {
    case file_system_success:
        break;
    case file_system_error:
        return handling_system_error;
    }
    return handling_success;
}

static enum handling_status handle_delete_table(const struct context *ctx,
    const char name[])
{
    if (ctx-> 
}

static enum handling_status handle_delete(const struct context *ctx,
    const struct command_arguments_delete *args)
{
    enum handling_status s;
    switch (args->obj) {
    case command_object_database:
        s = handle_delete_database(args->name);
        break;
    case command_object_table:
        s = handle_delete_table(ctx, args);
        break;
    case command_object_column:
        s = handle_delete_column(ctx, args);
        break;
    }
    return s;
}

enum handling_status handling_handle_command(struct context *ctx,
    const struct command *cmd)
{
    enum handling_status s;
    switch (cmd->t) {
    case command_type_open:
        s = handle_open(ctx, &cmd->args.open);
        break;
    case command_type_create:
        s = handle_create(ctx, &cmd->args.create);
        break;
    case command_type_delete:
        s = handle_delete(ctx, &cmd->args.delete);
        break;
    }
    return s;
}
