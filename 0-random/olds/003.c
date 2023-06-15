#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>

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
// text
char *text;
int txt_pos;
// tokens
Token **tokens; // use realloc
int tk_pos;
int exe_pos;
// env
Node **envirement;
int env_pos;
// PATH
char **PATH;
// file descriptors;
int curr[2];
int next[2];
int fd[2];
// global env
char **global_env;
// process ids
int pids[200];
int pid_pos;

//multiple pipes
int pipes[100];
int pipe_pos;

// types
enum Type
{
    skip_, assign_,
    // $                   '' ""   
    // dollar_, 
    identifier_,
    //  <           >             <<       >>      |
    redir_input, redir_output, heredoc_, append_, pipe_,
    // built in
    env_, echo_, cd_, pwd_, export_, unset_, exit_,
    end_
};

struct {
    char *value;
    Type type;
} symbol_lexic[] = {
    {"<<", heredoc_}, {">>", append_}, {"<", redir_input},
    {">", redir_output}, {"|", pipe_}, {0 , 0}
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
        {"ASSIGN", assign_}, 
        {"IDENTIFIER", identifier_}, {"INPUT REDIR", redir_input}, {"OUTPUT REDIR", redir_output},
        {"HEREDOC", heredoc_}, {"APPEND_OUTPUT", append_}, {"PIPE", pipe_}, {"ENV", env_}, {"ECHO", echo_}, 
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

    // Token *array_head;
    int token_start;
    // int joined_len;
    int token_len;
    pid_t process_id;
};

Token *new_token(Type type, int start, int end)
{
    Token *new = ft_calloc(1, sizeof(Token));
    new->type = type;
    if(start < end)
    {
        new->content = ft_calloc(end - start + 1, sizeof(char));
        int i = 0;
        while(start + i < end)
        {
            new->content[i] = text[start + i];
            i++;
        }    
    }
    ft_printf(out, "new token with type %12t ", new->type);
    if(new->content)
        ft_printf(out, "has value: '%k'", new);
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

    // int token_start;
    // int array_len;
};

Node *new_node(Token *token)
{
    Node *new = ft_calloc(1, sizeof(Node));
    new->token = token;
    ft_printf(out, "new  node with type %15t has token: '%k'\n", token->type, new->token);
    return new;
}

// char methods
int ft_isnum(int c) { return (c >= '0' && c <= '9'); }
int ft_isupper(int c) { return (c >= 'A' && c <= 'Z'); }
int ft_islower(int c) { return (c >= 'a' && c <= 'z'); }
int ft_isalpha(int c) { return (ft_islower(c) || ft_isupper(c)); }
int ft_isalphanum(int c) { return (ft_isalpha(c) || ft_isnum(c)); }
int ft_isspace(int c){ return (c == '\n' || c == ' ' || c == '\v' || c == '\r' || c == '\t');}

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
            res = ft_realloc(res, (j) * sizeof(char*), ( j + 3 ) * sizeof(char*));
        res[j] = ft_calloc(i - start + 1, sizeof(char));
        ft_strncpy(res[j], string + start, i - start);
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

