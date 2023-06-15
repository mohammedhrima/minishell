/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parenteses.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/15 11:31:55 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/15 12:49:02 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "evaluate.h"

static void	check_imbegious_redirection(t_token *token, int pos, t_file *input,
		t_file *output)
{
	t_type	type;
	char	*filename;

	type = token->type;
	token = ((t_token **)global.tokens.pointers)[pos];
	if (token->type == star_)
	{
		printf("minishell: * ambiguous redirection\n");
		ft_exit(AMBIGIOUS_REDIRECTION);
	}
	else if (token->type != identifier_)
	{
		printf("minishell: Unexpected symbol after redirection\n");
		ft_exit(UNEXPECTED_ERROR);
	}
	filename = expand(token->value);
	if (type == redir_input)
		*input = *new_file(filename, NOT_OPENED, type);
	if (type == redir_output || type == append_)
		*output = *new_file(filename, NOT_OPENED, type);
	if (type == heredoc_)
		*input = *open_heredoc(filename, input);
}

void	output_redirection(t_node *node, t_file *input, t_file *output)
{
	t_value	*value;
	int		status;

	if (output->type == redir_output)
	{
		open_file(output);
		write(output->fd, "", ft_strlen(""));
		close(output->fd);
		output->type = append_;
		output->fd = NOT_OPENED;
	}
	value = evaluate(node->left, input, output);
	while (global.pids.pos > 0)
		status = get_last_exit_code();
	ft_exit(SUCCESS);
}

void	child_process(t_node *node, t_file *input, t_file *output)
{
	int		pos;
	int		i;
	t_token	*token;

	pos = node->token->start;
	i = 0;
	while (++i < node->token->len)
	{
		token = ((t_token **)global.tokens.pointers)[pos];
		pos++;
		if (is_redirection(token->type))
		{
			check_imbegious_redirection(token, pos, input, output);
			i++;
			pos++;
		}
	}
	output_redirection(node, input, output);
}

void	open_parenteses(t_node *node, t_file *input, t_file *output)
{
	int	pid;

	pid = fork();
	add_number(&global.pids, pid);
	if (pid == 0)
		child_process(node, input, output);
	waitpid(pid, NULL, 0);
}
