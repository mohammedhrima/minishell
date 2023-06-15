/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/15 01:50:29 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/15 13:02:36 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "evaluate.h"

void	fork_heredoc(char *delimiter, int input, int output)
{
	char	*heredoc_text;
	char	*tmp;

	heredoc_text = readline("heredoc $> ");
	while (heredoc_text == NULL || ft_strcmp(heredoc_text, delimiter))
	{
		tmp = expand(heredoc_text);
		free(heredoc_text);
		write(output, tmp, ft_strlen(tmp));
		write(output, "\n", ft_strlen("\n"));
		heredoc_text = readline("heredoc $> ");
	}
	close(input);
	close(output);
	ft_exit(SUCCESS);
}

t_file	*open_heredoc(char *delimiter, t_file *input)
{
	int	fd[2];
	int	pid;

	if (pipe(fd) < 0)
	{
		printf("minishell: Error openning pipe\n");
		ft_exit(ERROR);
	}
	input = new_file(NULL, fd[0], redir_input);
	new_file(NULL, fd[1], redir_output);
	pid = fork();
	add_number(&global.pids, pid);
	if (pid == 0)
	{
		signal(SIGINT, handle_heredoc_signal);
		signal(SIGQUIT, handle_heredoc_signal);
		fork_heredoc(delimiter, fd[0], fd[1]);
	}
	waitpid(pid, NULL, 0);
	close(fd[1]);
	return (input);
}
