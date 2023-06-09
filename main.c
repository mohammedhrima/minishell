#include "header.h"

// new struct
List	*new_list(List *list, size_t size, Type type)
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

File	*new_file(char *name, int fd, Type type)
{
	File	*new;

	new = ft_calloc(1, sizeof(File));
	new->name = name;
	new->fd = fd;
	new->type = type;
	add_number(&Global.fds, new->fd);
	return (new);
}

char	*type_to_string(Type type)
{
	char	**values;
	Type	*types;
	int		i;

	values = (char *[]){"=", "identifier", "<", ">", "<<", ">>", "|", "env",
		"echo", "&&", "||", "cd", "pwd", "export", "unset", "exit", "end", "*",
		"(", ")", NULL};
	types = (Type[]){assign_, identifier_, redir_input, redir_output,
		heredoc_, append_, pipe_, env_, echo_, and_, or_, cd_, pwd_, export_,
		unset_, exit_, end_, star_, lparent_, rparent_, 0};
	i = 0;
	while (types[i])
	{
		if (types[i] == type)
			return (values[i]);
		i++;
	}
	return (NULL);
}

Token	*new_token(Type type, char *source, int len)
{
	Token	*new;

	new = ft_calloc(1, sizeof(Token));
	new->type = type;
	if (source)
	{
		new->content = ft_calloc(len + 1, sizeof(char));
		ft_strncpy(new->content, source, len);
	}
	printf("new token with type %10s value %s\n", type_to_string(type), new->content);
	if(Global.add_to_tokens)
		add_pointer(&Global.tokens, new);
	return (new);
}

Node	*new_node(Token *token)
{
	Node	*new;

	new = ft_calloc(1, sizeof(Node));
	new->token = token;
    printf("new node with type %10s value %s\n", type_to_string(new->token->type), new->token->content);
	return (new);
}

void init_global(char **envp)
{
	Global.addresses.pointers = NULL;
	Global.addresses.pos = 0;
	Global.values = ft_calloc(11, sizeof(char *));
	Global.types = ft_calloc(11, sizeof(Type));
	ft_memcpy(Global.values, (char *[]){"<<", ">>", "<", ">", "&&", "||", "(", ")", "|", "*", 0}, 11 * sizeof(char *));
	ft_memcpy(Global.types, (Type[]){heredoc_, append_, redir_input, redir_output, and_, or_, lparent_, rparent_, pipe_, star_, 0}, 11 * sizeof(Type));
	new_list(&Global.envirement, sizeof(Node *), pointers_);
}

void build_envirement(char **envp)
{
	int i;
	char **array;
	Node *node;

	i = 0;
    Global.add_to_tokens = false;
	while (envp && envp[i])
	{
		array = split(envp[i], "=");
		node = new_node(new_token(assign_, NULL, 0));
		node->left = new_node(new_token(identifier_, array[0], ft_strlen(array[0])));
		node->right = new_node(new_token(identifier_, array[1], ft_strlen(array[1])));
		add_pointer(&Global.envirement, node);
		i++;
	}
	Global.tokens.pos = 0;
    Global.add_to_tokens = true;
}

// tokenize
void    build_tokens(char *line)
{
    int     pos;
    int     i;
    int     squote;
    int     dquote;
    int     start;

    pos = 0;
    while(line && line[pos])
    {
        i = 0;
        squote = 0;
        dquote = 0;
        while(line[pos] && ft_isspace(line[pos]))
            pos++;
        while(Global.values[i])
        {
            if(ft_strncmp(Global.values[i], line + pos, ft_strlen(Global.values[i])) == 0)
            {
                new_token(Global.types[i], Global.values[i], ft_strlen(Global.values[i]));
                pos += ft_strlen(Global.values[i]);
                break;
            }
            i++;
        }
        if(Global.values[i] == NULL)
        {
            start = pos;
            while(line && line[pos])
            {
                if(line[pos] == '\'' && !dquote)
                    squote = !squote;
                if(line[pos] == '"' && !squote)
                    dquote = !dquote;
                if(!squote && !dquote && ft_strchr("|&<>()*\t\n\f\r\v ", line[pos]))
                    break;
                pos++;
            }
            if(pos > start)
                new_token(identifier_, line + start, pos - start);
        }
    }
    // check if squote or dquote is syntax error
    new_token(end_, NULL, 0);
    if(dquote || squote)
    {
        printf("minishell: syntax error\n");
        ft_memset(Global.tokens.pointers, 0, Global.tokens.pos * sizeof(Token*));
    }
}

