#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>

// macros
#define in STDIN_FILENO
#define out STDOUT_FILENO
#define err STDERR_FILENO

// exit codes
#define SUCESS 0
#define NO_SUCH_FILE_OR_DIR 2
#define INPUT_OUTPUT_ERROR 5
#define BAD_FILE_DESCRIPTOR 9
#define PERMISSION_DENIED 13

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

// alocated list
uintptr_t *addresses;
int address_index;
int address_len = 100;

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

// global env
char **global_env;

// process ids
int pids[200];
int pid_pos;

//multiple pipes
int pipes[100];
int pipe_pos;
int inside_pipe;

// exit status
int status;

// types
enum Type
{
    skip_, 
    // =                      (        )
    assign_, identifier_, lparent_, rparent_,
    //  <           >             <<       >>      |
    redir_input, redir_output, heredoc_, append_, pipe_,
    // and or
    and_, or_,
    // built in
    env_, echo_, cd_, pwd_, export_, unset_, exit_,
    // processes id
    pid_,
    // end
    end_
};

struct {
    char *value;
    Type type;
} symbol_lexic[] = {
    {"<<", heredoc_}, {">>", append_}, {"<", redir_input}, {">", redir_output},
    {"&&", and_}, {"||", or_}, {"(", lparent_}, {")", rparent_}, {"|", pipe_}, {0 , 0}
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
        {"ASSIGN", assign_},  {"IDENTIFIER", identifier_},
        {"INPUT REDIR", redir_input}, {"OUTPUT REDIR", redir_output},  
        {"HEREDOC", heredoc_}, {"APPEND_OUTPUT", append_}, {"PIPE", pipe_}, 
        {"ENV", env_}, {"ECHO", echo_}, {"PROCECESS ID", pid_}, {"AND", and_}, 
        {"OR", or_}, {"CD", cd_}, {"PWD", pwd_},  {"EXPORT", export_}, {"UNSET", unset_}, 
        {"EXIT", exit_}, {"END", end_}, 
        {0, 0}
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
    int token_start;
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
    addresses[address_index] = (uintptr_t)new;
    address_index++;
    if(address_index + 10 > address_len)
    {
        void *temporary = malloc(address_len * 2 * sizeof(uintptr_t));
        ft_memcpy(temporary, addresses, address_index * sizeof(uintptr_t));
        address_len *= 2;
        free(addresses);
        addresses = temporary;
    }
	return (new);
}
void *ft_realloc(void *pointer, size_t old_size ,size_t new_size)
{
    if (pointer == NULL)
        return ft_calloc(1, new_size);

    void* new = ft_calloc(1, new_size);
    ft_memcpy(new, pointer, old_size);
    return new;
}
void ft_exit(int code)
{
    while(address_index >= 0)
    {
        free((void*)(addresses[address_index]));
        address_index--;
    }
    exit(code);
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

// ft_itoa
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
    left[ft_strlen(left)] = num % 10 + '0';;
    return left;
}

#if 0
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
        if(res == NULL)
            res = ft_calloc(ft_strlen(res) + 2, sizeof(char));
        else
            res = ft_realloc(res, ft_strlen(res), (ft_strlen(res) + 3) * sizeof(char));
        res[ft_strlen(res)] = c[0];
        if (c[0] == '\n' || c[0] == '\0')
            break;
    }
    // res[ft_strlen(res)] = '\0';
    return res;
}
#endif

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
                    case and_:
                    case or_:
                        print_space(fd, space - ft_strlen(variable->content));
                        ft_putstr(fd, variable->content);
                        break;
                    default:
                        ft_putstr(err, "Unkown given token type: ");
                        ft_putnbr(err, variable->type);
                        ft_putstr(err, "\n");
                        ft_exit(0);
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
        ft_exit(1);
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
        // ft_printf(out, "+ new node with type: %t \n", envirement[env_pos]->token->type);
        // ft_printf(out, "           has  left: %k\n", envirement[env_pos]->left->token);
        // ft_printf(out, "           has right: %k\n\n", envirement[env_pos]->right->token);
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
Node *or();          // ||
Node *and();         // &&
Node *pipe_node();   // |
Node *prime();       // files, command, argument, (), built in commands: echo, cd, pwd, export, unset, env, exit

