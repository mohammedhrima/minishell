#include "evaluate.h"

void build_and_evaluate_arguments(t_node *node, t_file *input, t_file *output)
{
    char **arguments;
    char *argument;
    int i;
    int len;
    int pos;
    int pid;
    t_token *token;
    t_type type;
    int include_hidden;
    int there_is_redirection;

    arguments = NULL;
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
                pid = fork();
                if (pid == 0)
                    ft_exit(AMBIGIOUS_REDIRECTION);
                return;
            }
            else if (token->type != identifier_)
            {
                printf("minishell: Unexpected symbol after redirection\n");
                pid = fork();
                if (pid == 0)
                    ft_exit(UNEXPECTED_ERROR);
                return;
            }
            pos++;
            i++;
            if (type == redir_input)
                input = new_file(expand(token->value), NOT_OPENED, type);
            if (type == redir_output || type == append_)
                output = new_file(expand(token->value), NOT_OPENED, type);
            if (type == heredoc_)
                open_heredoc(input, expand(token->value));
        }
        i++;
    }
    if (arguments && arguments[0])
        execute(input, output, arguments);
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

void evaluate_pipe(t_node *node, t_file *input, t_file *output)
{
    int fd[2];

    if (pipe(fd))
        ft_exit(ERROR);
    add_number(&global.fds, fd[0]);
    add_number(&global.fds, fd[1]);
    evaluate(node->left, input, new_file(NULL, fd[1], redir_output));
    evaluate(node->right, new_file(NULL, fd[0], redir_input), output);
    close(fd[0]);
    close(fd[1]);
}

t_value *evaluate_logic_operator(t_node *node, t_file *input, t_file *output)
{
    t_value *value;
    int status;

    value = evaluate(node->left, input, output);
    status = get_last_exit_code();
    if (status == 0 && node->token->type == and_)
        value = evaluate(node->right, input, output);
    if (status != 0 && node->token->type == or_)
        value = evaluate(node->right, input, output);
    return (value);
}

void evaluate_parenteses(t_node *node, t_file *input, t_file *output)
{
    int pid;
    int i;
    int pos;
    t_type type;
    int status;
    t_value *value;
    t_token *token;

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
                if (type == redir_input)
                    input = new_file(expand(token->value), NOT_OPENED, type);
                if (type == redir_output || type == append_)
                    output = new_file(expand(token->value), NOT_OPENED, type);
                if (type == heredoc_)
                    open_heredoc(input, expand(token->value));
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

t_value *evaluate(t_node *node, t_file *input, t_file *output)
{
    t_type type;
    t_token *token;
    t_value *value;
    int i;
    int pos;
    int pid;
    int fd[2];
    int status;

    type = node->token->type;
    if (is_redirection(type) || type == identifier_)
        build_and_evaluate_arguments(node, input, output);
    else if (type == pipe_)
        evaluate_pipe(node, input, output);
    else if (type == or_ || type == and_)
        return (evaluate_logic_operator(node, input, output));
    else if (type == lparent_)
        evaluate_parenteses(node, input, output);
    return NULL;
}
