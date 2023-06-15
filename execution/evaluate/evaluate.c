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

void is_redirection_or_identifier(t_node *node, t_file *input, t_file *output)
{
    char **arguments;
    char *argument;
    int i;
    int len;
    int pos;
    int pid;
    int there_is_redirection = 0;
    void (*func)(char **);
    t_type type;
    t_token *token;
    char *filename;

    i = 0;
    len = 0;
    pos = node->token->start;
    arguments = NULL;
    argument = NULL;
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
                pid = fork();
                add_number(&global.pids, pid);
                if (pid == 0)
                    ft_exit(AMBIGIOUS_REDIRECTION);
                return;
            }
            else if (token->type != identifier_)
            {
                printf("minishell: Unexpected symbol after redirection\n");
                pid = fork();
                add_number(&global.pids, pid);
                if (pid == 0)
                    ft_exit(AMBIGIOUS_REDIRECTION);
                return;
            }
            pos++;
            i++;
            filename = expand(token->value);
            if (type == redir_input)
                input = new_file(filename, NOT_OPENED, type);
            if (type == redir_output || type == append_)
                output = new_file(filename, NOT_OPENED, type);
            if (type == heredoc_)
                input = open_heredoc(filename, input);
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