/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 03:08:03 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/19 20:54:37 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../_headers/execution.h"

void new_envirement_variable(char **arguments)
{
	t_node *node;
	t_node **envirement;
	char **array;
	int i;
	int k;

	i = 0;
	while (arguments && arguments[i])
	{
		array = split_by_two(arguments[i], "=");
		envirement = (t_node **)g_global.envirement.pointers;
		k = 0;
		while (envirement && envirement[k])
		{
			if (ft_strcmp(envirement[k]->left->token->value, array[0]) == 0)
			{
				char *value = expand(array[1]);
				if(value)
					envirement[k]->right->token->value = value;
				break;
			}
			else
				
			k++;
		}
		if (envirement[k] == NULL)
		{
			node = new_node(new_token(assign_, NULL, 0));
			node->left = new_node(new_token(identifier_, NULL, 0));
			node->left->token->value = array[0];
			node->right = new_node(new_token(identifier_, NULL, 0));
			if(array[1])
				node->right->token->value = expand(array[1]);
			add_pointer(&g_global.envirement, node);
		}
		i++;
	}
}

void export_func(char **arguments)
{
	t_node **envirement;
	int i;

	if (arguments[0])
		new_envirement_variable(arguments);
	else
	{
		i = 0;
		envirement = (t_node **)g_global.envirement.pointers;
		while (envirement && envirement[i])
		{
			if (envirement[i]->left->token->value)
			{
				if(envirement[i]->right->token->value)
				{
					printf("declare -x %s=\"%s\"\n",
						envirement[i]->left->token->value,
						envirement[i]->right->token->value);
				}
				else
				{
					printf("declare -x %s\n",
						envirement[i]->left->token->value);
				}
			}
			i++;
		}
	}
}
