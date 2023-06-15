#include "header.h"

t_list *new_list(t_list *list, size_t size, t_type type)
{
    list->pos = 0;
    list->len = 100;
    list->type = type;
    list->size = size;
    if (list->type == pointers_)
        list->pointers = ft_calloc(list->len, list->size);
    if (list->type == integers_)
        list->integers = ft_calloc(list->len, list->size);
    return (list);
}

t_node *new_node(t_token *token)
{
    t_node *new;

    new = ft_calloc(1, sizeof(t_node));
    new->token = token;
    if (new->token)
        ft_printf(OUT, "new  node with type %6t has token: '%k'\n", token->type, new->token);
    return (new);
}

t_token *new_token(t_type type, char *source, int len)
{
    t_token *new;

    new = ft_calloc(1, sizeof(t_token));
    new->type = type;
    ft_printf(OUT, "new token with type %6t ", new->type);
    if (source)
    {
        new->value = ft_calloc(len + 1, sizeof(char));
        ft_strncpy(new->value, source, len);
        ft_printf(OUT, "has value: '%k'", new);
    }
    ft_printf(OUT, "\n");
    return (new);
}

t_file new_file(char *name, int fd, t_type type)
{
    t_file new;

    new.name = name;
    new.fd = fd;
    new.type = type;
    add_number(&global.fds, new.fd);
    return (new);
}

void handle_signal(int signum)
{
    if (signum == CTRL_C)
    {
        printf("\n");
        rl_replace_line("", 0);
        rl_on_new_line();
        rl_redisplay();
    }
    if (signum == CTRL_SLASH)
    {
        printf("");
        rl_replace_line("", 0);
        rl_on_new_line();
        rl_redisplay();
    }
}

void handle_heredoc_signal(int signum)
{
    if (signum == CTRL_C)
    {
        printf("\n");
        rl_replace_line("", 0);
        rl_on_new_line();
        rl_redisplay();
        ft_exit(SUCCESS);
    }
    if (signum == CTRL_SLASH)
    {
        printf("");
        rl_replace_line("", 0);
        rl_on_new_line();
        rl_redisplay();
    }
}

void add_pointer(t_list *list, void *value)
{
    list->pointers[list->pos++] = value;
    if (list->pos + 10 > list->len)
    {
        list->len *= 2;
        list->pointers = ft_realloc(list->pointers, list->pos * list->size, list->len * list->size);
    }
}

void add_number(t_list *list, int number)
{
    list->integers[list->pos++] = number;
    if (list->pos + 10 > list->len)
    {
        list->len *= 2;
        list->integers = ft_realloc(list->integers, list->pos * list->size, list->len * list->size);
    }
}

void clear_list(t_list *list)
{
    t_type type;

    type = list->type;
    if (type == integers_)
        ft_memset(list->integers, 0, list->len * list->size);
    else if (type == pointers_)
        ft_memset(list->pointers, 0, list->len * list->size);
    list->pos = 0;
}

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

void build_tokens(char *text)
{
    int txt_pos;
    int start;
    int single_quote;
    int double_quote;
    DIR *curr_dir;
    struct dirent *content;
    char *new;
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
        if (text[txt_pos] == '*')
        {
            add_pointer(&global.tokens, new_token(star_, text + txt_pos, 1));
            txt_pos++;
            new = getcwd(NULL, 0);
            curr_dir = opendir(new);
            free(new);
            content = readdir(curr_dir);
            while (content)
            {
                add_pointer(&global.tokens, new_token(identifier_, content->d_name, ft_strlen(content->d_name)));
                content = readdir(curr_dir);
            }
            closedir(curr_dir);
            continue;
        }
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
            else if (!single_quote && !double_quote && (ft_isspace(text[txt_pos]) || ft_strchr("|&<>()", text[txt_pos])))
                break;
            txt_pos++;
        }
        if (single_quote || double_quote)
        {
            if (single_quote)
                printf("minishell: syntax error expected '\''\n");
            else if (double_quote)
                printf("minishell: syntax error expected '\"'\n");
            ft_exit(SYNTAX_ERROR);
        }
        add_pointer(&global.tokens, new_token(identifier_, text + start, txt_pos - start));
    }
    add_pointer(&global.tokens, new_token(end_, NULL, 0));
}

void skip(t_type type)
{
    t_token *token;

    token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
    if (token->type != type)
    {
        printf("minishell: syntax error expected '%s'\n", type_to_string(type));
        ft_exit(SYNTAX_ERROR);
    }
    global.tokens.pos++;
}