char *charjoin(char *string, char c)
{
    char *res = ft_calloc(ft_strlen(string) + 2, sizeof(char));
    if (string)
        ft_strncpy(res, string, ft_strlen(string));
    res[ft_strlen(res)] = c;
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
                    case identifier_:
                    case pipe_:
                    case end_:
                    // case dollar_:
                    case redir_input:
                    case redir_output:
                    case heredoc_:
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
        envirement[env_pos]->right = new_node(new_token(identifier_, 0, 0));
        envirement[env_pos]->left->token->content = array[0];
        envirement[env_pos]->right->token->content = array[1];
        ft_printf(out, "+ new node with type: %t \n", envirement[env_pos]->token->type);
        ft_printf(out, "           has  left: %k\n", envirement[env_pos]->left->token);
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
    int single_quotes = 0;
    int double_quotes = 0;

    while(text && text[txt_pos])
    {
        single_quotes = 0;
        double_quotes = 0;
        
        if(text[txt_pos] == '\0')
            break;
        // skip spaces
        if(text[txt_pos] == ' ')
        {
            start = txt_pos;
            while(text[txt_pos] == ' ')
                txt_pos++;
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
            continue;
        }
        start = txt_pos;
        while(text[txt_pos])
        {
            if(text[txt_pos] == '\'')
            {
                if(!double_quotes)
                    single_quotes = !single_quotes;
            }
            else if(text[txt_pos] == '"')
            {
                if(!single_quotes)
                    double_quotes = !double_quotes;
            }
            else
            {
                if(!single_quotes && !double_quotes && (ft_isspace(text[txt_pos]) || ft_strchr("|&<>", text[txt_pos])))
                    break;
            }
            txt_pos++;
        }
        new_token(identifier_, start, txt_pos);
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
Node *prime();       // file, command, argument, built in commands: echo, cd, pwd, export, unset, env, exit

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

Node *pipe_node()
{
    ft_printf(out, "call pipe\n");
    Node *left = prime();
    while(tokens[exe_pos]->type == pipe_)
    {
        static int i = 0;
        ft_printf(out, "find pipe %d\n", i++);
        Node *node = new_node(tokens[exe_pos]);
        skip(pipe_);
        node->left = left;
        node->right = prime();
        left = node;
    }
    return left;
}

Node *prime()
{
    ft_printf(out, "call prime: tokens[%d] has type %s\n", exe_pos, type_to_string(tokens[exe_pos]->type));
    // Node *left = NULL;
    
    Type array[] = {identifier_, redir_input, redir_output, heredoc_, append_};
    if (check(tokens[exe_pos]->type, array, sizeof(array) / sizeof(Type)))
    {
        // ft_printf(out, "\n");
        Node *node = new_node(tokens[exe_pos]);
        node->token->token_start = exe_pos;
        node->token->token_len = 0;
        // skip(tokens[exe_pos]->type);
        while(check(tokens[exe_pos]->type, array, sizeof(array) / sizeof(Type)))
        {
            node->token->token_len++;
            skip(tokens[exe_pos]->type);
        }
        return node;
    }
    ft_printf(err, "Error in prime, tokens[%d] has type %t\n", exe_pos ,tokens[exe_pos]->type);
    return NULL;
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
// expanding
char *get_var(char *name)
{
    int i = 0;
    // ft_printf(out, "Enter get_var, search for '%s'\n", name);
    while(envirement && envirement[i])
    {
        if(ft_strcmp(envirement[i]->left->token->content, name) == 0)
        {
            return envirement[i]->right->token->content;
        }
        i++;
    }
    return NULL;
}

char *expand(Token *token)
{
    int i = 0;
    int j = 0;
    char *res = NULL;
    int single_quotes = 0;
    int double_quotes = 0;

    while(token->content && token->content[i])
    {
        // ft_printf(out, "loop '%c'\n", token->content[i]);
        if(token->content[i] == '\'')
        {
            if(!double_quotes)
                single_quotes = !single_quotes;
            else
            {
                res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
                res[ft_strlen(res)] =  token->content[i];
            }
            i++;
        }
        else if(token->content[i] == '"')
        {
            if(!single_quotes)
                double_quotes = !double_quotes;
            else
            {
                res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
                res[ft_strlen(res)] =  token->content[i];
            }
            i++;
        }
        else if (token->content[i] == '$')
        {
            if(double_quotes || !single_quotes)
            {
                i++;
                int j = i;
                while (token->content[j] && !ft_strchr(" \"", token->content[j]))
                    j++;
                char *var = ft_calloc(j - i + 1, sizeof(char));
                ft_strncpy(var, token->content + i, j - i);
                char *to_add = get_var(var);
                free(var);
                if(to_add)
                {
                    res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + ft_strlen(to_add) + 1);
                    ft_strncpy(res + ft_strlen(res), to_add, ft_strlen(to_add));
                }
                i = j;
            }
            else if (single_quotes)
            {
                res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
                res[ft_strlen(res)] =  token->content[i];
                i++;
            }
        }
        else
        {
            res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
            res[ft_strlen(res)] =  token->content[i];
            i++;
        }
    }

    return res;
}

// evaluate
Value *evaluate(Node *node, int input, int output)
{
    ft_printf(out, "Evaluate %k\n", node->token);
    switch(node->token->type)
    {
        case identifier_:
        case redir_input:
        case redir_output:
        case heredoc_:
        {
            int len = 0;
            char **arguments = calloc(len + 2, sizeof(char*));
            int in_tmp = -1;
            int out_tmp = -1;
            // arguments[len] = expand(node->token);
            // ft_printf(out, "command %s:\n", arguments[len]);
            // len++;

            // if(node->token->token_len)
            //     ft_printf(out, "      has arguments:");
            int i = 0;
            int pos = node->token->token_start;
            while(i < node->token->token_len)
            {
                ft_putstr(out, "         ");
                if (tokens[pos]->type == identifier_)
                {
                    arguments = ft_realloc(arguments, len * sizeof(char*), (len + 2) * sizeof(char*));
                    arguments[len] = expand(tokens[pos]);
                    ft_printf(out,"%s\n", arguments[len]);
                    len++;
                }
                // use expand here too
                else if 
                (
                    tokens[pos]->type == redir_input ||
                    tokens[pos]->type == redir_output ||
                    tokens[pos]->type == heredoc_
                )
                {
                    // int fd1 = -1;
                    Type type = tokens[pos]->type;
                    ft_printf(out,"%s : ", type_to_string(tokens[pos]->type));
                    pos++, i++;
                    char *filename = expand(tokens[pos]);
                    ft_printf(out,"%s\n", filename);
                    if(type == redir_input)
                    {
                        input = open(filename, O_RDONLY);
                        close(in_tmp);
                        in_tmp = input;
                    }
                    if(type == redir_output)
                    {
                        output = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
                        close(out_tmp);
                        out_tmp = output;
                    }

                }
                // ft_printf(out,"%s ", expand(tokens[pos]));
                pos++;
                i++;
            }
            char *full_command = get_command(arguments[0]);
            ft_printf(out, "full command is %s\n", full_command);
            int pid = fork();
            if (!pid)
            {  
                int n = 0;
                while(n < pipe_pos)
                {
                    if(pipes[n] != input && pipes[n] != output)
                        close(pipes[n]);
                    n++;
                }
                if(input != in)
                {
                    dup2(input, in);
                    close(input);
                }
                if(output != out)
                {
                    dup2(output, out);
                    close(output);
                }
                if(execve(full_command, arguments, global_env) < 0)
                    ft_printf(err, "Execve failed\n");
            }
            else
                pids[pid_pos++] = pid;

            close(in_tmp);
            close(out_tmp);
            ft_printf(out, "\n");

            return node->token;
            // break;
        }
        case pipe_:
        {
            ft_printf(out, "open pipe\n");
            // if there is no right output will be set to out

            int fd[2];
            if(pipe(fd) < 0 )
                ft_printf(err, "Error opening pipe\n");
            pipes[pipe_pos++] = fd[0];
            pipes[pipe_pos++] = fd[1];

            evaluate(node->left, input, fd[1]);            
            evaluate(node->right, fd[0], output);

            close(pipes[--pipe_pos]);
            close(pipes[--pipe_pos]);

            // else
                // evaluate(node->left, next[0], curr[1]);


            break;
        }
        default:
            ft_printf(err, "Error in evaluate\n");
    }
    return NULL;
}

int main(int argc, char **argv, char **envp)
{
    global_env = envp;
    tokens = ft_calloc(1000, sizeof(Token*));
    envirement = ft_calloc(1000, sizeof(Token*));

    for(int i = 0; envp && envp[i] ;i++)
    {
        if(ft_strncmp("PATH", envp[i], ft_strlen("PATH")) == 0)
        {
            PATH = split(envp[i], ":");
            break;
        }
    }
    tk_pos = 0;
    build_env_tokens(envp);
    ft_memset(tokens, 0, 1000*sizeof(Token *));
    ft_printf(out, "\n");

    // create pipes
    // if(pipe(curr) < 0)
    // {
    //     printf("Error opening pipe 1\n");
    //     exit(0);
    // }
    // if(pipe(next) < 0)
    // {
    //     printf("Error opening pipe 2\n");
    //     exit(0);
    // }

    while(1)
    {
        ft_putstr(out, "minishell> ");
        text = ft_readline(out);
        tk_pos = 0;
        build_tokens();
#if 1
        exe_pos = 0;
        // build tokens
        ft_printf(out, "\n");
        // evaluate
        tk_pos++;
        
        // while(tokens[exe_pos]->type != end_)
        // {
            // ft_printf(out, "Call evaluate\n");
            evaluate(expr(), in, out);
        // }
        while(pid_pos >= 0)
        {
            // ft_printf(out, "=> wait \n");
            waitpid(pids[pid_pos], NULL, 0);
            pid_pos--;
        }
        txt_pos = 0;
        exe_pos = 0;
        pid_pos = 0;
        pipe_pos = 0;
#endif
        ft_memset(tokens, 0, 1000*sizeof(Token *));
    }
}