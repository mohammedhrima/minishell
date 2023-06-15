#include "header.h"

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
                if (ft_strchr(argument, '-') && ft_strchr(argument, 'a'))
                    include_hidden = 1;
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
                    ft_printf(OUT, "open pipe has input: %d, ouput: %d\n", fd[0], fd[1]);

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
                }
            }
            i++;
        }
        if (arguments && arguments[0])
        {
            void (*func)(char **) = get_built_in_function(arguments[0]);
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
        else if (there_is_redirection)
        {
            pid = fork();
            add_number(&global.pids, pid);
            if (pid == 0)
            {
                check_redirection(input, output);
                ft_exit(SUCCESS);
            }
        }
    }
    else if (type == pipe_)
    {
        if (pipe(fd) < 0)
        {
            printf("minishell: Error openning pipe\n");
            ft_exit(ERROR);
        }
        add_number(&global.fds, fd[0]);
        add_number(&global.fds, fd[1]);
        evaluate(node->left, input, new_file(NULL, fd[1], redir_output));
        evaluate(node->right, new_file(NULL, fd[0], redir_input), output);
        close(fd[0]);
        close(fd[1]);
    }
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
    {
        ft_printf(OUT, "execute lparent\n");
        pid = fork();
        add_number(&global.pids, pid);
        if (pid == 0)
        {
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
                    }
                }
                i++;
            }
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
        waitpid(pid, NULL, 0);
    }
    return NULL;
}

int main(int argc, char **argv, char **envp)
{
    char *text;
    t_file *input;
    t_file *output;
    int i;
    t_token **tokens;
    t_node *curr;

    global.values = (char *[]){"<<", ">>", "<", ">", "&&", "||", "(", ")", "|",
                               "*", 0};
    global.types = (t_type[]){heredoc_, append_, redir_input, redir_output,
                              and_, or_, lparent_, rparent_, pipe_, star_, 0};
    global.env = envp;
    new_list(&global.envirement, sizeof(t_node *), pointers_);
    new_list(&global.tokens, sizeof(t_token *), pointers_);
    new_list(&global.pids, sizeof(int), integers_);
    new_list(&global.fds, sizeof(int), integers_);
    i = 0;
    while (envp && envp[i])
    {
        if (ft_strncmp("PATH", envp[i], ft_strlen("PATH")) == 0)
        {
            global.path = split(envp[i], ":");
            break;
        }
        i++;
    }

    build_envirement(envp);
    clear_list(&global.tokens);

    while (argc && argv)
    {
        signal(SIGINT, handle_signal);
        signal(SIGQUIT, handle_signal);

        input = new_file(NULL, IN, 0);
        output = new_file(NULL, OUT, 0);

        text = readline("minishell $> ");
        if (text && is_inclosed(text))
        {
            printf("minishell: syntax error\n");
            add_history(text);
            free(text);
            clear_list(&global.tokens);
            clear_list(&global.pids);
            clear_list(&global.fds);
            continue;
        }
        if (text == NULL)
            break;
        else
            add_history(text);
        if (build_tokens(text) != SUCCESS)
        {
            while (global.pids.pos > 0)
                get_last_exit_code();
            free(text);
            clear_list(&global.tokens);
            clear_list(&global.pids);
            clear_list(&global.fds);
            continue;
        }
        global.tokens.pos = 0;
        tokens = (t_token **)global.tokens.pointers;
        while (tokens[global.tokens.pos]->type != end_)
        {
            curr = expr();
            if (curr == NULL)
                break;
            evaluate(curr, input, output);
        }

        // wait for all child processes
        while (global.pids.pos > 0)
            get_last_exit_code();
        ft_printf(OUT, "finish evaluating\n");
        free(text);
        clear_list(&global.tokens);
        clear_list(&global.pids);
        clear_list(&global.fds);
    }

    ft_exit(0);
}