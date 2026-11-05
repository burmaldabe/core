#include "handling.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#define RETURN_ERROR(OUT, CODE) \
{ \
    if ((OUT) != NULL) \
        *(OUT) = (CODE); \
    return 1; \
}

static int handle_open(struct context *ctx,
    const struct command_arguments_open *args, enum handling_error *err)
{
    int error_occured;
    enum context_error ce;

    switch(args->object_code) {
    case command_object_database:
        error_occured = context_open_database(ctx, args->name, &ce);
        break;
    case command_object_table:
        error_occured = context_open_table(ctx, args->name, &ce);
        break;
    case command_object_column:
        RETURN_ERROR(err, handling_trying_to_open_column)
    }

    if (error_occured) {
        if (err != NULL)
            switch (ce) {
            case context_database_not_opened:
                *err = handling_database_not_opened;
                break;
            case context_system_error:
                *err = handling_system_error;
                break;
            }
        return 1;
    }
    
    return 0;
}

static int create_dir_at(DIR *parent, const char name[])
{
    int fd = dirfd(parent); 
    if (fd == -1)
        return 1;
    return mkdirat(fd, name, 0777);
}

static int handle_create_table(const struct context *ctx, const char name[],
    enum handling_error *err)
{
    int error_occured;
    if (ctx->opened_database == NULL)
        RETURN_ERROR(err, handling_database_not_opened);
    error_occured = create_dir_at(ctx->opened_database, name);
    if (error_occured)
        return 1;
    return 0;
}

static int handle_create_column(const struct context *ctx, const char name[],
    struct column_metadata md, enum handling_error *err)
{
    int error_occured, tabfd, values_fd, colfd, bytes_written;

    if (ctx->opened_table == NULL)
        RETURN_ERROR(err, handling_table_not_opened)

    error_occured = create_dir_at(ctx->opened_table, name);
    if (error_occured)
        return 1;

    tabfd = dirfd(ctx->opened_table);
    if (tabfd == -1)
        RETURN_ERROR(err, handling_system_error);

    colfd = openat(tabfd, name, O_DIRECTORY);
    if (colfd == -1)
        RETURN_ERROR(err, handling_system_error)

    values_fd = openat(colfd, "values", O_CREAT | O_RDWR, 0666);
    if (values_fd == -1) {
        close(colfd);
        RETURN_ERROR(err, handling_system_error)
    }

    bytes_written = write(values_fd, &md, sizeof(md));
    if (bytes_written == -1) {
        close(colfd);
        close(values_fd);
        RETURN_ERROR(err, handling_system_error)
    }

    error_occured = fdatasync(values_fd);
    if (error_occured) {
        close(colfd);
        close(values_fd);
        RETURN_ERROR(err, handling_system_error);
    }

    close(values_fd);
    close(colfd);
    return 0;
}

static int handle_create(const struct context *ctx,
    const struct command_arguments_create *args, enum handling_error *err)
{
    int error_occured;
    switch (args->object_code) {
    case command_object_database:
        error_occured = mkdir(args->name, 0777);
        if (error_occured)
            RETURN_ERROR(err, handling_system_error)
        break;
    case command_object_table:
        return handle_create_table(ctx, args->name, err);
    case command_object_column:
        return handle_create_column(ctx, args->name, args->metadata_codes, err);
    }
    return 0;
}

/*
static int handle_delete(const struct context *ctx,
    const struct command_arguments_delete *args, enum handling_error *err)
{
}
*/

int handling_do(struct context *ctx, const struct command *cmd,
    enum handling_error *err)
{
    switch (cmd->type_code) {
    case command_type_open:
        return handle_open(ctx, &cmd->arguments_passed.open_arguments,
            err);
    case command_type_create:
        return handle_create(ctx, &cmd->arguments_passed.create_arguments,
            err);
    case command_type_delete:
    /*
        return handle_delete(ctx, &cmd->arguments_passed.delete_arguments,
            err);
    */
        break;
    }
    return 0; 
}
