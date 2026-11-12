#include <stdio.h>
#include "execution.h"

int main(int argc, const char *argv[])
{
    (void) args;
    enum execution_status s = execution_execute_commands(stdin, argv[0]);
    switch (s) {
    case execution_success:
        break;
    case execution_system_error:
        perror(argv[0]);
    }
    return 0;
}
