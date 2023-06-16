#include "headers/minishell.h"

void build_envirement(char **envp)
{
    int i;
    char **array;
    t_node *node;

    i = 0;
    while (envp && envp[i])
    {
        array = split_by_two(envp[i], '=');
        node = new_node(new_token(assign_, NULL, 0));
        node->left = new_node(new_token(identifier_, NULL, 0));
        node->left->token->value = array[0];
        node->right = new_node(new_token(identifier_, NULL, 0));
        node->right->token->value = array[1];
        add_pointer(&global.envirement, node);
        i++;
    }
}

int build_tokens(char *text)
{
    int txt_pos;
    int start;
    int single_quote;
    int double_quote;

    int i;
    t_type type;
    txt_pos = 0;
    start = 0;

    while (text && text[txt_pos])
    {
        single_quote = 0;
        double_quote = 0;
        if (text[txt_pos] == '\0')
            break;
        if (ft_isspace(text[txt_pos]))
        {
            start = txt_pos;
            while (ft_isspace(text[txt_pos]))
                txt_pos++;
            continue;
        }
        i = 0;
        type = 0;
        while (global.values[i])
        {
            if (ft_strncmp(global.values[i], &text[txt_pos], ft_strlen(global.values[i])) == 0)
            {
                start = txt_pos;
                type = global.types[i];
                txt_pos += ft_strlen(global.values[i]);
                break;
            }
            i++;
        }
        if (type)
        {
            add_pointer(&global.tokens, new_token(type, text + start, txt_pos - start));
            continue;
        }
        start = txt_pos;
        while (text[txt_pos])
        {
            if (text[txt_pos] == '\'' && !double_quote)
                single_quote = !single_quote;
            else if (text[txt_pos] == '"' && !single_quote)
                double_quote = !double_quote;
            else if (!single_quote && !double_quote && (ft_isspace(text[txt_pos]) || ft_strchr("|&<>()*", text[txt_pos])))
                break;
            txt_pos++;
        }
        add_pointer(&global.tokens, new_token(identifier_, text + start, txt_pos - start));
    }
    add_pointer(&global.tokens, new_token(end_, NULL, 0));
    return SUCCESS;
}

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

char *get_var(char *name)
{
    int i;
    t_node **nodes;

    nodes = (t_node **)global.envirement.pointers, i = 0;
    if (ft_strcmp(name, "?") == 0)
        return (ft_itoa(get_last_exit_code()));
    while (nodes && nodes[i])
    {
        if (nodes[i]->left->token->value && ft_strcmp(nodes[i]->left->token->value, name) == 0)
            return (nodes[i]->right->token->value);
        i++;
    }
    return ("");
}

char *expand(char *value)
{
    int j;
    char *var;
    char *to_add;
    char *res;
    int i;
    int single_quotes;
    int double_quotes;

    res = "", i = 0, single_quotes = 0, double_quotes = 0;
    while (value && value[i])
    {
        if (value[i] == '\'')
        {
            if (!double_quotes)
                single_quotes = !single_quotes;
            else
            {
                res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
                res[ft_strlen(res)] = value[i];
            }
            i++;
        }
        else if (value[i] == '"')
        {
            if (!single_quotes)
                double_quotes = !double_quotes;
            else
            {
                res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
                res[ft_strlen(res)] = value[i];
            }
            i++;
        }
        else if (value[i] == '$')
        {
            if (double_quotes || !single_quotes)
            {
                i++;
                j = i;
                while (value[j] && !ft_strchr(" \"", value[j]))
                    j++;
                var = ft_calloc(j - i + 1, sizeof(char)), ft_strncpy(var, value + i, j - i);
                to_add = get_var(var);
                res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + ft_strlen(to_add) + 2);
                ft_strncpy(res + ft_strlen(res), to_add, ft_strlen(to_add));
                i = j;
            }
            else if (single_quotes)
            {
                res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
                res[ft_strlen(res)] = value[i];
                i++;
            }
        }
        else
        {
            res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
            res[ft_strlen(res)] = value[i];
            i++;
        }
    }
    return (res);
}

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
    char *filename;
    char *delimiter;
    char *heredoc_text;
    int pid;
    int fd[2];
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
                            arguments[len++] = content->d_name;
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
                    ft_printf(OUT, "built in function: %s\n", arguments[0]);
                    int i = 1;
                    if (arguments[i])
                        ft_printf(OUT, "       has arguments:\n");
                    while (arguments[i])
                    {
                        ft_printf(OUT, "     	%s\n", arguments[i]);
                        i++;
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
                ft_printf(OUT, "execute command %s, has input: %d, and output: %d\n", arguments[0], input->fd, output->fd);
                pid = fork();
                add_number(&global.pids, pid);
                if (pid == 0)
                {
                    if (input->type == redir_input)
                    {
                        open_file(input);
                        ft_printf(OUT, "redirect input: %d, output: %d\n", input->fd, output->fd);
                        dup2(input->fd, IN);
                        close(input->fd);
                    }
                    if (output->type == redir_output || output->type == append_)
                    {
                        open_file(output);
                        ft_printf(OUT, "redirect output: %d, input: %d\n", output->fd, input->fd);
                        dup2(output->fd, OUT);
                        close(output->fd);
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

                    if (execve(arguments[0], arguments, global.env) < 0)
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
                if (input->type == redir_input)
                {
                    open_file(input);
                    ft_printf(OUT, "redirect input: %d, output: %d\n", input->fd, output->fd);
                    dup2(input->fd, IN);
                    close(input->fd);
                }
                if (output->type == redir_output || output->type == append_)
                {
                    open_file(output);
                    ft_printf(OUT, "redirect output: %d, input: %d\n", output->fd, input->fd);
                    dup2(output->fd, OUT);
                    close(output->fd);
                }
                ft_exit(SUCCESS);
            }
        }
    }
    else if (type == pipe_)
    {
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

int is_inclosed(char *text)
{
    int quote;
    int is_operator;
    int is_parent;
    int i;

    quote = 0;
    is_operator = 0;
    is_parent = 0;
    i = 0;
    while (text && text[i])
    {
        if (ft_isspace(text[i]))
            ;
        else if (text[i] == '(')
        {
            is_parent++;
            i++;
            while(ft_isspace(text[i]))
                i++;
            if(text[i] == ')')
                return true;
            continue;
        }
        else if (text[i] == ')')
        {
            if (i == 0)
                return true;
            if (text[i])
                is_parent--;
        }
        else if (text[i] == '\'' && quote == 0)
            quote = '\'';
        else if (text[i] == '"' && quote == 0)
            quote = '"';
        else if (text[i] == quote)
            quote = 0;
        else if (ft_strchr("&|", text[i]))
            is_operator = 1;
        else if (text[i] && text[i] != '\n')
            is_operator = 0;
        i++;
    }
    if (quote || is_operator || is_parent)
        return (true);
    return (false);
}

int main(int argc, char **argv, char **envp)
{
    char *text;
    t_file *input;
    t_file *output;
    int i;
    t_token **tokens;
    t_node *curr;

    global.values = (char *[]){"<<", ">>", "<", ">", "&&", "||", "(", ")", "|", "*", 0};
    global.types = (t_type[]){heredoc_, append_, redir_input, redir_output, and_, or_, lparent_, rparent_, pipe_, star_, 0};
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

    while (argc || argv)
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