// nodes
Node    *expr_node();
Node    *logic_node();
Node    *pipe_node();
Node    *prime_node();
Node    *parenteses_node();

int skip(Type type)
{
    Token   *token;

    token = ((Token**)Global.tokens.pointers)[Global.tokens.pos];
    if(token->type != type)
    {
        printf("minishell: syntax error expected '%s'\n", type_to_string(type));
        return (SYNTAX_ERROR);
    }
    Global.tokens.pos++;
    return SUCCESS;
}

Node    *expr_node(void)
{
    return (logic_node());
}

Node    *logic_node(void)
{
    Node    *left;
    Node    *node;
    Token   *token;

    left = pipe_node();
    token = ((Token**)Global.tokens.pointers)[Global.tokens.pos];
    while(left && (token->type == and_ || token->type == or_))
    {
        node = new_node(token);
        skip(token->type);
        node->left = left;
        node->right = pipe_node();
        left = node;
        token = ((Token**)Global.tokens.pointers)[Global.tokens.pos];
    }
    return (left);
}

Node *pipe_node(void)
{
    Node    *left;
    Node    *node;
    Token   *token;

    left = prime_node();
    token = ((Token**)Global.tokens.pointers)[Global.tokens.pos];
    while(left && token->type == pipe_)
    {
        node = new_node(token);
        skip(pipe_);
        node->left = left;
        node->right = prime_node();
        left = node;
        token = ((Token**)Global.tokens.pointers)[Global.tokens.pos];
    }
    return (left);
}

Node *prime_node(void)
{
    Token   *heredoc_token;
    Token   *token;
    Node    *node;
    int     pid;

    token = ((Token **)Global.tokens.pointers)[Global.tokens.pos];
    if (token->type == identifier_ || token->type == star_ || is_redirection(token->type))
    {
        node = new_node(token);
        node->token->start = Global.tokens.pos;
        while (token->type == identifier_ || token->type == star_ || is_redirection(token->type))
        {
            // node->token->len++;
            if(token->type == heredoc_)
            {
                heredoc_token = token;
                skip(token->type);
                node->token->len++;
                token = ((Token **)Global.tokens.pointers)[Global.tokens.pos];
                if(token->type != identifier_)
                {
                    pid = new_child_process();
                    printf("minishell: syntax error unexpected '%s'\n", token->content);
                    if (pid == 0)
                        ft_exit(SYNTAX_ERROR);
                    while(token->type != end_)
                    {
                        Global.tokens.pos++;
                        token = ((Token **)Global.tokens.pointers)[Global.tokens.pos];
                    }
                    return (NULL);
                }
                heredoc_token->content = open_heredoc(expand(token->content));
                skip(token->type);
                node->token->len++;
                token = ((Token **)Global.tokens.pointers)[Global.tokens.pos];
            }
            else
            {
                skip(token->type);
                node->token->len++;
                token = ((Token **)Global.tokens.pointers)[Global.tokens.pos];
            }
        }
        return (node);
    }
    return (parenteses_node());
}

Node *parenteses_node()
{
    Token   *token;
    Node    *node;

    token = ((Token **)Global.tokens.pointers)[Global.tokens.pos];
    if(token->type == lparent_)
    {
        node = new_node(token);
        skip(token->type);
        node->left = expr_node();
        token = ((Token **)Global.tokens.pointers)[Global.tokens.pos];
        if(token->type != rparent_)
        {
            while(token->type != end_)
            {
                Global.tokens.pos++;
                token = ((Token **)Global.tokens.pointers)[Global.tokens.pos];
            }
            return (NULL);
        }
        skip(token->type);
        return (node);
    }
    return (NULL);
}

