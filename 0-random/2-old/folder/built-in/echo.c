#include "built-in.h"

void echo_func(char **arguments)
{
    int print_new_line;
    int i;

    if (ft_strcmp(arguments[0], "-n") == 0)
    {
        print_new_line = 0;
        i = 1;
    }
    else
    {
        print_new_line = 1;
        i = 0;
    }
    while (arguments && arguments[i])
    {
        write(OUT, arguments[i], ft_strlen(arguments[i]));
        i++;
    }
    if (print_new_line)
        write(OUT, "\n", 1);
}
