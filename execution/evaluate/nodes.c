/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nodes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/15 02:07:38 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/15 02:07:39 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "evaluate.h"

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
	token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
	while (left && (token->type == and_ || token->type == or_))
	{
		node = new_node(token);
		skip(token->type);
		node->left = left;
		node->right = pipe_node();
		left = node;
		token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
	}
	return (left);
}

t_node	*pipe_node(void)
{
	t_node	*left;
	t_token	*token;
	t_node	*node;

	left = prime();
	token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
	while (left && token->type == pipe_)
	{
		node = new_node(token);
		skip(pipe_);
		node->left = left;
		node->right = prime();
		left = node;
		token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
	}
	return (left);
}

t_node	*prime(void)
{
	t_token	*token;
	t_node	*node;

	token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
	if (token->type == identifier_ || token->type == star_
		|| is_redirection(token->type))
	{
		node = new_node(token);
		node->token->start = global.tokens.pos;
		node->token->len++;
		skip(token->type);
		return (advance(node));
	}
	return (parentses());
}

t_node	*parentses(void)
{
	t_token	*token;
	t_node	*node;
	int		pid;

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
