#include "builtin.h"

void exit_func(char **arguments)
{
    printf("exit with %d\n", ft_atoi(arguments[0]));
    ft_exit(ft_atoi(arguments[0]));
}
