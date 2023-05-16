#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// macros
#define in STDIN_FILENO
#define out STDOUT_FILENO
#define err STDERR_FILENO

// stupid implicent declaration error again
void ft_printf(int fd, char *fmt, ...);
void ft_putchar(int fd, int c);
void ft_putstr(int fd, char *str);
void *ft_calloc(size_t count, size_t size);

// typdefs
typedef enum Type Type;
typedef struct Token Token;
typedef struct Token Value;
typedef struct Node Node;

// Globals
char *text;
int txt_pos;

Token *tokens[1000]; // try using realloc
int tk_pos;
int exe_pos;
int print_tk;

Node *envirement[1000];
int env_pos;

char **PATH;

// file descriptors

int curr[2];
int next[2];
int fd[2];
int ps = 0;

// Envp
char **global_envp;



// types
enum Type
{
    skip_,
    space_, assign_, 
    // $, expanding will be handled in double_quotes_
    dollar_,
    // can be command, or file
    // keep string because $PATH $"PATH" has difrent meaning
    expand_, identifier_, string_, pid_,
    //      <               >          <<             >>         |
    redirect_input, redirect_output, heredoc_, append_to_file, pipe_, 
    // built in
    env_, echo_, cd_, pwd_, export_, unset_, exit_,
    end_,
};

struct {
    char* value;
    Type type;
} symbol_lexic[] = {
    {"<<", heredoc_}, {">>", append_to_file}, {"<", redirect_input},
    {">", redirect_output}, {"|", pipe_}, {"$", dollar_}, {0 , 0}
    // keep it splited because this case my cause problems $"PATH" vs $PATH vs $$
};

struct {
    char* value;
    Type type;
} alpha_lexic[] = {
    {"echo", echo_}, {"cd", cd_}, {"pwd", pwd_}, {"export", export_},
    {"unset", unset_}, {"env", env_}, {"exit", exit_}, {0 , 0}
};

char *type_to_string(Type type)
{
    struct {
        char *value;
        Type type;
    } lexic[] = {
        {"SPACE", space_}, {"ASSIGN", assign_}, {"EXPAND", expand_}, {"DOLLAR", dollar_},{"STRING", string_},
        {"IDENTIFIER", identifier_}, {"INPUT REDIR", redirect_input}, {"OUTPUT REDIR", redirect_output},
        {"HEREDOC", heredoc_}, {"APPEND_OUTPUT", append_to_file}, {"PIPE", pipe_}, {"ENV", env_}, {"ECHO", echo_}, 
        {"CD", cd_}, {"PWD", pwd_},  {"EXPORT", export_}, {"UNSET", unset_}, {"EXIT", exit_}, {"END", end_}, {0, 0}
    };
    for(int i = 0; lexic[i].value; i++)
    {
        if(lexic[i].type == type)
            return lexic[i].value;
    }
    return NULL;
}

// Tokens
struct Token
{
    Type type;
    char *content;

    int joined_len;
    int arguments_len;
    pid_t process_id;
};

Token *new_token(Type type, int start, int end)
{
    Token *new = ft_calloc(1, sizeof(Token));
    new->type = type;
    if(print_tk)
        ft_printf(out, "new token with type %12t ", new->type); 
    if(start < end)
    {
        if(type == identifier_ || type == string_ || type == pipe_ || type == space_)
        {
            new->content = ft_calloc(end - start + 1, sizeof(char));
            int i = 0;
            while(start + i < end)
            {
                new->content[i] = text[start + i];
                i++;
            }
            if(print_tk)
                ft_printf(out, "has value: '%k'", new); // to be checked
        }
    }
    if(print_tk)
        ft_printf(out, "\n");
    tokens[tk_pos] = new;
    tk_pos++;
    return new;
};

// Nodes
struct Node
{
    Node *left;
    Node *right;
    Token *token;
};

Node *new_node(Token *token)
{
    Node *new = ft_calloc(1, sizeof(Node));
    new->token = token;
    // ft_printf(out, "new  node with type %15t has token: '%k'\n", token->type, new->token);
    return new;
}

