#include "header0.h"

struct
{
    char *value;
    t_type type;
} symbol_lexic[] = {
    {"<<", heredoc_}, {">>", append_}, {"<", redir_input}, 
    {">", redir_output}, {"&&", and_}, {"||", or_}, 
    {"(", lparent_}, {")", rparent_}, {"|", pipe_}, 
    {"*", star_}, {0, 0}};

// new structs functions
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

    return list;
}

t_node *new_node(t_token *token)
{
    t_node *new = ft_calloc(1, sizeof(t_node));
    new->token = token;
    // if(new->token)
    //     ft_printf(out, "new  node with type %15t has token: '%k'\n", token->type, new->token);
    return new;
}

t_token *new_token(t_type type, char *source, int len)
{
    t_token *new = ft_calloc(1, sizeof(t_token));
    new->type = type;
    ft_printf(out, "new token with type %12t ", new->type);
    if (source)
    {
        new->value = ft_calloc(len + 1, sizeof(char));
        ft_strncpy(new->value, source, len);
        ft_printf(out, "has value: '%k'", new);
    }
    ft_printf(out, "\n");
    return new;
};

t_file new_file(char *name, int fd, t_type type)
{
    t_file new;
    new.name = name;
    new.fd = fd;
    new.type = type;
    add_to_list(&global.fds, &new.fd);
    return new;
}

// Signals
void handle_signal(int signum)
{
    if (signum == CTRL_C)
    {
        ft_putstr(out, "\n");
        rl_replace_line("", 0);
        rl_on_new_line();
        rl_redisplay();
    }
    if (signum == CTRL_SLASH)
    {
        ft_putstr(out, "");
        rl_replace_line("", 0);
        rl_on_new_line();
        rl_redisplay();
    }
}
void handle_heredoc_signal(int signum)
{
    if (signum == CTRL_C)
        ft_exit(SUCCESS);
    if (signum == CTRL_SLASH)
    {
        ft_putstr(out, "");
        rl_replace_line("", 0);
        rl_on_new_line();
        rl_redisplay();
    }
}

void add_to_list(t_list *list, void *value)
{
    if (list->type == integers_)
        list->integers[list->pos++] = *((int *)value);
    if (list->type == pointers_)
        list->pointers[list->pos++] = value;

    if (list->pos + 10 > list->len)
    {
        list->len *= 2;
        if (list->type == integers_)
            list->integers = ft_realloc(list->integers, list->pos * list->size, list->len * list->size);
        if (list->type == pointers_)
            list->pointers = ft_realloc(list->pointers, list->pos * list->size, list->len * list->size);
    }
}

void clear_list(t_list *list)
{
    t_type type = list->type;
    if (type == integers_)
        ft_memset(list->integers, 0, list->len * list->size);
    else if (type == pointers_)
        ft_memset(list->pointers, 0, list->len * list->size);
    // else
    //     ft_printf(err, " \033[0;31mError: Received unknown type to clear\n\033[0m");
    list->pos = 0;
}

void build_envirement(char **envp)
{
    int i = 0;
    while (envp && envp[i])
    {
        char **array = split_by_two(envp[i], '='); // split by only two, because value my contains '='
        t_node *node = new_node(new_token(assign_, NULL, 0));
        node->left = new_node(new_token(identifier_, NULL, 0));
        node->left->token->value = array[0];
        node->right = new_node(new_token(identifier_, NULL, 0));
        node->right->token->value = array[1];
        add_to_list(&global.envirement, node);
        i++;
    }
}

int is_inclosed(char *text)
{
    int quote = 0;
    int is_operator = 0;
    int i = 0;
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
        return true;
    return false;
}

void add_to_addresses(void *pointer)
{
    if (global.addresses.len == 0)
    {
        global.addresses.pos = 0;
        global.addresses.len = 100;
        global.addresses.pointers = malloc(global.addresses.len * sizeof(void *));
    }
    global.addresses.pointers[global.addresses.pos++] = pointer;
    if (global.addresses.pos + 10 > global.addresses.len)
    {
        global.addresses.len *= 2;
        void *temporary = malloc(global.addresses.len * sizeof(void *));
        ft_memcpy(temporary, global.addresses.pointers, global.addresses.pos * sizeof(void *));
        free(global.addresses.pointers);
        global.addresses.pointers = temporary;
    }
}

