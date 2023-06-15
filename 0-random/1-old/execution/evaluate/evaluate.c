#include "evaluate.h"

void open_pipe(t_node *node, t_file *input, t_file *output)
{
    int fd[2];

    if (pipe(fd) < 0)
    {
        printf("minishell: Error openning pipe\n");
        exit(ERROR);
    }
    add_number(&global.fds, fd[0]);
    add_number(&global.fds, fd[1]);
    evaluate(node->left, input, new_file(NULL, fd[1], redir_output));
    evaluate(node->right, new_file(NULL, fd[0], redir_input), output);
    close(fd[0]);
    close(fd[1]);
}

char **build_arguments(char **arguments, t_token *token, int *len_ptr, int *pos_ptr)
{
    char *argument;
    int len;
    int pos;

    len = *len_ptr;
    pos = *pos_ptr;
    if (token->type == star_)
    {
        pos++;
        token = ((t_token **)global.tokens.pointers)[pos];
    }
    arguments = ft_realloc(arguments, len * sizeof(char *), (len + 2) * sizeof(char *));
    argument = expand(token->value);
    arguments[len++] = argument;
    arguments[len] = NULL;
    return arguments;
}

char **is_identifier_or_star(t_token *token, char **arguments, int *len_ptr, int *pos_ptr)
{
    char *argument;
    int len;
    int pos;

    len = *len_ptr;
    pos = *pos_ptr;
    if (token->type == star_)
    {
        pos++;
        token = ((t_token **)global.tokens.pointers)[pos];
    }
    arguments = ft_realloc(arguments, len * sizeof(char *), (len + 2) * sizeof(char *));
    argument = expand(token->value);
    arguments[len++] = argument;
    arguments[len] = NULL;
    *pos_ptr = pos;
    *len_ptr = len;
    return arguments;
}

static int check_imbegious_redirection(t_token *token, int pos, t_file *input, t_file *output)
{
    int pid;
    t_type type;
    char *filename;

    type = token->type;
    token = ((t_token **)global.tokens.pointers)[pos];
    if (token->type == star_)
    {
        printf("minishell: * ambiguous redirection\n");
        pid = fork();
        add_number(&global.pids, pid);
        if (pid == 0)
            ft_exit(AMBIGIOUS_REDIRECTION);
        return ERROR;
    }
    else if (token->type != identifier_)
    {
        printf("minishell: Unexpected symbol after redirection\n");
        pid = fork();
        add_number(&global.pids, pid);
        if (pid == 0)
            ft_exit(AMBIGIOUS_REDIRECTION);
        return ERROR;
    }
    filename = expand(token->value);
    if (type == redir_input)
        input = new_file(filename, NOT_OPENED, type);
    if (type == redir_output || type == append_)
        output = new_file(filename, NOT_OPENED, type);
    if (type == heredoc_)
        input = open_heredoc(filename, input);
    return SUCCESS;
}