// heredoc
char *open_heredoc(char *delimiter)
{
    static int  i;
	int         pid;
	File        *input;
	char        *heredoc_text;
    char        *name;
    char        *tmp;
# if 1
    name = strjoin(".heredoc", ft_itoa(i++), NULL);
	pid = new_child_process();
	if (pid == 0)
    {
    	input = new_file(name, open(name, O_RDWR | O_TRUNC | O_CREAT, 0777), redir_output);
    	// signal(SIGINT, handle_heredoc_signal);
    	heredoc_text = readline("heredoc $> ");
        while (heredoc_text == NULL || ft_strcmp(heredoc_text, delimiter))
        {
            tmp = expand(heredoc_text);
            free(heredoc_text);
            write(input->fd, tmp, ft_strlen(tmp));
            write(input->fd, "\n", ft_strlen("\n"));
            heredoc_text = readline("heredoc $> ");
        }
        close(input->fd);
    	ft_exit(SUCCESS);
    }
	waitpid(pid, NULL, 0);
	// close(input->fd);
#endif
	return (name);
}

// execution
char **get_paths(void)
{
    int     i;
    Node    **envirement;
    Token   *left;
    Token   *right;
    char    **paths;

    envirement = (Node**)Global.envirement.pointers;
    i = 0;
    while(envirement && envirement[i])
    {
        left = envirement[i]->left->token;
        right = envirement[i]->right->token;
        // check if left or right are NULL
        if(left->content && ft_strcmp(left->content, "PATH") == 0)
        {
            // use lstat
            paths = split(right->content, ":");
            return paths;
        }
        i++;
    }
    return NULL;
}

char **get_envirement(void)
{
    int     i;
    int     j;
    Node    **envirement;
    Token   *left;
    Token   *right;
    char    **env;

    i = 0;
    j = 0;
    envirement = (Node**)Global.envirement.pointers;
    env = NULL;
    while(envirement && envirement[i])
    {
        left = envirement[j]->left->token;
        right = envirement[j]->right->token;
        // check if left or right are NULL
        if(left->content)
        {
            env = ft_realloc(env, j, j + 2, sizeof(char*));
            env[j] = strjoin(left->content, "=", right->content);
            j++;
        }
        i++;
    }
    return env;
}

char *get_env(char *name)
{
    int     i;
    Node    **envirement;
    Token   *left;
    Token   *right;

    i = 0;
    envirement = (Node**)Global.envirement.pointers;
    while(envirement && envirement[i])
    {
        left = envirement[i]->left->token;
        right = envirement[i]->right->token;
        if(left->content && ft_strcmp(left->content, name) == 0)
            return right->content;
        i++;
    }
    return NULL;
}

char *get_command_path(char *command)
{
    char    *res;
    int     i;
    char    **path;
    if(command == NULL || ft_strchr(command, '/') || ft_strlen(command) == 0)
        return command;
    path = get_paths();
    i = 0;
    while(path && path[i])
    {
        res = strjoin(path[i], "/", command);
        if (access(res, F_OK) == 0 && access(res, X_OK) == 0)
            return (res);
        i++;
    }
    return command;   
}

int	open_file(File *file, unsigned long access_type)
{
	if (access(file->name, F_OK) == 0 && access(file->name, W_OK))
	{
		printf("minishell: %s Permission denied in open file\n", file->name);
		ft_exit(PERMISSION_DENIED);
	}
	else
		file->fd = open(file->name, access_type);
    return 0;
}

void check_redirection(File *input, File *output)
{
    if (input->type == redir_input)
    {
        if (access(input->name, F_OK))
        {
            printf("minishell: %s No such file or directory\n", input->name);
            ft_exit(NO_SUCH_FILE_OR_DIR);
        }
        else
        {
            open_file(input, O_RDONLY);
            dup2(input->fd, IN);
            close(input->fd);
        }
    }
    if ((output->type == redir_output || output->type == append_) && output->name)
    {
        output->type == redir_output && open_file(output, O_WRONLY | O_CREAT | O_TRUNC);
        output->type == append_ && open_file(output, O_WRONLY | O_CREAT | O_APPEND);
        dup2(output->fd, OUT);
        close(output->fd);
    }
}

