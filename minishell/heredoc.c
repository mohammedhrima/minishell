#include "_headers/execution.h"

void	heredoc_child_process(char *delimiter, int fd)
{
	char	*heredoc_text;
	char	*tmp;

	signal(SIGINT, handle_heredoc_signal);
	// signal(SIGQUIT, handle_heredoc_signal);
	heredoc_text = readline("heredoc $> ");
	while (heredoc_text == NULL || ft_strcmp(heredoc_text, delimiter))
	{
		tmp = expand(heredoc_text);
		free(heredoc_text);
		write(fd, tmp, ft_strlen(tmp));
		write(fd, "\n", ft_strlen("\n"));
		heredoc_text = readline("heredoc $> ");
	}
	close(fd);
	ft_exit(SUCCESS);
}

char *open_heredoc(char *delimiter)
{
	int		pid;
	t_file	*input;
    static int i;
    char *name;

    name = strjoin(".heredoc", ft_itoa(i++), NULL);
	input = new_file(name, open(name, O_RDWR | O_TRUNC | O_CREAT), redir_output);
	pid = create_child();
	if (pid == 0)
		heredoc_child_process(delimiter, input->fd);
	waitpid(pid, NULL, 0);
	close(input->fd);
	return (name);
}
