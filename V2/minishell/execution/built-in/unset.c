/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 03:08:11 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/19 20:31:45 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../_headers/execution.h"

void	unset_func(char **argument)
{
	int		i;
	t_node	**envirement;
	char	*value;

	envirement = (t_node **)g_global.envirement.pointers;
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
}
