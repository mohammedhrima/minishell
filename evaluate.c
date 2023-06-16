#include "./headers/execute.h"

t_value *evaluate(t_node *node, t_file *input, t_file *output)
{
    t_type type;
    t_token *token;
    t_value *value;
    int i;
    int pos;
    int len;
    char **arguments;
    char *argument;
    int there_is_redirection = 0;
    int pid;
    int *fd;
    int status;
    char *new;
    DIR *curr_dir;
    struct dirent *content;

    type = node->token->type;
    argument = NULL;
    arguments = NULL;
    if (is_redirection(type) || type == identifier_)
    {
        i = 0;
        len = 0;
        pos = node->token->start;
        while (i < node->token->len)
        {
            token = ((t_token **)global.tokens.pointers)[pos];
            pos++;
            if (token->type == identifier_ || token->type == star_)
            {
                arguments = ft_realloc(arguments, len * sizeof(char *), (len + 2) * sizeof(char *));
                if (token->type == star_)
                {
                    pos++;
                    token = ((t_token **)global.tokens.pointers)[pos];
                    new = getcwd(NULL, 0);
                    curr_dir = opendir(new);
                    free(new);
                    content = readdir(curr_dir);
                    while (content)
                    {
                        if (content->d_name[0] != '.')
                        {
                            arguments = ft_realloc(arguments, len * sizeof(char *), (len + 2) * sizeof(char *));
                            arguments[len++] = ft_strdup(content->d_name);
                            arguments[len] = NULL;
                        }
                        content = readdir(curr_dir);
                    }
                    closedir(curr_dir);
                }
                else
                {
                    argument = expand(token->value);
                    arguments[len++] = argument;
                }
                arguments[len] = NULL;
            }
            else if (is_redirection(token->type))
            {
                there_is_redirection = 1;
                type = token->type;
                token = ((t_token **)global.tokens.pointers)[pos];
                if (check_redirection_errors(token, false))
                    return (NULL);
                pos++;
                i++;
                do_redirection(type, expand(token->value), input, output);
            }
            i++;
        }
        if (arguments && arguments[0])
            execute(arguments, input, output);
        else if (there_is_redirection)
        {
            if (create_child() == 0)
            {
                check_redirection(input, output);
                ft_exit(SUCCESS);
            }
        }
    }
    else if (type == pipe_)
    {
        fd = open_pipe();
        add_number(&global.fds, fd[0]);
        add_number(&global.fds, fd[1]);
        evaluate(node->left, input, new_file(NULL, fd[1], redir_output));
        evaluate(node->right, new_file(NULL, fd[0], redir_input), output);
        close(fd[0]);
        close(fd[1]);
        // global.fds.pos -= 2;
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
        pid = create_child();
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
                    if (check_redirection_errors(token, true))
                        return (NULL);
                    pos++;
                    i++;
                    do_redirection(type, expand(token->value), input, output);
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
            ft_printf(OUT, "Exit lparent 1\n");
            while (global.pids.pos > 0)
                status = get_last_exit_code();

            ft_exit(SUCCESS);
        }
        waitpid(pid, NULL, 0);
        ft_printf(OUT, "Exit lparent 2\n");
    }
    return NULL;
}