void build_tokens(char *text)
{
    int txt_pos = 0;
    int start = 0;
    int single_quotes = 0;
    int double_quotes = 0;

    while (text && text[txt_pos])
    {
        single_quotes = 0;
        double_quotes = 0;

        if (text[txt_pos] == '\0')
            break;
        if (text[txt_pos] == '*')
        {
            add_to_list(&global.tokens, new_token(star_, text + txt_pos, 1));
            txt_pos++;
            char *new = getcwd(NULL, 0);
            DIR *curr_dir = opendir(new);
            free(new);
            struct dirent *curr_dir_content;
            curr_dir_content = readdir(curr_dir);
            while (curr_dir_content)
            {
                add_to_list(&global.tokens, new_token(identifier_, curr_dir_content->d_name, ft_strlen(curr_dir_content->d_name)));
                curr_dir_content = readdir(curr_dir);
            }
            // add_to_list(&global.tokens, new_token(star_end, NULL, 0));
            closedir(curr_dir);
            continue;
        }
        if (text[txt_pos] == ' ')
        {
            start = txt_pos;
            while (text[txt_pos] == ' ')
                txt_pos++;
            continue;
        }
        int i = 0;
        t_type type = 0;
        while (symbol_lexic[i].value)
        {
            if (ft_strncmp(symbol_lexic[i].value, &text[txt_pos], ft_strlen(symbol_lexic[i].value)) == 0)
            {
                start = txt_pos;
                type = symbol_lexic[i].type;
                txt_pos += ft_strlen(symbol_lexic[i].value);
                break;
            }
            i++;
        }
        if (type)
        {
            add_to_list(&global.tokens, new_token(type, text + start, txt_pos - start));
            continue;
        }
        start = txt_pos;
        while (text[txt_pos])
        {
            if (text[txt_pos] == '\'')
            {
                if (!double_quotes)
                    single_quotes = !single_quotes;
            }
            else if (text[txt_pos] == '"')
            {
                if (!single_quotes)
                    double_quotes = !double_quotes;
            }
            else
            {
                if (!single_quotes && !double_quotes && (ft_isspace(text[txt_pos]) || ft_strchr("|&<>()", text[txt_pos])))
                    break;
            }
            txt_pos++;
        }
        add_to_list(&global.tokens, new_token(identifier_, text + start, txt_pos - start));
    }
    add_to_list(&global.tokens, new_token(end_, NULL, 0));
}

void skip(t_type type)
{
    t_token *token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
    if (token->type != type)
    {    
        ft_printf(out, "minishell: syntax error expected '%s'\n", type_to_string(type)); // must exit or something
        ft_exit(SYNTAX_ERROR);
    }
    global.tokens.pos++;
}

t_node	*expr(void)
{
	return (and_or());
}

