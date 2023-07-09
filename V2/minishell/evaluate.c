#include "_headers/execution.h"

void expand_star(char **arguments, int *len_ptr)
{
    char *new;
    DIR *curr_dir;
    struct dirent *content;
    int len;

    len = *len_ptr;
    new = getcwd(NULL, 0);
    curr_dir = opendir(new);
    free(new);
    content = readdir(curr_dir);
    while (content)
    {
        if (content->d_name[0] != '.')
        {
            arguments = ft_realloc(arguments, len, len + 2, sizeof(char *));
            arguments[len] = ft_strdup(content->d_name);
            printf("--> %s\n", arguments[len]);
            len++;
        }
        content = readdir(curr_dir);
    }
    closedir(curr_dir);
    *len_ptr = len;
}

char **build_arguments(t_token *token, t_file *input, t_file *output)
{
    t_token **tokens;
    char **arguments;
    int len;
    int pos;

    arguments = NULL;
    len = 0;
    pos = token->start;
    tokens = (t_token **)g_global.tokens.pointers;
    while (pos < token->len)
    {
        if (tokens[pos]->type == identifier_)
        {
            arguments = ft_realloc(arguments, len, len + 2, sizeof(char *));
            arguments[len] = expand(tokens[pos]->value);
            arguments[++len] = NULL;
        }
        else if (tokens[pos]->type == star_)
            expand_star(arguments, &len);
        else if (is_redirection(tokens[pos]->type) || tokens[pos]->type == heredoc_)
        {
            printf("found redirection\n");
            pos++;
            // handle redirection
            if (tokens[pos - 1]->type == redir_input)
            {
                *input = *new_file(expand(tokens[pos]->value), NOT_OPENED, redir_input);
                printf("open '%s' '%s'\n", type_to_string(tokens[pos - 1]->type), expand(tokens[pos]->value));
            }
            if (tokens[pos - 1]->type == heredoc_)
            {
                *input = *new_file(expand(tokens[pos - 1]->value), NOT_OPENED, redir_input);
                printf("open '%s' '%s'\n", type_to_string(tokens[pos - 1]->type), expand(tokens[pos - 1]->value));
            }
            if (tokens[pos - 1]->type == redir_output || tokens[pos - 1]->type == append_)
            {
                *output = *new_file(expand(tokens[pos]->value), NOT_OPENED, tokens[pos - 1]->type);
                printf("open '%s' '%s'\n", type_to_string(tokens[pos - 1]->type), expand(tokens[pos]->value));
            }
            // exit(0);
        }
        pos++;
    }

    return (arguments);
}

char **get_path()
{
    int i = 0;

    t_node **envirement;
    t_token *left;
    t_token *right;
    char **path;

    envirement = (t_node **)g_global.envirement.pointers;
    while (envirement && envirement[i])
    {
        left = envirement[i]->left->token;
        right = envirement[i]->right->token;
        // check if left or right ar nulls
        // printf("'%s'='%s' \n", left->value, right->value);
        if (left->value && ft_strcmp(left->value, "PATH") == 0)
        {
            // use lstate
            // printf("found\n");
            path = split(right->value, ":"); // check IFS
            return path;
        }
        i++;
    }
    // exit(0);
    return NULL;
}

char *get_command_path(char *cmd)
{
    char *res;
    int i;
    char **path;

    if (cmd == NULL || ft_strchr(cmd, '/') || ft_strlen(cmd) == 0)
        return (cmd);
    path = get_path();
    i = 0;
    while (path && path[i])
    {
        res = strjoin(path[i], "/", cmd);
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

void execute_command(char **arguments, t_file *input, t_file *output)
{
    char *argument;
    argument = get_command_path(arguments[0]);
    if (create_child() == 0)
    {
        check_redirection(input, output);
        if (execve(argument, arguments, g_global.env) < 0)
        {
            if (ft_strchr(argument, '/'))
                printf("minishell: '%s' no such file or directory\n", argument);
            else
                printf("minishell: '%s' command not found\n", argument);
            ft_exit(COMMAND_NOT_FOUND);
        }
    }
}

void call_func(void (*func)(char **), char **args, t_file *in, t_file *out)
{
    printf("is built in\n");
    if (g_global.inside_pipe)
    {
        if (create_child() == 0)
        {
            check_redirection(in, out);
            func(&args[1]);
            ft_exit(SUCCESS);
        }
    }
    else
    {
        check_redirection(in, out);
        func(&args[1]);
        if (create_child() == 0)
            ft_exit(SUCCESS);
    }
}

void command(t_token *token, t_file *input, t_file *output)
{
    void (*func)(char **) = NULL;
    char **arguments;
    printf("node has start: %d, and len: %d\n", token->start, token->len);
    arguments = build_arguments(token, input, output);
    if (arguments && arguments[0])
    {

        // check if argements[0] is in "> < << >>" else it's built in or command
        // check if it's built in else it's a command
        // give built in output and output
        func = get_built_in_function(arguments[0]);
        if (func)
        {
            printf("built in function '%s' has the following arguments:\n", arguments[0]);
            int i = 0;
            while(arguments[i])
            {
                printf("        '%s'\n", arguments[i]);
                i++;
            }
            call_func(func, arguments, input, output);
        }
        else
            execute_command(arguments, input, output);
    }
    else if (create_child() == 0)
    {
        check_redirection(input, output);
        ft_exit(SUCCESS);
    }
}

t_value *evaluate(t_node *node, t_file *input, t_file *output)
{
    t_type type;
    t_value *value;
    // int     status;

    printf("Evaluate '%s' has type: %s\n", node->token->value, type_to_string(node->token->type));
    type = node->token->type;
    if (is_redirection(type) || type == identifier_ || type == heredoc_)
    {
        command(node->token, input, output);
    }
    // exit(0);
    return NULL;
}