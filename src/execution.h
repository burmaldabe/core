#ifndef EXECUTION_H_INCLUDED
#define EXECUTION_H_INCLUDED

#include <stdio.h>

enum execution_status {
    execution_success,
    execution_system_error
};

enum execution_status execution_execute_commands(FILE *commands,
    const char log_prefix[]);

#endif /* EXECUTION_H_INCLUDED */