char *get_var(char *name)
{
    int     i;
    Node    **nodes;

    nodes = (Node**)Global.envirement.pointers;
    i = 0;
#if 0
    if(ft_strcmp(name, "?") == 0)
        return (ft_itoa(get_last_exit_code()));
#endif
    while(nodes && nodes[i])
    {
        if(nodes[i]->left->token->content && ft_strcmp(nodes[i]->left->token->content, name) == 0)
            return nodes[i]->right->token->content;
        i++;
    }
    return ("");
}

char *expand(char *content)
{
    char    *res;
    char    *var;
    char    *to_add;
    int     i;
    int     start;
    int     squote;
    int     dquote;

    res = NULL;
    i = 0;
    squote = 0;
    dquote = 0;
    while(content && content[i])
    {
        if(content[i] == '\'' && !dquote)
        {
            squote = !squote;
            i++;
        }
        else if(content[i] == '"' && !squote)
        {
            dquote = !dquote;
            i++;
        }
        else if(content[i] == '$')
        {
            if(dquote || !squote)
            {
                i++;
                start = i;
                while(content[i] && !ft_strchr(" \"\'$", content[i]))
                    i++;
                var = ft_calloc(i - start + 1, sizeof(char));
                ft_memcpy(var, content + start, i - start);
                to_add = get_var(var);
                res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + ft_strlen(to_add) + 2, sizeof(char));
                ft_strncpy(res + ft_strlen(res), to_add, ft_strlen(to_add));
            }
            else if(squote)
            {
                res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2, sizeof(char));
                res[ft_strlen(res)] = content[i++];
            }
        }
        else
        {
            res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2, sizeof(char));
            res[ft_strlen(res)] = content[i++];
        }
    }
    return res;
}


// built ins
void	cd_func(char **arguments)
{
	// change PWD envirement variable
	if (arguments[0])
		chdir(arguments[0]);
	else
		chdir(get_env("HOME")); // to check
}

void	echo_func(char **arguments)
{
	int	print_new_line;
	int	i;
    int j;

    i = 0;
    print_new_line = 1;
	while(arguments && ft_strncmp(arguments[i], "-n", ft_strlen("-n")) == 0)
	{
        j = 1;
        while(arguments[i][j] == 'n')
            j++;
        if(ft_isspace(arguments[i][j]))
        {
    		print_new_line = 0;
	    	i++;
        }
        else
        {
            print_new_line = 1;
            i = 0;
            break;
        }
	}
	while (arguments && arguments[i])
	{
		write(OUT, arguments[i], ft_strlen(arguments[i]));
		write(OUT, " ", ft_strlen(" "));
		i++;
	}
	if (print_new_line)
		write(OUT, "\n", 1);
}

void	env_func(void)
{
	int		i;
	Node	**envirement;

	envirement = (Node **)Global.envirement.pointers;
	i = 0;
	while (envirement[i])
	{
		if (envirement[i]->left->token->content && envirement[i]->right->token->content)
		{	
			printf("%s=%s\n", envirement[i]->left->token->content,
				envirement[i]->right->token->content);
		}
		i++;
	}
}

void	exit_func(char **arguments)
{
    // check lot of shit here
    // printf("Exit function received \n");
	ft_exit(ft_atoi(arguments[0]) % 256);
}

