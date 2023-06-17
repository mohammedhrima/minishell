/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   evaluate.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 00:07:23 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/17 00:08:24 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/execute.h"

int	redirect_parenteses(t_node *node, t_file *input, t_file *output)
{
	int		pos;
	int		i;
	t_token	*token;
	t_type	type;

	pos = node->token->start;
	i = 0;
	while (i < node->token->len)
	{
		token = ((t_token **)global.tokens.pointers)[pos];
		pos++;
		if (is_redirection(token->type))
		{
			type = token->type;
			token = ((t_token **)global.tokens.pointers)[pos];
			if (check_redirection_errors(token, true))
				return (ERROR);
			pos++;
			i++;
			do_redirection(type, expand(token->value), input, output);
		}
		i++;
	}
	return (SUCCESS);
}

void	evaluate_parenteses(t_node *node, t_file *input, t_file *output)
{
	int	pid;

	pid = create_child();
	if (pid == 0)
	{
		if (redirect_parenteses(node, input, output))
			return ;
		if (output->type == redir_output)
		{
			open_file(output);
			write(output->fd, "", ft_strlen(""));
			close(output->fd);
			output->type = append_;
			output->fd = NOT_OPENED;
		}
		evaluate(node->left, input, output);
		while (global.pids.pos > 0)
			get_last_exit_code();
		ft_exit(SUCCESS);
	}
	waitpid(pid, NULL, 0);
}

void	evaluate_pipe(t_node *node, t_file *input, t_file *output)
{
	int	*fd;

	fd = open_pipe();
	add_number(&global.fds, fd[0]);
	add_number(&global.fds, fd[1]);
	evaluate(node->left, input, new_file(NULL, fd[1], redir_output));
	evaluate(node->right, new_file(NULL, fd[0], redir_input), output);
	close(fd[0]);
	close(fd[1]);
}

t_value	*evaluate(t_node *node, t_file *input, t_file *output)
{
	t_type	type;
	t_value	*value;
	int		status;

	type = node->token->type;
	if (is_redirection(type) || type == identifier_)
		evaluate_command(node, input, output);
	else if (type == pipe_)
		evaluate_pipe(node, input, output);
	else if (type == or_ || type == and_)
	{
		value = evaluate(node->left, input, output);
		status = get_last_exit_code();
		if (status == 0 && node->token->type == and_)
			value = evaluate(node->right, input, output);
		if (status != 0 && node->token->type == or_)
			value = evaluate(node->right, input, output);
		return (value);
	}
	else if (type == lparent_)
		evaluate_parenteses(node, input, output);
	return (NULL);
}
