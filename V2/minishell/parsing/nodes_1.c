/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nodes_1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 03:42:19 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/19 05:20:16 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../_headers/parsing.h"

t_node	*expr_node(void)
{
	return (logic_node());
}

t_node	*logic_node(void)
{
	t_node	*left;
	t_token	*token;
	t_node	*node;

	left = pipe_node();
	token = ((t_token **)g_global.tokens.pointers)[g_global.tokens.pos];
	while (left && (token->type == and_ || token->type == or_))
	{
		node = new_node(token);
		skip(token->type);
		node->left = left;
		node->right = pipe_node();
		left = node;
		token = ((t_token **)g_global.tokens.pointers)[g_global.tokens.pos];
	}
	return (left);
}

t_node	*pipe_node(void)
{
	t_node	*left;
	t_token	*token;
	t_node	*node;

	left = prime_node();
	token = ((t_token **)g_global.tokens.pointers)[g_global.tokens.pos];
	while (left && token->type == pipe_)
	{
		node = new_node(token);
		skip(pipe_);
		node->left = left;
		node->right = prime_node();
		left = node;
		token = ((t_token **)g_global.tokens.pointers)[g_global.tokens.pos];
	}
	return (left);
}

t_node	*prime_node(void)
{
	t_token	*token;
	t_node	*node;

	token = ((t_token **)g_global.tokens.pointers)[g_global.tokens.pos];
	if (token->type == identifier_
		|| token->type == star_
		|| token->type == redir_input
		|| token->type == redir_output)
	{
		node = new_node(token);
		node->token->start = g_global.tokens.pos;
		node->token->len++;
		skip(token->type);
		return (advance(node));
	}
	return (heredoc_node());
}

t_node	*heredoc_node(void)
{
	t_token	*token;
	t_node	*node;
	int		pid;

	token = ((t_token **)g_global.tokens.pointers)[g_global.tokens.pos];
	if (
		token->type == heredoc_)
	{
		node = new_node(token);
		skip(token->type);
		token = ((t_token **)g_global.tokens.pointers)[g_global.tokens.pos];
		if (token->type != identifier_)
		{
			pid = fork();
			add_number(&g_global.pids, pid);
			printf("minishell: syntax error unexpected '%s'\n", token->value);
			if (pid == 0)
				ft_exit(SYNTAX_ERROR);
			while (token->type != end_)
			{
				g_global.tokens.pos++;
				token = ((t_token **)g_global.tokens.pointers)
				[g_global.tokens.pos];
			}
			return (NULL);
		}
		node->token->value = open_heredoc(expand(token->value)); // check if is NULL
		skip(token->type);
		return (node);
	}
	return (parentses_node());
}
