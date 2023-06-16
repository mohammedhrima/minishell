#include "../header.h"

char *get_command_path(char *cmd)
{
    char *res;
    int i;

    if (cmd == NULL || ft_strchr(cmd, '/') || ft_strlen(cmd) == 0)
        return (cmd);
    i = 0;
    while (global.path && global.path[i])
    {
        res = strjoin(global.path[i], "/", cmd);
        if (access(res, F_OK) == 0 && access(res, X_OK) == 0)
            return (res);
        i++;
    }
    return (cmd);
}

void check_redirection(t_file *input, t_file *output)
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

void execute(t_file *input, t_file *output, char **arguments)
{
    int pid;
    void (*func)(char **);
    int i;

    func = get_built_in_function(arguments[0]);
    if (func)
    {
        if (global.inside_pipe)
        {
            pid = fork();
            add_number(&global.pids, pid);
            if (pid == 0)
            {
                check_redirection(input, output);
                func(&arguments[1]);
                ft_exit(SUCCESS);
            }
        }
        else
        {
            check_redirection(input, output);
            func(&arguments[1]);
            pid = fork();
            add_number(&global.pids, pid);
            if (pid == 0)
                ft_exit(SUCCESS);
        }
    }
    else
    {
        arguments[0] = get_command_path(arguments[0]);
        pid = fork();
        add_number(&global.pids, pid);
        if (pid == 0)
        {
            check_redirection(input, output);
            i = 0;
            while (i < global.fds.pos)
            {
                if (global.fds.integers[i] != IN && global.fds.integers[i] != OUT)
                    close(global.fds.integers[i]);
                i++;
            }
            if (execve(arguments[0], &arguments[0], global.env))
            {
                if (ft_strchr(arguments[0], '/'))
                    printf("minishell: '%s' no such file or directory\n", arguments[0]);
                else
                    printf("minishell: '%s' command not found\n", arguments[0]);
                ft_exit(COMMAND_NOT_FOUND);
            }
        }
    }
}