void is_redirection_or_identifier(t_node *node, t_file *input, t_file *output)
{
    char **arguments;
    int i;
    int len;
    int pos;
    int pid;
    int there_is_redirection = 0;
    void (*func)(char **);
    t_token *token;

    arguments = NULL;
    i = 0;
    len = 0;
    pos = node->token->start;
    while (i < node->token->len)
    {
        token = ((t_token **)global.tokens.pointers)[pos];
        pos++;
        if (token->type == identifier_ || token->type == star_)
            arguments = is_identifier_or_star(token, arguments, &len, &pos);
        else if (is_redirection(token->type))
        {
            there_is_redirection = 1;
            if (check_imbegious_redirection(token, pos, input, output))
                return;
            pos++;
            i++;
        }
        i++;
    }
    if (arguments && arguments[0])
    {
        func = get_built_in_func(arguments[0]);
        if (func)
            call_built_in_function(func, arguments, input, output);
        else
            execute_command(input, output, arguments);
    }
    else if (there_is_redirection)
    {
        pid = fork();
        add_number(&global.pids, pid);
        if (pid == 0)
        {
            do_redirection(input, output);
            ft_exit(SUCCESS);
        }
    }
}
#if 0
t_value *evaluate(t_node *node, t_file *input, t_file *output)
{
    t_type type;
    t_value *value;
    int status;

    type = node->token->type;
    if (is_redirection(type) || type == identifier_)
        is_redirection_or_identifier(node, input, output);
    else if (type == pipe_)
        open_pipe(node, input, output);
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
        open_parenteses(node, input, output);
    return (NULL);
}
#else
t_value *evaluate(t_node *node, t_file *input, t_file *output)
{
    ft_printf(OUT, "Evaluate %k\n", node->token);
    t_type *types;
    t_type type;
    t_token *token;
    t_value *value;
    int i;
    int pos;
    int len;
    char **arguments;
    char *argument;
    int include_hidden;
    int there_is_redirection = 0;
    char *filename;
    char *delimiter;
    static int heredoc_index;
    char *heredoc_text;
    int pid;
    int fd[2];
    int *fds;
    int status;

    type = node->token->type;
    argument = NULL;
    arguments = NULL;
    if (is_redirection(type) || type == identifier_)
    {
        i = 0;
        len = 0;
        include_hidden = 0;
        pos = node->token->start;
        while (i < node->token->len)
        {
            token = ((t_token **)global.tokens.pointers)[pos];
            pos++;
            if (token->type == identifier_ || token->type == star_)
            {
                if (token->type == star_)
                {
                    pos++;
                    token = ((t_token **)global.tokens.pointers)[pos];
                }
                arguments = ft_realloc(arguments, len * sizeof(char *), (len + 2) * sizeof(char *));
                argument = expand(token->value);
                if (argument && argument[0] == '.')
                {
                    if (ft_strcmp(argument, ".") == 0 || ft_strcmp(argument, "..") == 0)
                        arguments[len++] = argument;
                    else if (include_hidden)
                        arguments[len++] = argument;
                }
                else
                    arguments[len++] = argument;
                arguments[len] = NULL;
            }
            else if (is_redirection(token->type))
            {
                there_is_redirection = 1;
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
                pos++;
                i++;
                filename = expand(token->value);
                if (type == redir_input)
                    input = new_file(filename, NOT_OPENED, type);
                if (type == redir_output || type == append_)
                    output = new_file(filename, NOT_OPENED, type);
                if (type == heredoc_)
                {
                    delimiter = filename;
                    pipe(fd); // check if it fails

                    input = new_file(NULL, fd[0], redir_input);
                    new_file(NULL, fd[1], redir_output);

                    pid = fork();
                    add_number(&global.pids, pid);

                    if (pid == 0)
                    {
                        signal(SIGINT, handle_heredoc_signal);
                        signal(SIGQUIT, handle_heredoc_signal);
                        heredoc_text = readline("heredoc $> ");
                        while (heredoc_text == NULL || ft_strcmp(heredoc_text, delimiter))
                        {
                            char *tmp = expand(heredoc_text);
                            free(heredoc_text);
                            write(fd[1], tmp, ft_strlen(tmp));
                            write(fd[1], "\n", ft_strlen("\n"));
                            heredoc_text = readline("heredoc $> ");
                        }
                        i = 0;
                        while (i < global.fds.pos)
                        {
                            if (global.fds.integers[i] != IN && global.fds.integers[i] != OUT)
                                close(global.fds.integers[i]);
                            i++;
                        }
                        close(fd[0]);
                        close(fd[1]);
                        ft_exit(SUCCESS);
                    }
                    waitpid(pid, NULL, 0);
                    close(fd[1]);
                }
            }
            i++;
        }
        if (arguments && arguments[0])
        {
            void (*func)(char **) = get_built_in_func(arguments[0]);
            if (func)
            {
                if (global.inside_pipe)
                {
                    pid = fork();
                    add_number(&global.pids, pid);
                    if (pid == 0)
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
                        func(&arguments[1]);
                        ft_exit(SUCCESS);
                    }
                }
                else
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
    }
    if (is_redirection(type) || type == identifier_)
        is_redirection_or_identifier(node, input, output);
    else if (type == pipe_)
        open_pipe(node, input, output);
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
        open_parenteses(node, input, output);
    return (NULL);
}
#endif