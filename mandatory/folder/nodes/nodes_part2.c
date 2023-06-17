/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nodes_part2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 04:18:34 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/17 02:38:34 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/nodes.h"

int	skip(t_type type)
{
	t_token	*token;

	token = ((t_token **)g_global.tokens.pointers)[g_global.tokens.pos];
	if (token->type != type)
	{
		printf("minishell: syntax error expected '%s'\n", type_to_string(type));
		return (SYNTAX_ERROR);
	}
	g_global.tokens.pos++;
	return (SUCCESS);
}

bool	check(t_type type, t_type types[])
{
	int	i;

	i = 0;
	while (types[i])
	{
		if (types[i] == type)
			return (true);
		i++;
	}
	return (false);
}
