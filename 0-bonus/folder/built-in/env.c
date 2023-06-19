/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 03:07:57 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/18 20:35:05 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/builtin.h"

void	env_func(char **arguments)
{
	int		i;
	t_node	**envirement;

	arguments = NULL;
	envirement = (t_node **)g_global.envirement.pointers;
	i = 0;
	while (envirement[i])
	{
		if (envirement[i]->left->token->value && envirement[i]->right->token->value)
		{	
			printf("%s=%s\n", envirement[i]->left->token->value,
				envirement[i]->right->token->value);
		}
		i++;
	}
}