t_node	*and_or(void)
{
	t_node	*left;
	t_token	*token;
	t_node	*node;

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

t_node	*pipe_node(void)
{
	t_node	*left;
	t_token	*token;
	t_node	*node;

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

bool	is_redirection(t_type type)
{
	return (type == redir_input || type == redir_output || type == heredoc_
		|| type == append_);
}

t_node	*prime(void)
{
	t_token	*token;
	t_node	*node;

	token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
	if (token->type == identifier_ || token->type == star_
		|| is_redirection(token->type))
	{
		node = new_node(token);
		node->token->start = global.tokens.pos;
		node->token->len++;
		skip(token->type);
		token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
		while (token->type == identifier_ || token->type == star_
			|| is_redirection(token->type))
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
		while (token->type == identifier_ || token->type == star_
			|| is_redirection(token->type))
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
    if(cmd == NULL || ft_strchr(cmd, '/'))
        return cmd;
    int i = 0;
    while(global.path && global.path[i])
    {
        char *res = strjoin(global.path[i], "/", cmd);
        if (access(res, F_OK) == 0 && access(res, X_OK) == 0)
            return (res);
        i++;
    }
    return cmd;
}

// built in functions
void echo_func(int fd, char **arguments)
{
    int print_new_line = 1;
    int i = 0;
    while(arguments && arguments[i])
    {
        if(i == 0 && ft_strcmp(arguments[i], "-n") == 0)
            print_new_line = 0;
        else
            ft_printf(fd, "%s", arguments[i]);
        i++;
    }
    if(print_new_line)
        ft_printf(fd, "\n");
}

void cd_func(int fd, char **arguments)
{
    if(arguments[0])
        chdir(arguments[0]); // verify if this little shit leaks
    else
        chdir(getenv("HOME")); // verify if it leaks
}

void pwd_func(int fd, char **arguments)
{
    char *new = getcwd(NULL, 0);
    ft_printf(fd, "%s\n", new);
    free(new);
}

void export_func(int fd, char **arguments)
{
    if(arguments[0])
    {
        char **array = split_by_two(arguments[0], '='); // split by only two, because value my contains '='
        t_node *node = new_node(new_token(assign_, NULL, 0));
        node->left = new_node(new_token(identifier_, NULL, 0));
        node->left->token->value = array[0];
        node->right = new_node(new_token(identifier_, NULL, 0));
        node->right->token->value = array[1];
        add_to_list(&global.envirement, node);
        return;
    }
    else
    {
        int i = 0;
        t_node **envirement = (t_node**)global.envirement.pointers;
        while (envirement && envirement[i])
        {
            if(envirement[i]->left->token->value)
                ft_printf(fd ,"declare -x %s=\"%s\"\n", envirement[i]->left->token->value, envirement[i]->right->token->value);
            i++;
        }
    }
}

void unset_func(int fd, char **argument)
{
    int i = 0;
    t_node **envirement = (t_node**)global.envirement.pointers;
    while (envirement && envirement[i])
    {
        char *value = envirement[i]->left->token->value;
        if(value && ft_strcmp(value, argument[0]) == 0)
        {
            free(envirement[i]->left->token->value);
            envirement[i]->left->token->value = NULL;
            free(envirement[i]->right->token->value);
            envirement[i]->right->token->value = NULL;
            break;
        }
        i++;
    }
}

void env_func(int fd, char **arguments)
{
    int i = 0;
    t_node **envirement = (t_node**)global.envirement.pointers;
    while(envirement[i])
    {
        if(envirement[i]->left->token->value)
            ft_printf(fd, "%s=%s\n", envirement[i]->left->token->value, envirement[i]->right->token->value);
        i++;
    }
}

void exit_func(int fd, char **arguments)
{
    ft_printf(out, "exit with %d\n", ft_atoi(arguments[0]));
    ft_exit(ft_atoi(arguments[0])); // verify exit code after
}

void* built_in(char *cmd)
{
    if(ft_strcmp(cmd, "echo") == 0)
        return &echo_func;
    if(ft_strcmp(cmd, "cd") == 0)
        return &cd_func;
    if(ft_strcmp(cmd, "pwd") == 0)
        return &pwd_func;
    if(ft_strcmp(cmd, "export") == 0)
        return &export_func;
    if(ft_strcmp(cmd, "unset") == 0)
        return &unset_func;
    if(ft_strcmp(cmd, "env") == 0)
        return &env_func;
    if(ft_strcmp(cmd, "exit") == 0)
        return &exit_func;
    return NULL;
}

// expanding
char *get_var(char *name)
{
    int i = 0;
    t_node **nodes = (t_node **)global.envirement.pointers;
    if(ft_strcmp(name, "?") == 0)
    {
        return ft_itoa(get_last_exit_code());
    }
    while(nodes && nodes[i])
    {
        if(ft_strcmp(nodes[i]->left->token->value, name) == 0)
        {
            return nodes[i]->right->token->value;
        }
        i++;
    }
    return "";
}

char *expand(t_token *token)
{
    char *res = NULL;
    int i = 0;
    int single_quotes = 0;
    int double_quotes = 0;

    char *value = token->value;
    while(value && value[i])
    {
        if(value[i] == '\'')
        {
            if(!double_quotes)
                single_quotes = !single_quotes;
            else
            {
                res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
                res[ft_strlen(res)] =  value[i];
            }
            i++;
        }
        else if(value[i] == '"')
        {
            if(!single_quotes)
                double_quotes = !double_quotes;
            else
            {
                res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
                res[ft_strlen(res)] =  value[i];
            }
            i++;
        }
        else if (token->value[i] == '$')
        {
            // if(token->value[i] == '$' && )
            if(double_quotes || !single_quotes)
            {
                i++;
                int j = i;
                while (token->value[j] && !ft_strchr(" \"", token->value[j]))
                    j++;
                char *var = ft_calloc(j - i + 1, sizeof(char));
                ft_strncpy(var, token->value + i, j - i);
                char *to_add = get_var(var);
                res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + ft_strlen(to_add) + 2);
                ft_strncpy(res + ft_strlen(res), to_add, ft_strlen(to_add));           
                i = j;
            }
            else if (single_quotes)
            {
                res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
                res[ft_strlen(res)] =  token->value[i];
                i++;
            }
        }
        else
        {
            res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
            res[ft_strlen(res)] =  value[i];
            i++;
        }
    }
    return res;
}

void open_file(t_file *file)
{
    if(file->type == redir_input)
    {
        // ft_putstr(out, "redir input\n");
        if(file->fd == NOT_OPENED)
        {
            if(access(file->name, F_OK)) // check existance
            {    
                ft_printf(err, "minishell: %s: No such file or directory\n", file->name);
                ft_exit(NO_SUCH_FILE_OR_DIR);
            }
            else if(access(file->name, R_OK)) // check if readable
            {
                ft_printf(err, "minishell: %s: Permission denied\n", file->name);
                ft_exit(PERMISSION_DENIED);
            }
            else
                file->fd = open(file->name, O_RDONLY);
        }
    }
    if(file->type == redir_output)
    {
        // ft_putstr(out, "redir output\n");
        if(file->fd == NOT_OPENED)
        {
            if(access(file->name, F_OK) == 0 &&  access(file->name, W_OK)) // check if readable
            {
                ft_printf(err, "minishell: %s: Permission denied\n", file->name);
                ft_exit(PERMISSION_DENIED);
            }
            else
                file->fd = open(file->name,  O_WRONLY | O_CREAT | O_TRUNC);
        }   
    }
    if(file->type == append_)
    {
        // ft_putstr(out, "redir append\n");
        if(access(file->name, F_OK) == 0 &&  access(file->name, W_OK)) // check if readable
        {
            ft_printf(err, "minishell: %s: Permission denied\n", file->name);
            ft_exit(PERMISSION_DENIED);
        }
        else
            file->fd = open(file->name, O_WRONLY | O_CREAT | O_APPEND);
         
    }
}

int get_last_exit_code()
{
    int *pids = global.pids.integers;
    int status = 0;
    static int res = 0;
    if(global.pids.pos > 0)
    {
        waitpid(pids[global.pids.pos--], &status, 0);
        res = WEXITSTATUS(status);
        return(res);
    }
    return res;
}

t_value *evaluate(t_node *node, t_file input, t_file output)
{
    // ft_printf(out, "Evaluate %k\n", node->token);
    switch(node->token->type)
    {
        case identifier_:
        case redir_input: // verify if need to be removed
        case redir_output:
        case heredoc_: 
        case append_:
        {
            int pid = 0;
            char **arguments = NULL;
            int len = 0;
            int pos = node->token->start;
            int i = 0;
            int include_hidden = 0;
            while(i < node->token->len)
            {
                t_token *token =((t_token**)global.tokens.pointers)[pos];
                pos++;
                if (token->type == identifier_ || token->type == star_)
                {
                    if(token->type == star_)
                    {
                        pos++;
                        token =((t_token**)global.tokens.pointers)[pos];
                    }
                    arguments = ft_realloc(arguments, len * sizeof(char*), (len + 2) * sizeof(char*));
                    char *argument = expand(token);
                    if(argument[0] != '.' || (argument[0] == '.' && include_hidden))
                        arguments[len++] = argument;
                    if(arguments[len - 1] && arguments[len - 1][0] == '-' && ft_strchr(arguments[len - 1], '-'))
                        include_hidden = 1;
                    arguments[len] = NULL;
                }
                else if(is_redirection(token->type)) // handle * here too
                {
                    t_type type = token->type;
                    token = ((t_token**)global.tokens.pointers)[pos];
                    if(token->type == star_)
                    {
                        ft_printf(out, "minishell: *: ambiguous redirection\n");
                        ft_exit(AMBIGIOUS_REDIRECTION);
                        return NULL;
                    }
                    else if(token->type != identifier_)
                    {
                        ft_printf(out, "minishell: Unexpected symbol after redirection\n"); // this error message to be checked
                        ft_exit(UNEXPECTED_ERROR);
                        return NULL;
                    }
                    pos++;
                    i++;
                    char *filename = expand(token);
                    // ft_printf(out, "is redirection type %s, has filename %s\n\n", type_to_string(type), filename);
                    if(type == redir_input)
                        input = new_file(filename, NOT_OPENED, type); // be carefull here because maybe you will get back to stdin 
                    if(type == redir_output || type == append_)                    
                        output = new_file(filename, NOT_OPENED, type); // be carefull here because maybe you will get back to stdout
                    if(type == heredoc_)
                    {
                        // print error if filename is NULL;
                        char *delimiter = filename;
                        input =  new_file(ft_strdup("/tmp/heredoc"), open("/tmp/heredoc", O_WRONLY | O_CREAT | O_TRUNC), redir_output);
                        int pid = fork();
                        if(pid == 0)
                        {
                            signal(SIGINT, handle_heredoc_signal);
                            signal(SIGQUIT, handle_heredoc_signal);
                            char *res = NULL;
                            while(res == NULL || ft_strcmp(res, delimiter))
                            {
                                res = readline("heredoc $> ");
                                if(res)
                                    write(input.fd, res, ft_strlen(res));
                            }
                            // close(input.fd);

                            ft_exit(SUCCESS);
                        }
                        int status;
                        waitpid(pid, &status , 0);
                        close(input.fd);
                        input.type = redir_input;
                        input.fd = NOT_OPENED;
                    }
                }
                i++;
            }
            if(arguments && arguments[0])
            {
                // built in function
                void (*func)(int, char**) = built_in(arguments[0]);
                if(func)
                {
                    // if inside_pipe fork
                    if(global.inside_pipe)
                    {
                        // ft_printf(out, "built in function, with forking\n");
                        pid = fork();
                        add_to_list(&global.pids, &pid);
                        if (pid == 0)
                        {  
                            // int *fds = global.fds.integers;
                            // int i = 0;
                            // while(i < global.fds.pos)
                            // {
                            //     if(fds[i] != input.fd && fds[i] != output.fd && fds[i] != out && fds[i] != in)
                            //         close(fds[i]);
                            //     i++;
                            // }
                            // while(global.pids.pos > 0) // can be removed
                            //     get_last_exit_code();
                            if(input.type == redir_input)
                            {
                                open_file(&input);
                                dup2(input.fd, in);
                                close(input.fd);
                            }
                            if(output.type == redir_output || output.type == append_)
                            {
                                open_file(&output);
                                dup2(output.fd, out);
                                close(output.fd);
                            }                       
                            func(out, &arguments[1]);
                            ft_exit(SUCCESS);
                        }
                    }
                    else
                    {
                        ft_printf(out, "built in function, without forking\n");
                        // to check after
                        if(input.type == redir_input)
                        {
                            open_file(&input);
                            dup2(input.fd, in);
                            close(input.fd);
                        }
                        if(output.type == redir_output || output.type == append_)
                        {
                            open_file(&output);
                            dup2(output.fd, out);
                            close(output.fd);
                        } 
                        // open_file(&output);
                        func(output.fd ,&arguments[1]);
                        // pid = fork();
                        // add_to_list(&global.pids, &pid);

                        // if(pid == 0)
                        //     ft_exit(SUCCESS);
                    }
                }
                // executable
                else
                {
                    // char *full_command = get_command_path(arguments[0]);
                    arguments[0] = get_command_path(arguments[0]);
                    // ft_printf(out,"full command: %s\n    input : %F\n    output: %F\n       has arguments:\n", arguments[0], input, output);
                    // int i = 1;
                    // while(arguments[i])
                    // {
                    //     ft_printf(out,"     %s\n", arguments[i]);
                    //     i++;
                    // }
                    
                    pid = fork();
                    add_to_list(&global.pids, &pid);

                    if (pid == 0)
                    {  
                        
                        if(input.type == redir_input)
                        {
                            // ft_printf(out, "call redirect input\n");
                            open_file(&input);
                            dup2(input.fd, in);
                            // close(input.fd);
                        }
                        if(output.type == redir_output || output.type == append_)
                        {
                            // ft_printf(out, "call redirect output\n");
                            open_file(&output);
                            dup2(output.fd, out);
                            // close(output.fd);
                        }
                        // ft_printf(out, "full command is %s\n    has  input: %F\n    has output: %F\n", full_command, input, output);
                        int *fds = global.fds.integers;
                        int i = 0;
                        while(i < global.fds.pos)
                        {
                            if(fds[i] != out && fds[i] != in)
                                close(fds[i]);
                            i++;
                        }
                        while(global.pids.pos > 0)
                            get_last_exit_code();
                        // when argument[1]=NULL, an error happen
                        // when argument[1] and you give &argument[0] as argument, somwthing enexpectabe happen
                        // check exit code if command not valid
                        if(execve(arguments[0], &arguments[0], global.env) != 0)
                        {
                            ft_printf(out, "minishell: '%s' command not found\n", arguments[0]); // check if there  other errors
                            ft_exit(COMMAND_NOT_FOUND);
                        }
                        // exit(SUCCESS);
                    }
                }                
            }
            // node->token->process_id = pid;
            return node->token;
            // break;
        }
        case pipe_:
        {
            // ft_printf(out, "open pipe\n");
            int fd[2];

            if(pipe(fd) < 0 )
                ft_printf(err, "Error opening pipe\n");
            
            add_to_list(&global.fds, &fd[0]);
            add_to_list(&global.fds, &fd[1]);

            global.inside_pipe++;
            evaluate(node->left, input, new_file(NULL, fd[1], redir_output));

            evaluate(node->right, new_file(NULL, fd[0], redir_input), output);
            global.inside_pipe--;

            // to be checked
            int *fds = global.fds.integers;
            close(fds[--global.fds.pos]);
            close(fds[--global.fds.pos]);

            break;
        }
        case and_:
        case or_:
        {
            // ft_printf(out, "%k has:\n    left %k\n    right %k\n\n",node->token, node->left->token, node->right->token);
            // int status = get_last_exit_code();
            // if(node->token->type == and_ && status == 0)
            t_value *value = evaluate(node->left, input, output);
            int status = get_last_exit_code();
            if(status == 0 && node->token->type == and_)
                value = evaluate(node->right, input, output);
            if(status != 0 && node->token->type == or_)
                value = evaluate(node->right, input, output);
            return value;
            // break;
        }
        case lparent_:
        {
            // ft_printf(out, "Found lparent\n");
            int pos = node->token->start;
            int i = 0;
            while(i < node->token->len)
            {
                t_token *token =((t_token**)global.tokens.pointers)[pos];
                pos++;
                if(is_redirection(token->type)) // handle * here too
                {
                    t_type type = token->type;
                    token = ((t_token**)global.tokens.pointers)[pos];
                    if(token->type == star_)
                    {
                        ft_printf(out, "minishell: *: ambiguous redirection\n");
                        ft_exit(AMBIGIOUS_REDIRECTION);
                        return NULL;
                    }
                    else if(token->type != identifier_)
                    {
                        ft_printf(out, "minishell: Unexpected symbol after redirection\n"); // this error message to be checked
                        ft_exit(UNEXPECTED_ERROR);
                        return NULL;
                    }
                    pos++;
                    i++;
                    char *filename = expand(token);
                    // ft_printf(out, "is redirection type %s, has filename %s\n\n", type_to_string(type), filename);
                    if(type == redir_input)
                        input = new_file(filename, NOT_OPENED, type); // be carefull here because maybe you will get back to stdin 
                    if(type == redir_output || type == append_)                    
                        output = new_file(filename, NOT_OPENED, type); // be carefull here because maybe you will get back to stdout
                    if(type == heredoc_)
                    {
                        // print error if filename is NULL;
                        char *delimiter = filename;
                        input =  new_file(ft_strdup("/tmp/heredoc"), open("/tmp/heredoc", O_WRONLY | O_CREAT | O_TRUNC), redir_output);
                        int pid = fork();
                        if(pid == 0)
                        {
                            signal(SIGINT, handle_heredoc_signal);
                            signal(SIGQUIT, handle_heredoc_signal);
                            char *res = NULL;
                            while((res == NULL || ft_strcmp(res, delimiter)))
                            {
                                res = readline("heredoc $> ");
                                if(res)
                                    write(input.fd, res, ft_strlen(res));
                            }

                            ft_exit(SUCCESS);
                        }
                        int status;
                        waitpid(pid, &status , 0);
                        close(input.fd);
                        input.type = redir_input;
                        input.fd = NOT_OPENED;
                    }
                }
                i++;
            }
            if(output.type == redir_output && output.name)
            {
                // ft_printf(out, "Redir output\n");
                open_file(&output);
                write(output.fd, "", ft_strlen(""));
                close(output.fd);
                output.type = append_;
                output.fd = NOT_OPENED;
            }
            return (evaluate(node->left, input, output));
            // break;
        }
        default:
            ft_printf(err, "Error in evaluate\n");
        }
    return NULL;
}

int main(int argc, char **argv, char **envp)
{
    global.env = envp;
    new_list(&global.envirement, sizeof(t_node*), pointers_);
    new_list(&global.tokens, sizeof(t_token*), pointers_);
    new_list(&global.pids, sizeof(int), integers_);
    new_list(&global.fds, sizeof(int), integers_);
    char *text;
    int i = 0;
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

        t_file input = new_file(NULL, in, 0);
        t_file output = new_file(NULL, out, 0);
        text = readline("minishell $> ");

        add_to_addresses(text);
        if (is_inclosed(text))
        {
            text = ft_realloc(text, ft_strlen(text), ft_strlen(text) + 2);
            text[ft_strlen(text)] = '\n';
            char *tmp = text;
            while (tmp && is_inclosed(text))
            {
                tmp = ft_readline("> ");
                text = ft_realloc(text, ft_strlen(text), ft_strlen(text) + ft_strlen(tmp) + 1);
                // free(text);
                // text = ptr;
                ft_strcpy(text + ft_strlen(text), tmp);
                // text = tmp;
            }
            text[ft_strlen(text) - 1] = 0;
        }
        if (text)
            add_history(text);
        
        build_tokens(text);
        // exit(0);
        global.tokens.pos = 0;

        t_token **tokens =  (t_token**)global.tokens.pointers;

        while(tokens[global.tokens.pos]->type != end_)
            evaluate(expr(), input, output);
        
        int status;
        while(global.pids.pos > 0)
            status = get_last_exit_code();
        
        if(text == NULL)
            break; // exit if text is NULL
        text = NULL;
        clear_list(&global.tokens);
        clear_list(&global.pids);
        clear_list(&global.fds);
    }
    ft_exit(0);
}