bool check(t_type type, t_type types[])
{
    int i = 0;

    while (types[i])
    {
        if (types[i] == type)
            return true;
        i++;
    }
    return false;
}

t_node *expr(void)
{
    return (and_or());
}

t_node *and_or(void)
{
    t_node *left;
    t_token *token;
    t_node *node;

    left = pipe_node();
    token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
    while (token->type == and_ || token->type == or_)
    {
        node = new_node(token);
        skip(token->type);
        node->left = left;
        node->right = pipe_node();
        left = node;
        token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
    }
    return (left);
}

t_node *pipe_node(void)
{
    t_node *left;
    t_token *token;
    t_node *node;

    left = prime();
    token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
    while (token->type == pipe_)
    {
        node = new_node(token);
        skip(pipe_);
        node->left = left;
        node->right = prime();
        left = node;
        token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
    }
    return (left);
}

bool is_redirection(t_type type)
{
    return (type == redir_input || type == redir_output || type == heredoc_ || type == append_);
}

t_node *prime(void)
{
    t_token *token;
    t_node *node;

    token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
    if (token->type == identifier_ || token->type == star_ || is_redirection(token->type))
    {
        node = new_node(token);
        node->token->start = global.tokens.pos;
        node->token->len++;
        skip(token->type);
        token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
        while (token->type == identifier_ || token->type == star_ || is_redirection(token->type))
        {
            node->token->len++;
            skip(token->type);
            token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
        }
        return (node);
    }
    if (token->type == lparent_)
    {
        node = new_node(token);
        skip(token->type);
        node->left = expr();
        skip(rparent_);
        token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
        if (token->type == identifier_ || token->type == star_)
        {
            printf("minishell: syntax error unexpected '%s'\n", token->value);
            ft_exit(SYNTAX_ERROR);
        }
        node->token->start = global.tokens.pos;
        node->token->len++;
        while (token->type == identifier_ || token->type == star_ || is_redirection(token->type))
        {
            node->token->len++;
            skip(token->type);
            token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
        }
        return (node);
    }
    return (NULL);
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

void echo_func(char **arguments)
{
    int print_new_line;
    int i;

    print_new_line = 1, i = 0;
    while (arguments && arguments[i])
    {
        if (i == 0 && ft_strcmp(arguments[i], "-n") == 0)
            print_new_line = 0;
        else
            printf("%s", arguments[i]);
        i++;
    }
    if (print_new_line)
        printf("\n");
}

void cd_func(char **arguments)
{
    if (arguments[0])
    {
        ft_printf(OUT, "CD has arguments\n");
        chdir(arguments[0]); // verify if this little shit leaks
    }
    else
    {
        ft_printf(OUT, "CD has no arguments\n");
        chdir(getenv("HOME")); // verify if it leaks
    }
}

void pwd_func(char **arguments)
{
    char *new;

    new = getcwd(NULL, 0);
    printf("%s\n", new);
    free(new);
}

void export_func(char **arguments)
{
    char **array;
    t_node *node;
    t_node **envirement;
    int i;

    if (arguments[0])
    {
        array = split_by_two(arguments[0], '=');
        node = new_node(new_token(assign_, NULL, 0));
        node->left = new_node(new_token(identifier_, NULL, 0));
        node->left->token->value = array[0];
        node->right = new_node(new_token(identifier_, NULL, 0));
        node->right->token->value = array[1];
        add_pointer(&global.envirement, node);
        return;
    }
    else
    {
        i = 0;
        envirement = (t_node **)global.envirement.pointers;
        while (envirement && envirement[i])
        {
            if (envirement[i]->left->token->value)
                printf("declare -x %s=\"%s\"\n",
                       envirement[i]->left->token->value,
                       envirement[i]->right->token->value);
            i++;
        }
    }
}

void unset_func(char **argument)
{
    int i;
    t_node **envirement;
    char *value;

    // ft_printf(OUT, "unset %s\n", argument[0]);
    envirement = (t_node **)global.envirement.pointers, i = 0;
    while (envirement && envirement[i])
    {
        value = envirement[i]->left->token->value;
        if (value && ft_strcmp(value, argument[0]) == 0)
        {
            // free(envirement[i]->left->token->value);
            envirement[i]->left->token->value = NULL;
            // free(envirement[i]->right->token->value);
            envirement[i]->right->token->value = NULL;
            break;
        }
        i++;
    }
    if (ft_strcmp(argument[0], "PATH") == 0)
        global.path = NULL;
}

void env_func(char **arguments)
{
    int i;
    t_node **envirement;

    envirement = (t_node **)global.envirement.pointers, i = 0;
    while (envirement[i])
    {
        if (envirement[i]->left->token->value)
            printf("%s=%s\n", envirement[i]->left->token->value, envirement[i]->right->token->value);
        i++;
    }
}

void exit_func(char **arguments)
{
    // to be verified because it's not working properly
    printf("exit with %d\n", ft_atoi(arguments[0]));
    ft_exit(ft_atoi(arguments[0])); // verify exit code after
}

void *built_in(char *cmd)
{
    if (ft_strcmp(cmd, "echo") == 0)
        return (&echo_func);
    if (ft_strcmp(cmd, "cd") == 0)
        return (&cd_func);
    if (ft_strcmp(cmd, "pwd") == 0)
        return (&pwd_func);
    if (ft_strcmp(cmd, "export") == 0)
        return (&export_func);
    if (ft_strcmp(cmd, "unset") == 0)
        return (&unset_func);
    if (ft_strcmp(cmd, "env") == 0)
        return (&env_func);
    if (ft_strcmp(cmd, "exit") == 0)
        return (&exit_func);
    return (NULL);
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

void open_file(t_file *file)
{
    if (file->fd == NOT_OPENED)
    {
        if (file->type == redir_input)
        {
            if (access(file->name, F_OK))
            {
                printf("minishell: %s No such file or directory\n", file->name);
                ft_exit(NO_SUCH_FILE_OR_DIR);
            }
            else if (access(file->name, R_OK))
            {
                printf("minishell: %s Permission denied\n", file->name);
                ft_exit(PERMISSION_DENIED);
            }
            else
                file->fd = open(file->name, O_RDONLY);
        }
        if (file->type == redir_output && file->name)
        {
            if (access(file->name, F_OK) == 0 && access(file->name, W_OK))
            {
                printf("minishell: %s Permission denied\n", file->name);
                ft_exit(PERMISSION_DENIED);
            }
            else
                file->fd = open(file->name, O_WRONLY | O_CREAT | O_TRUNC);
        }
        if (file->type == append_ && file->name)
        {
            if (access(file->name, F_OK) == 0 && access(file->name, W_OK))
            {
                printf("minishell: %s Permission denied\n", file->name);
                ft_exit(PERMISSION_DENIED);
            }
            else
                file->fd = open(file->name, O_WRONLY | O_CREAT | O_APPEND);
        }
    }
}

int get_last_exit_code(void)
{
    int *pids;
    int status;
    static int res;

    pids = global.pids.integers, status = 0;
    if (global.pids.pos > 0)
    {
        waitpid(pids[--global.pids.pos], &status, 0);
        res = WEXITSTATUS(status);
    }
    return (res);
}

t_value *evaluate(t_node *node, t_file input, t_file output)
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
#if 0
                    delimiter = filename;
                    filename = strjoin(".heredoc", ft_itoa(heredoc_index++), NULL);
                    input = new_file(filename, open(filename, O_TRUNC | O_WRONLY | O_CREAT), redir_output);
                    int pid = fork();
                    if (pid == 0)
                    {
                        signal(SIGINT, handle_heredoc_signal);
                        signal(SIGQUIT, handle_heredoc_signal);
                        heredoc_text = NULL;
                        while (heredoc_text == NULL || ft_strcmp(heredoc_text, delimiter))
                        {
                            heredoc_text = readline("heredoc $> ");
                            if (heredoc_text)
                                write(input.fd, heredoc_text, ft_strlen(heredoc_text));
                        }
                        ft_exit(SUCCESS);
                    }
                    waitpid(pid, NULL, 0);
                    close(input.fd);
                    input.type = redir_input;
                    input.fd = NOT_OPENED;
#else
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
                    // close(fd[0]);
                    close(fd[1]);
#endif
                }
            }
            i++;
        }
        if (arguments && arguments[0])
        {
            void (*func)(char **) = built_in(arguments[0]);
            if (func)
            {
                if (global.inside_pipe)
                {
                    pid = fork();
                    add_number(&global.pids, pid);
                    if (pid == 0)
                    {
                        if (input.type == redir_input)
                        {
                            open_file(&input);
                            dup2(input.fd, IN);
                            close(input.fd);
                        }
                        if (output.type == redir_output || output.type == append_)
                        {
                            open_file(&output);
                            dup2(output.fd, OUT);
                            close(output.fd);
                        }
                        func(&arguments[1]);
                        ft_exit(SUCCESS);
                    }
                }
                else
                {
                    if (input.type == redir_input)
                    {
                        open_file(&input);
                        dup2(input.fd, IN);
                        close(input.fd);
                    }
                    if (output.type == redir_output || output.type == append_)
                    {
                        open_file(&output);
                        dup2(output.fd, OUT);
                        close(output.fd);
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
                    if(pid == 0)
                    {
                        ft_exit(SUCCESS);
                    }
                }
            }
            else
            {
                // command
                arguments[0] = get_command_path(arguments[0]);
                ft_printf(OUT, "execute command has input: %d, and output: %d\n", input.fd, output.fd);
                pid = fork();
                add_number(&global.pids, pid);
                if (pid == 0)
                {
                    if (input.type == redir_input)
                    {
                        open_file(&input);
                        ft_printf(OUT, "redirect input: %d, output: %d\n", input.fd, output.fd);
                        dup2(input.fd, IN);
                        close(input.fd);
                    }
                    if (output.type == redir_output || output.type == append_)
                    {
                        open_file(&output);
                        ft_printf(OUT, "redirect output: %d, input: %d\n", output.fd, input.fd);
                        dup2(output.fd, OUT);
                        close(output.fd);
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
                    // when argument[1]=NULL, an error happen
                    // when argument[1] and you give &argument[0] as argument, something enexpectabe happen
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
#if 0
                    delimiter = filename;
                    filename = strjoin(".heredoc", ft_itoa(heredoc_index++), NULL);
                    input = new_file(filename, open(filename, O_TRUNC | O_WRONLY | O_CREAT), redir_output);
                    int pid = fork();
                    if (pid == 0)
                    {
                        signal(SIGINT, handle_heredoc_signal);
                        signal(SIGQUIT, handle_heredoc_signal);
                        heredoc_text = NULL;
                        while (heredoc_text == NULL || ft_strcmp(heredoc_text, delimiter))
                        {
                            heredoc_text = readline("heredoc $> ");
                            if (heredoc_text)
                                write(input.fd, heredoc_text, ft_strlen(heredoc_text));
                        }
                        ft_exit(SUCCESS);
                    }
                    waitpid(pid, NULL, 0);
                    close(input.fd);
                    input.type = redir_input;
                    input.fd = NOT_OPENED;
#else
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
                        // close(fd[0]);
                        close(fd[1]);
#endif
                    }
                }
                i++;
            }
            if (output.type == redir_output)
            {
                open_file(&output);
                write(output.fd, "", ft_strlen(""));
                close(output.fd);
                output.type = append_;
                output.fd = NOT_OPENED;
            }
            t_value *value = evaluate(node->left, input, output);
            ft_printf(OUT, "Exit lparent 1\n");
            while (global.pids.pos > 0)
            {
#if 1
                status = get_last_exit_code();
#else
                waitpid(global.pids.integers[--global.pids.pos], &status, 0);
#endif
                // ft_printf(OUT, "wait pid\n");
            }
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
    int i;

    quote = 0;
    is_operator = 0;
    i = 0;
    while (text && text[i])
    {
        if (text[i] == '\'' && quote == 0)
            quote = '\'';
        else if (text[i] == '"' && quote == 0)
            quote = '"';
        else if (text[i] == quote)
            quote = 0;
        if (ft_strchr("&|", text[i]))
            is_operator = 1;
        else if (text[i] && text[i] != '\n')
            is_operator = 0;
        i++;
    }
    if (quote || is_operator)
        return (true);
    return (false);
}

int main(int argc, char **argv, char **envp)
{
    char *text;
    t_file input;
    t_file output;
    int i;
    t_token **tokens;

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

    while (1)
    {
        signal(SIGINT, handle_signal);
        signal(SIGQUIT, handle_signal);

        input = new_file(NULL, IN, 0);
        output = new_file(NULL, OUT, 0);

        text = readline("minishell $> ");
        if (is_inclosed(text))
        {
            printf("minishell: syntax error\n");
            exit(SYNTAX_ERROR);
        }
        if (text == NULL)
            break;
        else
            add_history(text);
        build_tokens(text);
        global.tokens.pos = 0;
        tokens = (t_token **)global.tokens.pointers;
        while (tokens[global.tokens.pos]->type != end_)
            evaluate(expr(), input, output);
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