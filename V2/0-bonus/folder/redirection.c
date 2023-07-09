/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 22:52:33 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/17 02:11:01 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/redirection.h"

bool	is_redirection(t_type type)
{
	return (type == redir_input || type == redir_output || type == heredoc_
		|| type == append_);
}

int	check_redirection_errors(t_token *token, bool is_child_process)
{
	int	pid;
	int	error;

	error = SUCCESS;
	pid = 0;
	if (token->type == star_)
	{
		printf("minishell: * ambiguous redirection\n");
		error = AMBIGIOUS_REDIRECTION;
	}
	else if (token->type != identifier_)
	{
		printf("minishell: Unexpected symbol after redirection\n");
		error = UNEXPECTED_ERROR;
	}
	if (error)
	{
		if (is_child_process == false)
			pid = fork();
		if (pid == 0)
			ft_exit(UNEXPECTED_ERROR);
	}
	return (error);
}

void	check_redirection(t_file *input, t_file *output)
{
	if (input->type == redir_input)
	{
		open_file(input);
		dup2(input->fd, IN);
		close(input->fd);
	}
	if (output->type == redir_output || output->type == append_)
	{
		open_file(output);
		dup2(output->fd, OUT);
		close(output->fd);
	}
}

void	do_redirection(t_type type, char *name, t_file *input, t_file *output)
{
	if (type == redir_input)
		*input = *new_file(name, NOT_OPENED, type);
	if (type == redir_output || type == append_)
		*output = *new_file(name, NOT_OPENED, type);
	if (type == heredoc_)
		*input = *open_heredoc(name);
}

int	handle_redirection(int *pos_ptr, t_token *token, t_file *input,
		t_file *output)
{
	t_type	type;

	type = token->type;
	token = ((t_token **)g_global.tokens.pointers)[(*pos_ptr)];
	if (check_redirection_errors(token, false))
		return (ERROR);
	(*pos_ptr)++;
	do_redirection(type, expand(token->value), input, output);
	return (SUCCESS);
}
