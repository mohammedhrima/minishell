/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 03:08:03 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/17 11:16:42 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/builtin.h"

void	new_envirement_variable(char **arguments)
{
	t_node	*node;
	t_node	**envirement;
	char	**array;
	int		i;

	i = 0;
	array = split_by_two(arguments[0], '=');
	envirement = (t_node **)g_global.envirement.pointers;
	while (envirement && envirement[i])
	{
		if (ft_strcmp(envirement[i]->left->token->value, array[0]) == 0)
		{
			envirement[i]->right->token->value = array[1];
			return ;
		}
		i++;
	}
	node = new_node(new_token(assign_, NULL, 0));
	node->left = new_node(new_token(identifier_, NULL, 0));
	node->left->token->value = array[0];
	node->right = new_node(new_token(identifier_, NULL, 0));
	node->right->token->value = array[1];
	add_pointer(&g_global.envirement, node);
}

void	export_func(char **arguments)
{
	t_node	**envirement;
	int		i;

	if (arguments[0])
		new_envirement_variable(arguments);
	else
	{
		i = 0;
		envirement = (t_node **)g_global.envirement.pointers;
		while (envirement && envirement[i])
		{
			if (envirement[i]->left->token->value)
				printf("declare -x %s=\"%s\"\n",
					envirement[i]->left->token->value,
					envirement[i]->right->token->value);
			i++;
		}
	}
}