void new_envirement_variable(char **arguments)
{
	Node *node;
	Node **envirement;
	char *name;
    char *assign;
    char *append;
    char *value;
	int i;
	int k;

    i = 0;
    while(arguments[i])
    {
        printf("-> %s\n", arguments[i]);
        i++;
    }

	i = 0;
	while (arguments && arguments[i])
	{
        name = NULL;
        assign = NULL;
        append = NULL;
        if(!ft_isalpha(arguments[i][0]))
        {
            printf("minishell: export: %s not a valid identifier\n", arguments[i]);
            return;
        }
        if(ft_strnstr(arguments[i], "+="))
        {
            name = ft_strdup(arguments[i], ft_strnstr(arguments[i], "+=") - arguments[i]);
            append = ft_strdup(ft_strnstr(arguments[i], "+=") + ft_strlen("+="), ft_strlen(arguments[i]));
            // printf("append '%s' to '%s'\n", value, name);
        }
        else if(ft_strchr(arguments[i], '='))
        {
            name = ft_strdup(arguments[i], ft_strchr(arguments[i], '=') - arguments[i]);
            assign = ft_strdup(ft_strchr(arguments[i], '=') + 1, ft_strlen(arguments[i]));
            // printf("assign '%s' to '%s'\n", value, name);
        }
        else
            name = arguments[i];
		envirement = (Node **)Global.envirement.pointers;
        k = 0;
		while (envirement && envirement[k])
		{
		 	if (ft_strcmp(envirement[k]->left->token->content, name) == 0)
		 	{
                if(assign)
    				envirement[k]->right->token->content = expand(assign);
                else if(append)
                    envirement[k]->right->token->content = strjoin(envirement[k]->right->token->content, expand(append), NULL);
				break;
			}	
			k++;
		}
		if (envirement[k] == NULL)
		{
            value = NULL;
            if(assign)
                value = expand(assign);
            if(append)
                value = expand(append);
			node = new_node(new_token(assign_, NULL, 0));
            node->left = new_node(new_token(identifier_, name, ft_strlen(name)));
            node->right = new_node(new_token(identifier_, value, ft_strlen(value)));
			add_pointer(&Global.envirement, node);
		}
		i++;
	}
}

void export_func(char **arguments)
{
	Node **envirement;
	int i;

	if (arguments[0])
		new_envirement_variable(arguments);
	else
	{
		i = 0;
		envirement = (Node **)Global.envirement.pointers;
		while (envirement && envirement[i])
		{
			if (envirement[i]->left->token->content)
			{
				if(envirement[i]->right->token->content)
					printf("declare -x %s=\"%s\"\n", envirement[i]->left->token->content, envirement[i]->right->token->content);
				else
					printf("declare -x %s\n", envirement[i]->left->token->content);
			}
			i++;
		}
	}
}

void	pwd_func(void)
{
	char	*new;

    new = getcwd(NULL, 0);
    printf("%s\n", new);
    free(new);
}

void	unset_func(char **argument)
{
	int		i;
	Node	**envirement;
	char	*value;

	envirement = (Node **)Global.envirement.pointers;
	i = 0;
	while (envirement && envirement[i])
	{
		value = envirement[i]->left->token->content;
		if (value && ft_strcmp(value, argument[0]) == 0)
		{
			envirement[i]->left->token->content = NULL;
			envirement[i]->right->token->content = NULL;
			break ;
		}
		i++;
	}
}

