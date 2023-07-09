/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nodes_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 03:42:05 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/19 05:28:38 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../_headers/parsing.h"

t_node	*parentses_node(void)
{
	t_token	*token;
	t_node	*node;

	token = ((t_token **)g_global.tokens.pointers)[g_global.tokens.pos];
	if (token->type == lparent_)
	{
		node = new_node(token);
		skip(token->type);
		node->left = expr_node();
		token = ((t_token **)g_global.tokens.pointers)[g_global.tokens.pos];
		if (skip(rparent_))
			return (NULL);
		token = ((t_token **)g_global.tokens.pointers)[g_global.tokens.pos];
		if (check_token_type(token))
			return (NULL);
		node->token->start = g_global.tokens.pos;
		return (advance(node));
	}
	return (NULL);
}

t_node	*advance(t_node *node)
{
	t_token	*token;

	token = ((t_token **)g_global.tokens.pointers)[g_global.tokens.pos];
	while (token->type == identifier_
		|| token->type == star_
		|| is_redirection(token->type)
		)
	{
		node->token->len++;
		skip(token->type);
		token = ((t_token **)g_global.tokens.pointers)[g_global.tokens.pos];
	}
	return (node);
}

t_node	*new_node(t_token *token)
{
	t_node	*new;

	new = ft_calloc(1, sizeof(t_node));
	new->token = token;
	return (new);
}

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

int	check_token_type(t_token *token)
{
	int	pid;
	int	pos;

	if (token->type == identifier_ || token->type == star_) // must be verified
	{
		pid = fork();
		add_number(&g_global.pids, pid);
		printf("minishell: syntax error unexpected '%s'\n", token->value);
		if (pid == 0)
			ft_exit(SYNTAX_ERROR);
		pos = g_global.tokens.pos;
		while (((t_token **)g_global.tokens.pointers)[pos]->type != end_)
			pos++;
		g_global.tokens.pos = pos;
		return (SYNTAX_ERROR);
	}
	return (SUCCESS);
}
