#include "../header.h"

t_file *open_heredoc(t_file *input, char *delimiter)
{
    int fd[2];
    int pid;
    char *heredoc_text;
    char *tmp;
    int i;

    if (pipe(fd))
        ft_exit(ERROR);
    *input = *new_file(NULL, fd[0], redir_input);
    new_file(NULL, fd[1], redir_output);
    pid = fork();
    if (pid < 0)
        ft_exit(ERROR);
    add_number(&global.pids, pid);
    if (pid == 0)
    {
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
        i = 0;
        while (i < global.fds.pos)
        {
            if (global.fds.integers[i] != IN && global.fds.integers[i] != OUT)
            {
                ft_printf(OUT, "close %d\n", global.fds.integers[i]);
                close(global.fds.integers[i]);
            }
            i++;
        }
        close(fd[0]);
        close(fd[1]);
        ft_exit(SUCCESS);
    }
    waitpid(pid, NULL, 0);
    close(fd[1]);
    return input;
}