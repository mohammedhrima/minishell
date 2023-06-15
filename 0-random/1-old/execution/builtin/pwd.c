#include "builtin.h"

void pwd_func(char **arguments)
{
    char *new;

    arguments = NULL;
    new = getcwd(NULL, 0);
    printf("%s\n", new);
    free(new);
}
