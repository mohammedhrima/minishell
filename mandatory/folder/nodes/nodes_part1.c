/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nodes_part1.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 04:18:15 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/17 02:50:17 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/nodes.h"

t_node	*expr(void)
{
	return (pipe_node());
}

t_node	*pipe_node(void)
{
	t_node	*left;
	t_token	*token;
	t_node	*node;

	left = prime();
	token = ((t_token **)g_global.tokens.pointers)[g_global.tokens.pos];
	while (left && token->type == pipe_)
	{
		node = new_node(token);
		skip(pipe_);
		node->left = left;
		node->right = prime();
		left = node;
		token = ((t_token **)g_global.tokens.pointers)[g_global.tokens.pos];
	}
	return (left);
}

t_node	*prime(void)
{
	t_token	*token;
	t_node	*node;

	token = ((t_token **)g_global.tokens.pointers)[g_global.tokens.pos];
	if (token->type == identifier_ || is_redirection(token->type))
	{
		node = new_node(token);
		node->token->start = g_global.tokens.pos;
		node->token->len++;
		skip(token->type);
		return (advance(node));
	}
	return (NULL);
}

t_node	*advance(t_node *node)
{
	t_token	*token;

	token = ((t_token **)g_global.tokens.pointers)[g_global.tokens.pos];
	while (token->type == identifier_ || is_redirection(token->type))
	{
		node->token->len++;
		skip(token->type);
		token = ((t_token **)g_global.tokens.pointers)[g_global.tokens.pos];
	}
	return (node);
}
