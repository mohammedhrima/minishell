#include "header.h"

char *type_to_string(t_type type)
{
    char **values;
    t_type *types;
    int i;

    values = (char*[]){"=", "identifier", "<", ">", "<<",  ">>", "|", "env", "echo", "&&", "||", 
    "cd", "pwd", "export", "unset", "exit", "end",  "*", "(", ")", NULL};
    types = (t_type[]){assign_, identifier_, redir_input, redir_output, heredoc_, append_, 
    pipe_, env_, echo_, and_, or_, cd_, pwd_, export_, unset_, exit_, end_, star_, lparent_, rparent_, 0};
    i = 0;
    while(types[i])
    {
        if(types[i] == type)
            return values[i];
        i++;
    }
    return NULL;
}

// exit function
int ft_exit(int code)
{
    int i;

    i = 0;
    while(i < global.fds.pos)
    {
        if(global.fds.integers[i] != OUT && global.fds.integers[i] != IN)
            close(global.fds.integers[i]);
        i++;
    }
    i = 0;
    while(i < global.addresses.pos)
    {
        free(global.addresses.pointers[i]);
        i++;
    }
    // if(DEBUG == 0)
        // system("leaks bonus");
    // ft_printf(OUT, "call exit with code %d\n",code);
    exit(code);
    return code;
}

// char methods
int ft_isnum(int c) { return (c >= '0' && c <= '9'); }
int	ft_isspace(int c){ return (c == '\t' || c == '\n' || c == '\f' || c == '\r' || c == '\v' || c == ' ');}

// memes
void	ft_memset(void *pointer, int c, size_t len)
{
	size_t			i;
	unsigned char	*temporary;

	temporary = (unsigned char *)pointer;
	i = 0;
	while (temporary && i < len)
	{
		temporary[i] = c;
		i++;
	}
}
void	*ft_memcpy(void *destination, void *source, size_t len)
{
	size_t	i;
	unsigned char	*pointer1;
	unsigned char	*pointer2;

	if (!destination)
		return (source);
	if (!source)
		return (destination);
	pointer1 = (unsigned char *)destination;
	pointer2 = (unsigned char *)source;
	i = 0;
	while (i < len)
	{
		pointer1[i] = pointer2[i];
		i++;
	}
	return (destination);
}

// memory allocation
// this one add to addresses
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
void    *ft_calloc(size_t count, size_t size)
{
	void			*new;
	unsigned char	*pointer;
	size_t			i;

	new = (void *)malloc(count * size);
    add_to_addresses(new);
	if (!new)
		return (NULL);
	i = 0;
	pointer = (unsigned char *)new;
	while (i < count * size)
	{
		pointer[i] = 0;
		i++;
	}
	return (new);
}

void *ft_realloc(void *pointer, size_t oldsize, size_t newsize)
{
    void *new = ft_calloc(1, newsize);
    if(pointer)
        ft_memcpy(new, pointer, oldsize);
    return new;
}

// string methods
int     ft_strlen(char *string){ int i = 0; while (string && string[i]) i++; return i;}
void    ft_strncpy(char *destination, char *source, int len)
{
    int i = 0;
    while (source[i] && i < len)
    {
        destination[i] = source[i];
        i++;
    }
}
void    ft_strcpy(char *destination, char *source)
{
    int i = 0;
    while (source[i])
    {
        destination[i] = source[i];
        i++;
    }
}
char	*ft_strchr(char *string, int c)
{
	int	i;

	i = 0;
	if (!string)
		return (NULL);
	while (string[i])
	{
		if (string[i] == (char)c)
			return (string + i);
		i++;
	}
	if (c == 0 && string[i] == 0)
		return (string + i);
	return (NULL);
}
int     ft_strncmp(char *left, char *right, size_t len)
{
    size_t i = 0;
    while (i < len && left[i] && right[i] && left[i] == right[i])
        i++;
    if (i == len)
        return (0);
    return (left[i] - right[i]);
}
int     ft_strcmp(char *left, char *right)
{
    size_t i = 0;
    while (left[i] && right[i] && left[i] == right[i])
        i++;
    return (left[i] - right[i]);
}
char **split(char *string, char *spliter)
{
    int i = 0;
    int j = 0;
    char **res = NULL;
    int start = 0;
    if(ft_strncmp(string + i, spliter, ft_strlen(spliter)) == 0)
        i += ft_strlen(spliter);
    start = i;
    while(string && string[i])
    {
        while(ft_strncmp(string + i, spliter, ft_strlen(spliter)) == 0)
            i += ft_strlen(spliter);
        start = i;
        while(string[i] && ft_strncmp(string + i, spliter, ft_strlen(spliter)) != 0)
            i++;    
        if(res == NULL)
            res = ft_calloc(j + 2, sizeof(char*));
        else
            res = ft_realloc(res, j * sizeof(char*), ( j + 2 ) * sizeof(char*));
        res[j] = ft_calloc(i - start + 2, sizeof(char));
        ft_strncpy(res[j], string + start, i - start);
        j++;
        res[j] = NULL;
    }
    return res;
}
char **split_by_two(char *string, char c)
{
    int i = 0;
    char **res = ft_calloc(2, sizeof(char *));
    while(string && string[i])
    {
        if(string[i] == c)
        {
            res[0] = ft_calloc(i + 1, sizeof(char));
            ft_strncpy(res[0], string, i);
            i++;
            res[1] = ft_calloc(ft_strlen(string) - i + 1 , sizeof(char));
            ft_strncpy(res[1], string + i, ft_strlen(string) - i);
            break;
        }
        i++;
    }
    return res;
}
char *strjoin(char *string1, char *string2, char *string3)
{
    char *res = ft_calloc(ft_strlen(string1) + ft_strlen(string2) + ft_strlen(string3) + 1, sizeof(char));
    if (string1)
        ft_strncpy(res, string1, ft_strlen(string1));
    if (string2)
        ft_strncpy(res + ft_strlen(res), string2, ft_strlen(string2));
    if (string3)
        ft_strncpy(res + ft_strlen(res), string3, ft_strlen(string3));
    return res;
}


