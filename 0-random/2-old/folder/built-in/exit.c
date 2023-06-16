#include "built-in.h"

void	exit_func(char **arguments)
{
	ft_exit(ft_atoi(arguments[0]));
}
