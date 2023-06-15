#include "built-in.h"

void	unset_func(char **argument)
{
	int		i;
	t_node	**envirement;
	char	*value;

	envirement = (t_node **)global.envirement.pointers;
	i = 0;
	while (envirement && envirement[i])
	{
		value = envirement[i]->left->token->value;
		if (value && ft_strcmp(value, argument[0]) == 0)
		{
			envirement[i]->left->token->value = NULL;
			envirement[i]->right->token->value = NULL;
			break ;
		}
		i++;
	}
	if (ft_strcmp(argument[0], "PATH") == 0)
		global.path = NULL;
}
