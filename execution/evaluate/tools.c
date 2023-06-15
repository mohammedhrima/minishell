/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/15 02:07:54 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/15 02:10:03 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "evaluate.h"

int	skip(t_type type)
{
	t_token	*token;
	int		pid;

	token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
	if (token->type != type)
	{
		printf("minishell: syntax error expected '%s'\n", type_to_string(type));
		return (SYNTAX_ERROR);
	}
	global.tokens.pos++;
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

bool	is_redirection(t_type type)
{
	return (type == redir_input || type == redir_output || type == heredoc_
		|| type == append_);
}

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

t_node	*advance(t_node *node)
{
	t_token	*token;

	token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
	while (token->type == identifier_ || token->type == star_
		|| is_redirection(token->type))
	{
		node->token->len++;
		skip(token->type);
		token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
	}
	return (node);
}
