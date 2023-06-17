/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 22:16:20 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/17 00:13:02 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/execute.h"

void	heredoc_child_process(char *delimiter, int *fd)
{
	char	*heredoc_text;
	char	*tmp;

	signal(SIGINT, handle_heredoc_signal);
	signal(SIGQUIT, handle_heredoc_signal);
	heredoc_text = readline("heredoc $> ");
	while (heredoc_text == NULL || ft_strcmp(heredoc_text, delimiter))
	{
		tmp = expand(heredoc_text);
		free(heredoc_text);
		write(fd[1], tmp, ft_strlen(tmp));
		write(fd[1], "\n", ft_strlen("\n"));
		heredoc_text = readline("heredoc $> ");
	}
	close(fd[0]);
	close(fd[1]);
	ft_exit(SUCCESS);
}

t_file	*open_heredoc(char *delimiter)
{
	int		*fd;
	int		pid;
	t_file	*input;

	fd = open_pipe();
	input = new_file(NULL, fd[0], redir_input);
	new_file(NULL, fd[1], redir_output);
	pid = create_child();
	if (pid == 0)
		heredoc_child_process(delimiter, fd);
	waitpid(pid, NULL, 0);
	close(fd[1]);
	return (input);
}