char	*ft_strdup(char *string)
{
	char	*pointer;
	int		i;

	if (!string)
		return (NULL);
	pointer = ft_calloc((ft_strlen(string) + 1), sizeof(char));
	if (!pointer)
		return (NULL);
	i = 0;
	while (string && string[i])
	{
		pointer[i] = string[i];
		i++;
	}
	pointer[i] = '\0';
	return (pointer);
}

// ft_itoa ft_atoi
char *ft_itoa(int num)
{
    if(num < 10)
    {
        char *res = ft_calloc(2, sizeof(char));
        res[0] = num + '0';
        return res;
    }
    char *left = ft_itoa(num / 10);
    left = ft_realloc(left, ft_strlen(left), ft_strlen(left) + 2);
    left[ft_strlen(left)] = num % 10 + '0';
    return left;
}
int ft_atoi(char *str) {
    int result = 0;
    int sign = 1;
    int i = 0;

    if(str == NULL)
        return 0;
    while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
        i++;

    if (str[i] == '+' || str[i] == '-') {
        sign = (str[i] == '-') ? -1 : 1;
        i++;
    }
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }
    return sign * result;
}

// printf
void ft_putchar(int fd, int c){ write(fd, &c, sizeof(char));}
void ft_putstr(int fd, char *str){ write(fd, str, ft_strlen(str));}
void ft_putnbr(int fd, long num)
{
    if (num < 0)
    {
        ft_putchar(fd, '-');
        num = -num;
    }
    if (num < 10) ft_putchar(fd, num + '0');
    else
    {
        ft_putnbr(fd, num / 10);
        ft_putnbr(fd, num % 10);
    }
}
void print_space(int fd, int len)
{
    int i = 0;
    while(i < len)
    {
        ft_putchar(fd, ' ');
        i++;
    }
}
#if 1
void ft_printf(int file_descriptor, char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    int i = 0;
    while (fmt && fmt[i])
    {
        if (fmt[i] == '%')
        {
            i++;
            int space = 0;
            if(ft_isnum(fmt[i]))
            {
                while(ft_isnum(fmt[i]))
                {
                    space = 10 * space + fmt[i] - '0';
                    i++;
                }
            }
            if (fmt[i] == 'k')
            {
                t_token *variable = va_arg(ap, t_token *);
                if (variable)
                {
                    switch (variable->type)
                    {
                    case assign_:
                    case identifier_:
                    case pipe_:
                    case end_:
                    case star_:
                    case redir_input:
                    case redir_output:
                    case heredoc_:
                    case append_:
                    case and_:
                    case or_:
                    case lparent_:
                    case rparent_:
                        print_space(file_descriptor, space - ft_strlen(variable->value));
                        ft_putstr(file_descriptor, variable->value);
                        break;
                    default:
                        ft_putstr(ERR, "Unkown given token type: ");
                        ft_putnbr(ERR, variable->type);
                        ft_putstr(ERR, "\n");
                        ft_exit(0);
                        break;
                    }
                }
                else
                    ft_putstr(file_descriptor, "(null token)");
            }
            if (fmt[i] == 't')
            {
                char* type = type_to_string(va_arg(ap, t_type));
                print_space(file_descriptor, space - ft_strlen(type));
                ft_putstr(file_descriptor, type);
            }
            if (fmt[i] == 'd')
            {
                int num = va_arg(ap, int);
                ft_putnbr(file_descriptor, (long)num);
            }
            if (fmt[i] == 'c')
            {
                int c = va_arg(ap, int);
                ft_putchar(file_descriptor, c);
            }
            if (fmt[i] == 's')
            {
                char *str = va_arg(ap, char *);
                ft_putstr(file_descriptor, str);
            }
        }
        else
            ft_putchar(file_descriptor, fmt[i]);
        i++;
    }
    va_end(ap);
}
#endif
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

    envirement = (t_node **)global.envirement.pointers, i = 0;
    while (envirement && envirement[i])
    {
        value = envirement[i]->left->token->value;
        if (value && ft_strcmp(value, argument[0]) == 0)
        {
            envirement[i]->left->token->value = NULL;
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
                        ft_exit(SUCCESS);
                }
            }
            else
            {
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
        else if (there_is_Redirection)
        {
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