// char methods
int ft_isnum(int c) { return (c >= '0' && c <= '9'); }
int ft_isupper(int c) { return (c >= 'A' && c <= 'Z'); }
int ft_islower(int c) { return (c >= 'a' && c <= 'z'); }
int ft_isalpha(int c) { return (ft_islower(c) || ft_isupper(c)); }
int ft_isalphanum(int c) { return (ft_isalpha(c) || ft_isnum(c)); }

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
	char	*pointer1;
	char	*pointer2;

	if (!destination)
		return (source);
	if (!source)
		return (destination);
	pointer1 = (char *)destination;
	pointer2 = (char *)source;
	i = 0;
	while (i < len)
	{
		pointer1[i] = pointer2[i];
		i++;
	}
	return (destination);
}

// memory allocation
void    *ft_calloc(size_t count, size_t size)
{
	void			*new;
	unsigned char	*pointer;
	size_t			i;

	new = (void *)malloc(count * size);
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
void *ft_realloc(void *pointer, size_t old_size ,size_t new_size)
{
    if (pointer == NULL)
        return ft_calloc(1, new_size);

    void* new = ft_calloc(1, new_size);
    ft_memcpy(new, pointer, old_size);
    free(pointer);
    return new;
}

// string methods
int     ft_strlen(char *string){ int i = 0; while (string && string[i]) i++; return i;}
void    ft_strncpy(char *destination, char *source, int len)
{
    if (destination == NULL || source == NULL)
        ft_printf(err, "receive NULL in strncpy\n");
    int dest_len = ft_strlen(destination);
    int i = 0;
    while (source[i] && i < len)
    {
        destination[dest_len + i] = source[i];
        i++;
    }
}
void    ft_strcpy(char *destination, char *source)
{
    if (destination == NULL || source == NULL)
        ft_printf(err, "receive NULL in strcpy\n");
    int dest_len = ft_strlen(destination);
    int i = 0;
    while (source[i])
    {
        destination[dest_len + i] = source[i];
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
char **split(char *str, char *spliter)
{
    int i = 0;
    int j = 0;
    char **res = NULL;
    int start = 0;
    if(ft_strncmp(str + i, spliter, ft_strlen(spliter)) == 0)
        i += ft_strlen(spliter);
    start = i;
    while(str && str[i])
    {
        while(ft_strncmp(str + i, spliter, ft_strlen(spliter)) == 0)
            i += ft_strlen(spliter);
        start = i;
        while(str[i] && ft_strncmp(str + i, spliter, ft_strlen(spliter)) != 0)
            i++;
        if(res == NULL)
            res = ft_calloc(j + 2, sizeof(char*));
        else
            res = ft_realloc(res, (j ) * sizeof(char*), ( j + 3 ) * sizeof(char*));
        res[j] = ft_calloc(i - start + 1, sizeof(char));
        ft_strncpy(res[j], str + start, i - start);
        j++;
        res[j] = NULL;
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

// readline
char *ft_readline(int fd)
{
    char *res = NULL;
    char *c = ft_calloc(2, sizeof(char));

    while (1)
    {
        int n = read(fd, c, sizeof(char));
        if (n <= 0)
            break;
        if (c[0] == '\n' || c[0] == '\0')
            break;
        if(res == NULL)
            res = ft_calloc(ft_strlen(res) + 2, sizeof(char));
        else
            res = ft_realloc(res, ft_strlen(res), (ft_strlen(res) + 3) * sizeof(char));
        res[ft_strlen(res)] = c[0];
    }
    return res;
}

// printf
void ft_putchar(int fd, int c)
{
    write(fd, &c, sizeof(char));
}
void ft_putstr(int fd, char *str)
{
    write(fd, str, ft_strlen(str));
}
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
void ft_printf(int fd, char *fmt, ...)
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
                Token *variable = va_arg(ap, Token *);
                if (variable)
                {
                    switch (variable->type)
                    {
                    case assign_:
                    case string_:
                    case identifier_:
                    case pipe_:
                    case space_:
                    case end_:
                    case dollar_:
                        print_space(fd, space - ft_strlen(variable->content));
                        ft_putstr(fd, variable->content);
                        break;
                    default:
                        ft_putstr(err, "Unkown given token type: ");
                        ft_putnbr(err, variable->type);
                        ft_putstr(err, "\n");
                        exit(0);
                        break;
                    }
                }
                else
                    ft_putstr(fd, "(null token)");
            }
            if (fmt[i] == 't')
            {
                char* type = type_to_string(va_arg(ap, Type));
                print_space(fd, space - ft_strlen(type));
                ft_putstr(fd, type);
            }
            if (fmt[i] == 'd')
            {
                int num = va_arg(ap, int);
                ft_putnbr(fd, (long)num);
            }
            if (fmt[i] == 'c')
            {
                int c = va_arg(ap, int);
                ft_putchar(fd, c);
            }
            if (fmt[i] == 's')
            {
                char *str = va_arg(ap, char *);
                ft_putstr(fd, str);
            }
        }
        else
            ft_putchar(fd, fmt[i]);
        i++;
    }
    va_end(ap);
    if (fd == err)
        exit(1);
}

// Tokenize
void error(char c)
{
    ft_printf(out,"\n%s\n", text);
    print_space(out ,txt_pos);
    ft_printf(out, "^\n");
    ft_printf(err, "Expected '%c'\n", c);
}

// build env nodes 
void build_env_tokens(char **envp)
{
    int i = 0;
    while(envp && envp[i])
    {
        char **array = split(envp[i], "=");
        envirement[env_pos] = new_node(new_token(assign_, 0, 0));
        envirement[env_pos]->left = new_node(new_token(identifier_, 0, 0));
        envirement[env_pos]->right = new_node(new_token(string_, 0, 0));
        envirement[env_pos]->left->token->content = array[0];
        envirement[env_pos]->right->token->content = array[1];
        ft_printf(out, "+ new node with type: %t \n", envirement[env_pos]->token->type);
        ft_printf(out, "            has left: %k\n", envirement[env_pos]->left->token);
        ft_printf(out, "           has right: %k\n\n", envirement[env_pos]->right->token);
        env_pos++;
        i++;
    }
}

void build_tokens()
{
    Token *token;
    int txt_pos = 0;
    int start = 0;

    while(text && text[txt_pos])
    {
        if(text[txt_pos] == '\0')
            break;
        // skip spaces
        if(text[txt_pos] == ' ')
        {
            start = txt_pos;
            while(text[txt_pos] == ' ')
                txt_pos++;
            new_token(space_, start, txt_pos);
            continue;
        }
        // double quotes command
        if(text[txt_pos] == '"')
        {            
            txt_pos++; // skip left "
            start = txt_pos;
            while(text[txt_pos] && text[txt_pos] != '"')
            {
                if(text[txt_pos] == '$')
                {
                    new_token(string_, start, txt_pos);
                    txt_pos++; // to skip $
                    start = txt_pos - 1;
                    new_token(dollar_, start, txt_pos);
                    start = txt_pos;
                    while(text[txt_pos] && text[txt_pos] != ' ' && text[txt_pos] != '"')
                        txt_pos++;
                    new_token(string_, start, txt_pos);

                    start = txt_pos; // for the next string
                    // don't increment txt_pos here to skip second " 
                    // because it may cause problems when exiting the while loop
                }
                else
                    txt_pos++;
            }
            if(text[txt_pos] != '"')
                error('"');
            new_token(string_, start, txt_pos);
            txt_pos++; // skip right "
            continue;
        }
        // single quotes command
        // treated as string
        if(text[txt_pos] == '\'')
        {
            txt_pos++; // skip left '
            start = txt_pos;
            while(text[txt_pos] && text[txt_pos] != '\'')
                txt_pos++;
            if(text[txt_pos] != '\'')
                error('\'');
            new_token(string_, start, txt_pos);
            txt_pos++; // skip right '
            continue;
        }
        // heredoc, redirection, pipe, dollar
        int i = 0;
        Type type = 0;
        while(symbol_lexic[i].value)
        {
            if(ft_strncmp(symbol_lexic[i].value, &text[txt_pos], ft_strlen(symbol_lexic[i].value)) == 0)
            {
                start = txt_pos;
                type = symbol_lexic[i].type;
                txt_pos += ft_strlen(symbol_lexic[i].value);
                break;
            }
            i++;
        }
        if(type)
        {
            new_token(type, start, txt_pos);
            // I added start and txt_pos here, to use them in case $$, second dollar will be considered as identifier
            // handle dollar in evaluation
            continue;
        }
        if (text[txt_pos] && !ft_strchr("\'\"$ ", text[txt_pos]))
        {
            start = txt_pos;
            while(text[txt_pos] && !ft_strchr("\'\"$ ", text[txt_pos]))
                txt_pos++;
            new_token(identifier_, start, txt_pos);
            continue;
        }
    }
    new_token(end_, 0, 0);
}

// build nodes
void skip(Type type)
{
    if(tokens[exe_pos]->type != type)
        ft_printf(err, "Error Expected '%s'\n", type_to_string(type));
    exe_pos++;
}

bool check(Type type, Type array[], int len)
{
    int i = 0;
    while(i < len)
    {
        if(array[i] == type)
            return true;
        i++;
    }
    return false;
}

Node *expr();        // expression
Node *assign();      // assignement
Node *pipe_node();   // |
Node *redirection(); // < > << >>
Node *arguments();   // has spaces
Node *join();        // no spaces
Node *prime();       // file, command, path, built in commands: echo, cd, pwd, export, unset, env, exit

Node *expr()
{
    ft_printf(out, "call expr\n");
    return assign();
}

// =
Node *assign()
{
    ft_printf(out, "call assign\n");
    Node *left = pipe_node();
    while (tokens[exe_pos]->type == assign_)
    {
        Node *node = new_node(tokens[exe_pos]);
        skip(assign_);
        node->left = left;
        node->right = pipe_node();
        left = node;
    }
    return left;
}

// |
Node *pipe_node()
{
    ft_printf(out, "call pipe\n");
    int i = 0;
    Node *left = redirection();
    if(tokens[exe_pos]->type == space_)
        skip(space_); // to be verified
    if (tokens[exe_pos]->type == pipe_)
    {
#if 1 
        // build a solution
        ft_printf(out, "find pipe %d\n", i);
        i++;
        Node *node = new_node(tokens[exe_pos]);
        skip(pipe_);
        node->left = left;
        
        if(tokens[exe_pos]->type == space_)
            skip(space_); // to be verified
        while (tokens[exe_pos]->type == pipe_)
        {
            ft_printf(out, "find pipe %d\n", i);
            i++;
            // make some changes here
            node->right = new_node(tokens[exe_pos]);
            skip(pipe_);
            node->left = redirection();
            node = node->right;
            if(tokens[exe_pos]->type == space_)
                skip(space_); // to be verified
        }
        left = node;
#endif 
    }
    return left;
}

// < > << >>
Node *redirection()
{
    ft_printf(out, "call redirect\n");
    Node *left = NULL;
    if(tokens[exe_pos]->type == space_)
        skip(space_); // to be verified
    
    Type array[] = {heredoc_, redirect_input, redirect_output, append_to_file};
    if (check(tokens[exe_pos]->type, array, sizeof(array) / sizeof(Type)))
    {
        // int limit = 0;
        while (check(tokens[exe_pos]->type, array, sizeof(array) / sizeof(Type)))
        {
            Node *node = new_node(tokens[exe_pos]);
            skip(tokens[exe_pos]->type);
            node->left = left;
            node->right = arguments();
            left = node;
            if(tokens[exe_pos]->type == space_)
                skip(space_); // to be verified
        }
        return left;
    }
    return arguments();
}

// splited by space
Node *arguments()
{
    ft_printf(out, "call arguments\n");
    Node *left = join();
    Node *node = left;

    Type array[] = {identifier_, string_, dollar_};
    while (tokens[exe_pos]->type == space_ && check(tokens[exe_pos + 1]->type, array, sizeof(array) / sizeof(Type)))
    {
        skip(space_); // to be verified
        node->left = join();
        node = node->left;
    }
    return left;
}

// joined commands
Node *join()
{
    ft_printf(out, "call join\n");
    Node *left = prime();
    Node *node = left;

    Type array[] = {identifier_, string_, dollar_};
    while (check(tokens[exe_pos]->type, array, sizeof(array) / sizeof(Type)))
    {
        node->right = prime();
        node = node->right;
    }
    return left;
}

// primary
Node *prime()
{
    ft_printf(out, "call prime\n");
    Node *left = NULL;
    if(tokens[exe_pos]->type == space_)
        skip(space_); // to be verified
    
    Type array[] = {identifier_, string_, dollar_};
    if (check(tokens[exe_pos]->type, array, sizeof(array) / sizeof(Type)))
    {
        Node *node = new_node(tokens[exe_pos]);
        skip(tokens[exe_pos]->type);
        return node;
    }
    
    ft_printf(err,"Error in prime\n");
    return left;
}

// Execution
char *get_command(char *cmd)
{
    if(cmd == NULL || ft_strchr(cmd, '/'))
        return cmd;
    int i = 0;
    while(PATH && PATH[i])
    {
        char *res = strjoin(PATH[i], "/", cmd);
        if (access(res, F_OK) == 0 && access(res, X_OK) == 0)
            return (res);
        i++;
    }
    return cmd;
}

// built in functions
void echo_func(char **arguments)
{
    // evaluate them after  , arguemnts must be an array of tokens in stead of char*
    // arguments might be expandables
    int i = 0;
    while(arguments && arguments[i])
    {
        ft_printf(out, "%s ", arguments[i]);
        i++;
    }
}
void cd_func(char **arguments)
{
    /// evaluate them after  , arguemnts must be an array of tokens in stead of char*
    // arguments might be expandables
    chdir(arguments[0]);
}
void pwd_func(char **arguments)
{
    ft_printf(out, "%s\n", getcwd(NULL, 0));
}
void export_func(char **arguments)
{
    int i = 0;
    while(envirement[i])
    {
        printf("declare -x %s=\"%s\"\n", envirement[i]->left->token->content, envirement[i]->right->token->content);
        i++;
    }
}
void unset_func(char **argument)
{
    // to be handled after
    return;
}
void env_func(char **arguments)
{
    int i = 0;
    while(envirement[i])
    {
        // print only those who have right token
        printf("%s=%s\n", envirement[i]->left->token->content, envirement[i]->right->token->content);
        i++;
    }
}
void exit_func(Node *node)
{
    exit(0); // verify exit code after
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

char *expand(Token *token)
{
    // if there isn't token return $
    // check token, only idenitifiers get expanded
    return NULL;
}

// evaluate
Value *evaluate(Node *node)
{
    ft_printf(out, "Evaluate %k\n", node->token);
    switch(node->token->type)
    {
        case identifier_:
        case string_:
        {
            Value *curr = node->token;
            Node *tmp = node->right;

            char *command = curr->content;
            char **arguments = NULL;
            char *to_add = NULL;

            // join
            while(tmp)
            {
                // evaluate if $
                if(tmp->token->type == identifier_)
                    to_add = tmp->token->content;
                else if(tmp->token->type == string_)
                    to_add = tmp->token->content;
                else if(tmp->token->type == dollar_)
                {
                    // handle this case $PATH"HH"
                    tmp = tmp->right;
                    to_add = expand(tmp->token);
                }
                command = ft_realloc(command, ft_strlen(command), ft_strlen(command) + ft_strlen(to_add) + 1);
                ft_strcpy(command + ft_strlen(command), to_add);
                tmp = tmp->right;
            }
            
            // arguments
            tmp = node->left;
            int len = 0;
            arguments = ft_realloc(arguments, len * sizeof(char *), (len + 2)*sizeof(char*));
            arguments[len] = command;
            len++;
            while(tmp)
            {
                // evaluate if $
                if(tmp->token->type == identifier_)
                    to_add = tmp->token->content;
                else if(tmp->token->type == string_)
                    to_add = tmp->token->content;
                else if(tmp->token->type == dollar_)
                {
                    // handle this case $PATH"HH"
                    tmp = tmp->right;
                    to_add = expand(tmp->token);
                }
                arguments = ft_realloc(arguments, len * sizeof(char *), (len + 2)*sizeof(char*));
                arguments[len] = to_add;
                len++;
                arguments[len] = NULL;
                tmp = tmp->left;
            }

            pid_t process_id = 0;
            // built in function
            // may be you may need to fork here maybeee
            void (*func)(char**) = built_in(command);
            if(func)
            {
                ft_printf(out, "Built in function with name '%s'\n", command);
                if(len)
                {
                    ft_printf(out, "    has current arguments : ");
                    int i = 0;
                    while(arguments[i])
                    {
                        ft_printf(out, "'%s' ", arguments[i]);
                        i++;
                    }
                }
                ft_printf(out, "\n");
                func(&arguments[1]);
            }
            else
            {
                char *command_to_execute = get_command(command);
                ft_printf(out, "command is '%s'\n", command_to_execute);
                if(len)
                {
                    ft_printf(out, "    has current arguments : ");
                    int i = 0;
                    while(arguments[i])
                    {
                        ft_printf(out, "'%s' ", arguments[i]);
                        i++;
                    }
                    ft_printf(out, "\n");
                }
                process_id = fork();
                // check if fork fail
                if(process_id == 0)
                {
                    ft_printf(out, "child process run %s has input %d and output %d\n",arguments[0], fd[0], fd[1]);
                    dup2(fd[0], in);
                    dup2(fd[1], out);
                    int n = 3;
                    while(n < 7)
                    {
#if 0
                        if(n != fd[0] && n != fd[1]) close(n);
#else
                        close(n);
#endif
                        n++;
                    }
                    if(execve(command_to_execute, arguments, global_envp) < 0)
                        ft_printf(err, "Execve failed\n");
                }
                else if(process_id > 0)
                {
                    ft_printf(out, "parent process\n");
                    // if(fd[0] != in)
                    // close(fd[0]);
                    // if(fd[1] != out)
                    // close(fd[1]);
                }
                else
                    ft_printf(err, "fork did failed\n");
            }

            Value *res = new_token(pid_,0 ,0);
            ft_printf(out, "\n");
            res->process_id = process_id;
            return res;
        }
        case pipe_:
        {
            /*
                pipe has left command and right command
                or left pipe and right command
            */
            Node *tmp = node;
            int i = 0;
            while(tmp)
            {
                ft_printf(out, "\nopen pipe %d\n", i);
                if(i % 2 == 0)
                {
                    if(i == 0) fd[0] = in;
                    else fd[0] = curr[0];

                    if(tmp->left == NULL) fd[1] = out;
                    else fd[1] = next[1];
                }
                else
                {
                    fd[0] = next[0];

                    if(tmp == NULL) fd[1] = out;
                    else fd[1] = curr[1];
                }
                // check if left type is pipe or command
                // ps++;
                Value *left = evaluate(tmp->left); // when evaluating command fork and execute it, return its pid
                tmp = tmp->right;
                i++;
            }

#if 0
            Value *left = evaluate(node->left); // when evaluating command fork and execute it, return its pid
            
            Value *right = NULL;
            if(node->right)
                right = evaluate(node->right);

            if(left->type != pid_)
                ft_printf(err, "Error 1 in pipe_ expected pid_\n");
            if(right->type != pid_)
                ft_printf(err, "Error 2 in pipe_ expected pid_\n");

            close(fd[0]);
            close(fd[1]);

            waitpid(left->process_id, NULL, 0);
            if(node->right)
                waitpid(right->process_id, NULL, 0);
#endif        
            while (i > 0) // to be removed after because you need the pid of child processes
            {
                wait(NULL);
                printf("wait child %d\n", i);
                i--;
            }    
            return NULL;
        }
        default:
            ft_printf(err, "Unknown token type in evaluate %t\n", node->token->type);
    }
    return NULL;
}

int main(int argc, char **argv, char **envp)
{
    global_envp = envp;
    for(int i = 0; envp && envp[i] ;i++)
    {
        if(ft_strncmp("PATH", envp[i], ft_strlen("PATH")) == 0)
        {
            PATH = split(envp[i], ":");
            break;
        }
    }
    print_tk = 0;
    build_env_tokens(envp);
    ft_printf(out, "\n");
    
    // create pipes
    if(pipe(curr) < 0)
    {
        printf("Error opening pipe 1\n");
        exit(0);
    }
    if(pipe(next) < 0)
    {
        printf("Error opening pipe 2\n");
        exit(0);
    }

    while(1)
    {
        text = ft_readline(out);
        tk_pos = 0;
        // build tokens
        print_tk = 1;
        build_tokens();
        ft_printf(out, "\n");
        // evaluate
        tk_pos++;
        
        
        while(tokens[exe_pos]->type != end_)
        {
            // fd[0] = in;
            // fd[1] = out;
            ft_printf(out, "Call evaluate\n");
            evaluate(expr());
           
        }

        txt_pos = 0;
        exe_pos = 0;
        ft_memset(tokens, 0, 1000*sizeof(Token *));
    }
}