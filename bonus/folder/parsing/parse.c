/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 00:20:57 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/17 02:10:06 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/parsing.h"

void	reset(char *text)
{
	while (g_global.pids.pos > 0)
		get_last_exit_code();
	free(text);
	clear_list(&g_global.tokens);
	clear_list(&g_global.pids);
	clear_list(&g_global.fds);
}

void	evaluate_input(char *text)
{
	t_file	*input;
	t_file	*output;
	t_token	**tokens;
	t_node	*curr;

	input = new_file(NULL, IN, 0);
	output = new_file(NULL, OUT, 0);
	g_global.tokens.pos = 0;
	tokens = (t_token **)g_global.tokens.pointers;
	while (tokens[g_global.tokens.pos]->type != end_)
	{
		curr = expr();
		if (curr == NULL)
			break ;
		evaluate(curr, input, output);
	}
	reset(text);
}

void	handle_input(char *text)
{
	if (is_inclosed(text))
	{
		printf("minishell: syntax error\n");
		add_history(text);
		reset(text);
	}
	else
	{
		add_history(text);
		if (build_tokens(text) != SUCCESS)
			reset(text);
		else
			evaluate_input(text);
	}
}
