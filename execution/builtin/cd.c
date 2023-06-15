#include "builtin.h"

void cd_func(char **arguments)
{
    if (arguments[0])
        chdir(arguments[0]);
    else
        chdir(getenv("HOME"));
}