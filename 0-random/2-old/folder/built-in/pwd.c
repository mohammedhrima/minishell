#include "built-in.h"

void	pwd_func(char **arguments)
{
	char	*new;

	if(arguments || arguments == NULL)
	{
		new = getcwd(NULL, 0);
		printf("%s\n", new);
		free(new);
	}
}