Node *expr()
{
    ft_printf(out, "call expr\n");
    return or();
}

Node *or()
{
    ft_printf(out, "call or\n");
    Node *left = and();
    while(tokens[exe_pos]->type == or_)
    {
        Node *node = new_node(tokens[exe_pos]);
        skip(or_);
        node->left = left;
        node->right = and();
        left = node;
    }
    return left;
}

Node *and()
{
    ft_printf(out, "call and\n");
    Node *left = pipe_node();
    while(tokens[exe_pos]->type == and_)
    {
        Node *node = new_node(tokens[exe_pos]);
        skip(and_);
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
    if(tokens[exe_pos]->type == lparent_)
    {
        skip(tokens[exe_pos]->type);
        Node *node = expr();
        skip(rparent_);
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
    if(arguments[0])
        chdir(arguments[0]);
    else
    {
        int i = 0;
        char *location = "/";
        while (envirement && envirement[i])
        {
            if(envirement[i]->left->token->content && ft_strcmp(envirement[i]->left->token->content, "HOME") == 0)
            {
                location = envirement[i]->right->token->content;
                // ft_printf(out, "found in cd: %s\n", location);
                break;
            }
            i++;
        }
        chdir(location);
    }
}
void pwd_func(char **arguments)
{
    ft_printf(out, "%s\n", getcwd(NULL, 0));
}
void export_func(char **arguments)
{
    if(arguments[0])
    {
        char **array = split(arguments[0], "=");
        envirement[env_pos] = new_node(new_token(assign_, 0, 0));
        envirement[env_pos]->left = new_node(new_token(identifier_, 0, 0));
        envirement[env_pos]->right = new_node(new_token(identifier_, 0, 0));
        envirement[env_pos]->left->token->content = array[0];
        envirement[env_pos]->right->token->content = array[1];
        ft_printf(out, "+ new node with type: %t \n", envirement[env_pos]->token->type);
        ft_printf(out, "           has  left: %k\n", envirement[env_pos]->left->token);
        ft_printf(out, "           has right: %k\n\n", envirement[env_pos]->right->token);
        env_pos++;
        return;
    }
    int i = 0;
    while(envirement[i])
    {
        if(envirement[i]->left->token->content)
            printf("declare -x %s=\"%s\"\n", envirement[i]->left->token->content, envirement[i]->right->token->content);
        i++;
    }
}
void unset_func(char **argument)
{
    ft_printf(out, "in unset search for '%s'\n", argument[0]);
    // to be handled after
    int i = 0;
    while (envirement && envirement[i])
    {
        if(envirement[i]->left->token->content && ft_strcmp(envirement[i]->left->token->content, argument[0]) == 0)
        {
            ft_printf(out, "found in unset\n");
            envirement[i]->left->token->content = NULL;
            break;
        }
        i++;
    }
}
void env_func(char **arguments)
{
    int i = 0;
    while(envirement[i])
    {
        // print only those who have right token
        if(envirement[i]->left->token->content)
            printf("%s=%s\n", envirement[i]->left->token->content, envirement[i]->right->token->content);
        i++;
    }
}
void exit_func(char **arguments)
{
    ft_exit(0); // verify exit code after
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
    if(ft_strcmp(name, "?") == 0)
        return ft_itoa(status);
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
            // char **arguments = calloc(len + 2, sizeof(char*));
            char **arguments = NULL;
            int in_tmp = -1;
            int out_tmp = -1;

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
                    arguments[len] = NULL;
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
                        if(input < 0)
                        {
                            ft_printf(out, "inpout: %d\n", input);
                            ft_printf(out, "minishell: %s: No such file or directory\n", filename);
                            ft_exit(1);
                        }
                        close(in_tmp);
                        in_tmp = input;
                    }
                    if(type == redir_output)
                    {
                        output = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
                        close(out_tmp);
                        out_tmp = output;
                    }
                    if(type == append_)
                    {
                        output = open(filename, O_APPEND, 0644);
                        close(out_tmp);
                        out_tmp = output;
                    }
                    if(type == heredoc_)
                    {
                        ft_printf(out, "heredoc\n");
                        char *delimiter = filename;
                        filename = ft_calloc(ft_strlen("/tmp/heredoc") + 1,sizeof(char));
                        ft_strcpy(filename, "/tmp/heredoc");
                        // ft_printf(out, "full path is '%s' \n", filename);
                        input =  open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        close(in_tmp);
                        in_tmp = input;
                        // ft_printf(out, "delimiter is '%s'\n", delimiter);
                        char *res = NULL;
                        while(res == NULL || ft_strncmp(res, delimiter, ft_strlen(res) - 1))
                        {
                            if(res)
                                write(input, res, ft_strlen(res));
                            // ft_putstr(out,"heredoc $> ");
                            res = readline("heredoc $> ");
                            // ft_printf(out, "receive '%s'\n", res);
                        }
                        close(input);
                        input =  open(filename, O_RDONLY);
                    }
                    // set it as input
                    // dir chi tekhwar hna
                }
                // ft_printf(out,"%s ", expand(tokens[pos]));
                pos++;
                i++;
            }
            if(arguments && arguments[0])
            {
                // built in function
                void (*func)(char**) = built_in(arguments[0]);
                if(func)
                {
                    // if inside_pipe fork
                    if(inside_pipe)
                    {
                        ft_printf(out, "built in function, with forking\n");
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
                            func(&arguments[1]);
                        }
                        pids[pid_pos++] = pid;
                        node->token->type = pid_;
                        node->token->process_id = pid;
                    }
                    else
                    {
                        ft_printf(out, "built in function, without forking\n");
                        func(&arguments[1]);
                    }
                }
                // executable
                else
                {
                    char *full_command = get_command(arguments[0]);
                    ft_printf(out, "full command is %s has input: %d, output: %d\n", full_command, input, output);
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
                    pids[pid_pos++] = pid;
                    node->token->type = pid_;
                    node->token->process_id = pid;
                }
            }

            close(in_tmp);
            close(out_tmp);
            ft_printf(out, "\n");

            return node->token;
            // break;
        }
        case and_:
        case or_:
        {
#if 1 // to be checked after
            ft_printf(out, "open pipe\n");
            Value *left = evaluate(node->left, input, output);
            if(left->type != pid_)
                ft_printf(err, "Expected pid in and/or\n");

            status = 0;
            waitpid(left->process_id, &status, 0);
            if(node->token->type == and_ && status == 0)
                evaluate(node->right, input, output);
            if(node->token->type == or_ && status != 0)
                evaluate(node->right, input, output);
#endif
            // to check after what to return
            break;
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

            inside_pipe++;
            evaluate(node->left, input, fd[1]);

            evaluate(node->right, fd[0], output);
            inside_pipe--;

            close(pipes[--pipe_pos]);
            close(pipes[--pipe_pos]);

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
    addresses = malloc(address_len * sizeof(uintptr_t));
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


    while(1)
    {
        // ft_putstr(out, "minishell $> ");
        text = readline("minishell $> ");
        if (text)
            add_history(text);
        // text[ft_strlen(text) - 1] = 0; // to eliminate \n
        tk_pos = 0;
        build_tokens();
#if 1
        exe_pos = 0;
        // build tokens
        ft_printf(out, "\n");
        // evaluate
        tk_pos++;
        
        evaluate(expr(), in, out);
        while(pid_pos >= 0)
        {
            waitpid(pids[pid_pos], NULL, 0);
            pid_pos--;
        }
        txt_pos = 0;
        exe_pos = 0;
        pid_pos = 0;
        pipe_pos = 0;
#endif
        ft_memset(tokens, 0, 1000*sizeof(Token *));
        text = NULL;
    }
    ft_exit(0);
}