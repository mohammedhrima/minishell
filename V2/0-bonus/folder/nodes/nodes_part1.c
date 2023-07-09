/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nodes_part1.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 04:18:15 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/17 02:08:05 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/nodes.h"

t_node	*expr(void)
{
	return (and_or());
}

t_node	*and_or(void)
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
	if (token->type == identifier_ || token->type == star_
		|| is_redirection(token->type))
	{
		node = new_node(token);
		node->token->start = g_global.tokens.pos;
		node->token->len++;
		skip(token->type);
		return (advance(node));
	}
	return (parentses());
}
