#include "_headers/execution.h"

int arrlen(void **pointer)
{
    int i = 0;
    while (pointer && pointer[i])
        i++;
    return i;
}

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
    // return (arguments);
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
            len++;
            arguments[len] = NULL;
        }
        else if (tokens[pos]->type == star_)
            expand_star(arguments, &len);
        // else if (is_redirection(tokens[pos]->type) || tokens[pos]->type == heredoc_)
        // {
        //     // handle redirection
        // }
        pos++;
    }
    return (arguments);
}

void execute_command(char **arguments, t_file *input, t_file *output)
{
#if 0
    char *argument;
    argument = get_command_path(arguments[0]);
    if (create_child() == 0)
    {
        check_redirection(input, output);
        close_fds();
        if (execve(argument, arguments, g_global.env) < 0)
        {
            if (ft_strchr(argument, '/'))
                printf("minishell: '%s' no such file or directory\n", argument);
            else
                printf("minishell: '%s' command not found\n", argument);
            ft_exit(COMMAND_NOT_FOUND);
        }
    }
#endif
}

void command(t_token *token, t_file *input, t_file *output)
{
    char **arguments;
    printf("node has start: %d, and len: %d\n", token->start, token->len);
    arguments = build_arguments(token, input, output);
    int i = 0;
    while (arguments && arguments[i])
    {
        printf("-> %s\n", arguments[i++]);
    }
    if (arguments && arguments[0])
    {
        // check if argements[0] is in "> < << >>" else it's built in or command
        // check if it's built in else it's a command
        // give built in output and output
        execute_command(arguments, input, output);
    }
}

t_value *evaluate(t_node *node, t_file *input, t_file *output)
{
    t_type type;
    t_value *value;
    // int     status;

    printf("Evaluate '%s' has type: %s\n", node->token->value, type_to_string(node->token->type));
    type = node->token->type;
    if (is_redirection(type) || type == identifier_)
    {
        command(node->token, input, output);
    }
    // exit(0);
    return NULL;
}