// evaluate
Value *evaluate(Node *node, File *input, File *output)
{
    Type    type;
    Value   *value;
    Token   **tokens;
    char    *argument;
    char    **arguments;
    int     len;
    int     pos;
    int     i;
    char    **built_ins;

    printf("Evaluate '%s' has type: %s\n", node->token->content, type_to_string(node->token->type));
    argument = NULL;
    arguments = NULL;
    value = NULL;
    len = 0;
    switch(node->token->type)
    {
        case identifier_:
        case redir_input:
        case redir_output:
        case append_:
        case heredoc_:
        {
            // token = node->token;
            pos = node->token->start;
            tokens = (Token**)Global.tokens.pointers;
            while(pos < node->token->len)
            {
                printf("tokens[pos]: %s\n", tokens[pos]->content);
                if(tokens[pos]->type == identifier_)
                {
                    arguments = ft_realloc(arguments, len, len + 2, sizeof(char*));
                    arguments[len] = expand(tokens[pos]->content);
                    arguments[++len] = NULL;
                }
                else if(tokens[pos]->type == star_)
                {
                    printf("handle star\n");
                    exit(1);
                }
                else if(is_redirection(tokens[pos]->type))
                {
                    printf("is redirection\n");
                    if(tokens[pos]->type == redir_input)
                    {
                        pos++;
                        input = new_file(expand(tokens[pos]->content), NOT_OPENED, redir_input);
                        printf("open '%s' type '%s'\n", input->name, type_to_string(tokens[pos - 1]->type));
                    }
                    if(tokens[pos]->type == heredoc_)
                    {
                        input = new_file(expand(tokens[pos]->content), NOT_OPENED, redir_input);
                        printf("open '%s' type '%s'\n", input->name, type_to_string(tokens[pos]->type));
                        pos++;
                    }
                    if(tokens[pos]->type == redir_output || tokens[pos]->type == append_)
                    {
                        pos++;
                        output = new_file(expand(tokens[pos]->content), NOT_OPENED, redir_output);
                        printf("open '%s' type '%s'\n", output->name, type_to_string(tokens[pos - 1]->type));
                    }
                }
                pos++;
            }
            if(arguments && arguments[0]) // check if "&& arguemnts[0]" should be added
            {
                if (ft_strcmp(arguments[0], "echo") == 0)
                    echo_func(&arguments[1]);
                else if (ft_strcmp(arguments[0], "cd") == 0)
                    cd_func(&arguments[1]);
                else if (ft_strcmp(arguments[0], "pwd") == 0)
                    pwd_func();
                else if (ft_strcmp(arguments[0], "export") == 0)
                    export_func(&arguments[1]);
                else if (ft_strcmp(arguments[0], "unset") == 0)
                    unset_func(&arguments[1]);
                else if (ft_strcmp(arguments[0], "env") == 0)
                    env_func();
                else if (ft_strcmp(arguments[0], "exit") == 0)
                    exit_func(&arguments[1]);
                else 
                {
                    argument = get_command_path(arguments[0]);
                    if(new_child_process() == 0)
                    {
                        check_redirection(input, output);
                        if (execve(argument, arguments, get_envirement()) < 0)
                        {
                            if (ft_strchr(argument, '/'))
                                printf("minishell: '%s' no such file or directory\n", argument);
                            else
                                printf("minishell: '%s' command not found\n", argument);
                            ft_exit(COMMAND_NOT_FOUND);
                        }
                    }
                }
            }
            return value;
        }
        default:
        {
            printf("Unkown type in evaluate\n");
            break;
        }
    }
    return NULL;
}

int	get_last_exit_code(void)
{
	int			*pids;
	int			status;
	static int	res;

	pids = Global.pids.integers;
	status = 0;
	if (Global.pids.pos > 0)
	{
		waitpid(pids[--Global.pids.pos], &status, 0);
		res = WEXITSTATUS(status);
	}
	return (res);
}

int main(int argc, char **argv, char **envp)
{

    char    *line;
    File    *input;
    File    *output;
    Node    *node;
    Token   **tokens;
    struct termios term;

    // set charaters for signals
    tcgetattr(0, &term);
	term.c_cc[VQUIT] = _POSIX_VDISABLE;
	term.c_lflag &= ~ECHOCTL; // to search for after
	tcsetattr(0, TCSANOW, &term);

    init_global(envp);
    build_envirement(envp);
    while(true)
    {
        // init
        new_list(&Global.tokens, sizeof(Token *), pointers_);
        new_list(&Global.pids, sizeof(int), integers_);
        new_list(&Global.fds, sizeof(int), integers_);
        Global.tokens.pos = 0;
        // read input
        input = new_file(NULL, IN, 0);
		output = new_file(NULL, OUT, 0);
        line = readline("minishell $> ");
        if(line == NULL)
            break;
        add_history(line);
        build_tokens(line);
        Global.tokens.pos = 0;
        tokens = (Token**)Global.tokens.pointers;
        while (tokens[Global.tokens.pos] && tokens[Global.tokens.pos]->type != end_)
        {
            node = expr_node();
            if (node == NULL)
                break;
            evaluate(node, input, output);
        }
        free(line);
        while (Global.pids.pos > 0)
		    get_last_exit_code();
        clear_list(&Global.tokens);
        clear_list(&Global.pids);
        clear_list(&Global.fds);
    }
    while (Global.pids.pos > 0)
        get_last_exit_code();
    clear_list(&Global.tokens);
    clear_list(&Global.pids);
    clear_list(&Global.fds);
    ft_exit(SUCCESS);
}