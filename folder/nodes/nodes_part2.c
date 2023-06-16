/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nodes_part2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 04:18:34 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 04:34:43 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/nodes.h"

t_node	*parentses(void)
{
	t_token	*token;
	t_node	*node;

	token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
	if (token->type == lparent_)
	{
		node = new_node(token);
		skip(token->type);
		node->left = expr();
		token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
		if (skip(rparent_))
			return (NULL);
		token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
		if (check_token_type(token))
			return (NULL);
		node->token->start = global.tokens.pos;
		return (advance(node));
	}
	return (NULL);
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

int	skip(t_type type)
{
	t_token	*token;

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
