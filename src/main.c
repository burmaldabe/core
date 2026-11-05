#include <stdio.h>
#include "execution.h"

int main()
{
    int error_occured = execution_do(stdin);
    if (error_occured) {
        perror(NULL);
        return 1;
    }
    return 0;
}
