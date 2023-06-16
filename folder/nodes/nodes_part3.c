/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nodes_part3.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 04:18:34 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 04:34:47 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/nodes.h"

int	check_token_type(t_token *token)
{
	int	pid;
	int	pos;

	if (token->type == identifier_ || token->type == star_)
	{
		pid = fork();
		add_number(&global.pids, pid);
		printf("minishell: syntax error unexpected '%s'\n", token->value);
		if (pid == 0)
			ft_exit(SYNTAX_ERROR);
		pos = global.tokens.pos;
		while (((t_token **)global.tokens.pointers)[pos]->type != end_)
			pos++;
		global.tokens.pos = pos;
		return (SYNTAX_ERROR);
	}
	return (SUCCESS);
}
