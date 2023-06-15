#include "builtin.h"

void echo_func(char **arguments)
{
    int print_new_line;
    int i;

    print_new_line = 1, i = 0;
    while (arguments && arguments[i])
    {
        if (i == 0 && ft_strcmp(arguments[i], "-n") == 0)
            print_new_line = 0;
        else
            printf("%s", arguments[i]);
        i++;
    }
    if (print_new_line)
        printf("\n");
}