#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// macros
#define in STDIN_FILENO
#define out STDOUT_FILENO
#define err STDERR_FILENO

// stupidi implicent declaration error again
void ft_printf(int fd, char *fmt, ...);
void ft_putchar(int fd, int c);
void ft_putstr(int fd, char *str);

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
Token *envirement[1000];
int env_pos;

enum Type
{
    skip_,
    space_,
    
    assign_,
    expand_,

    dollar_,              // $, expanding will be handled in double_quotes_

    identifier_,          // can be command, or file
    string_,
    array_,
    command_,

    args_,
    join_,
    
    // files
    redirect_input,       // <
    redirect_output,      // >
    heredoc_,             // <<
    append_to_file,       // >>
    
    pipe_,                // |
    
    // take argument if there is one
    env_,               // env
    echo_,             
    cd_,                         
    pwd_,
    export_,            
    unset_,         
    exit_,

    // end of tokens          
    end_,
};

struct Token
{
    char *name;
    Type type;

    struct
    {
        // characters
        char *string;
        // arguments
        struct
        {
            // Token **arguments;
            int joined_len;
            int arguments_len;
        };
        // struct
        // {
        //     // Token **array;
        //     int joined_len;
        //     int arguments_len;
        // };

    };  
};

struct {
    char*value;
    Type type;
} symbol_lexic[] = {
    {"<<", heredoc_},
    {">>", append_to_file},
    {"<", redirect_input},
    {">", redirect_output},
    {"|", pipe_},
    {"$", dollar_}, // keep it splited because this case my cause problems $"PATH" vs $PATH vs $$
    {0 , 0}
};

struct {
    char*value;
    Type type;
} alpha_lexic[] = {
    {"echo", echo_},
    {"cd", cd_},
    {"pwd", pwd_},
    {"export", export_},
    {"unset", unset_},
    {"env", env_},
    {"exit", exit_},
    {0 , 0}
};

char *type_to_string(Type type)
{
    struct {
        char *value;
        Type type;
    } lexic[] = {
        {"SPACE", space_},
        {"ASSIGN", assign_},
        {"EXPAND", expand_},
        {"DOLLAR", dollar_},
        {"IDENTIFIER", identifier_},
        {"STRING", string_},
        {"ARRAY", array_},
        {"ARGUMENTS", args_},
        {"JOIN", join_},
        {"INPUT REDIRECTION", redirect_input},
        {"OUTPUT REDIRECTION", redirect_output},
        {"HEREDOC", heredoc_},
        {"APPEND_OUTPUT", append_to_file},
        {"PIPE", pipe_},
        {"ENV", env_},
        {"ECHO", echo_},
        {"CD", cd_},
        {"PWD", pwd_},
        {"EXPORT", export_},
        {"UNSET", unset_},
        {"EXIT", exit_},
        {"END", end_},
        {0, 0}
    };
    for(int i = 0; lexic[i].value; i++)
    {
        if(lexic[i].type == type)
            return lexic[i].value;
    }
    return NULL;
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
        // ft_printf(out, "res[%d] = %s\n",j, res[j] );
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
    // int len = 0;

    while (1)
    {
        int n = read(fd, c, sizeof(char));
        if (n <= 0)
            break;
        if (c[0] == '\n' || c[0] == '\0')
            break;
        if(res == NULL)
        {
            // ft_printf(out, "calloc\n");
            res = ft_calloc(ft_strlen(res) + 2, sizeof(char));
        }
        else
        {    
            // ft_printf(out, "realloc\n");
            res = ft_realloc(res, ft_strlen(res), (ft_strlen(res) + 3) * sizeof(char));
        }
        // res = ft_realloc(res, ft_strlen(res) + 2);
        // ft_printf(out, "len is %d\n", ft_strlen(res));
        res[ft_strlen(res)] = c[0];
        // ft_printf(out, "res: %s\n", res);
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
                    if(variable->name)
                        print_space(fd, space - ft_strlen(variable->name));
                        ft_putstr(out, variable->name);
                    switch (variable->type)
                    {
                    case space_:
                    case identifier_:
                    case end_:
                        // print_space(fd, space - ft_strlen(variable->name));
                        // ft_putstr(fd, variable->name);
                        break;
                    case pipe_:
                    case string_:
                        print_space(fd, space - ft_strlen(variable->string));
                        ft_putstr(fd, variable->string);
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

// Tokens
Token *new_token(Type type, int start, int end)
{
    Token *new = ft_calloc(1, sizeof(Token));
    new->type = type;
    if(start < end)
    {
        if(type == string_ || type == pipe_ || type == space_)
        {
            new->string = ft_calloc(end - start + 1, sizeof(char));
            int i = 0;
            while(start + i < end)
            {
                new->string[i] = text[start + i];
                i++;
            }
        }
        if(type == identifier_)
        {
            new->name = ft_calloc(end - start + 1, sizeof(char));
            int i = 0;
            while(start + i < end)
            {
                new->name[i] = text[start + i];
                i++;
            }
        }
    }
    // else if(type != join_)
    //     new->content = ft_calloc(1, sizeof(char));
    ft_printf(out, "new token with type %15t has value: '%k'\n", new->type, new); // to be checked
    tokens[tk_pos] = new;
    tk_pos++;
    return new;
};

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
        envirement[env_pos] = ft_calloc(1, sizeof(Token));
        envirement[env_pos]->type = string_;
        envirement[env_pos]->name = array[0];
        envirement[env_pos]->string = array[1];
        ft_printf(out, "new env token: %k \n", envirement[env_pos]);
        env_pos++;
        i++;
    }
}

void build_tokens()
{
    // you can set txt_pos as a local variable
    Token *token;
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
            // I added start and txt_pos here, to use them in case $$, second dellar will be considered as identifier
            // handle dollar in evaluation
            continue;
        }
        if
        (
            text[txt_pos] != '\0' &&
            text[txt_pos] != '\'' &&
            text[txt_pos] != '"' &&
            text[txt_pos] != '$' &&
            text[txt_pos] != ' '
        )
        {
            start = txt_pos;
            while
            (
                text[txt_pos] != '\0' &&
                text[txt_pos] != '\'' &&
                text[txt_pos] != '"' &&
                text[txt_pos] != '$' &&
                text[txt_pos] != ' '
            )
            {
                txt_pos++;
            }
            new_token(identifier_, start, txt_pos);
            continue;
        }
    }
    new_token(end_, 0, 0);
    // ft_printf(out, "exit build_tokens\n");
}

// build nodes
struct Node
{
    Node *left;
    Node *right;
    Token *token;
};

void skip(Type type)
{
    if(tokens[exe_pos]->type != type)
        ft_printf(err, "Error Expected '%s'\n", type_to_string(type));
    exe_pos++;
}

Node *new_node(Token *token)
{
    Node *new = ft_calloc(1, sizeof(Node));
    new->token = token;
    ft_printf(out, "new  node with type %15t has token: '%k'\n", token->type, new->token);
    return new;
}

Node *expr();        // expression
Node *assign();      // assignement
Node *pipe_node();   // |
Node *redirection(); // < > << >>
Node *arguments();   // has spaces
Node *join();        // no spaces
Node *prime();       // file, command, path
                     // built in commands: echo, cd, pwd, export, unset, env, exit

Node *expr()
{
    return assign();
}

Node *assign()
{
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
// split by |
Node *pipe_node()
{
    Node *left = redirection();
    if(tokens[exe_pos]->type == space_)
        skip(space_); // to be verified
    while (tokens[exe_pos]->type == pipe_)
    {
        Node *node = new_node(tokens[exe_pos]);
        skip(pipe_);
        node->left = left;
        node->right = redirection();
        // ft_printf(out, "pipe has left %k and right %k\n", node->left->token, node->right->token);
        // exit(0);
        left = node;
        if(tokens[exe_pos]->type == space_)
            skip(space_); // to be verified
    }
    return left;
}

// to be verified
// split by redurection
Node *redirection()
{
    Node *left = NULL;
    if(tokens[exe_pos]->type == space_)
        skip(space_); // to be verified
    int limit = 0;
    
    while
    (
        tokens[exe_pos]->type == heredoc_ ||
        tokens[exe_pos]->type == redirect_input ||
        tokens[exe_pos]->type == redirect_output ||
        tokens[exe_pos]->type == append_to_file 
    )
    {
        Node *node = new_node(tokens[exe_pos]);
        skip(tokens[exe_pos]->type);
        node->left = left;
        node->right = arguments();
        left = node;
        limit++;
        if(limit > 14)
        {
            ft_printf(err,"You exited the limit of heredocs\n");
        }
        if(tokens[exe_pos]->type == space_)
            skip(space_); // to be verified
    }
    if(limit)
        return left;
    return arguments();
}

// splited by space
Node *arguments()
{
    Node *left = join();
    Node *node = left;
    while
    ( 
        tokens[exe_pos]->type == space_ && (
        tokens[exe_pos + 1]->type == identifier_ ||
        tokens[exe_pos + 1]->type == dollar_ ||
        tokens[exe_pos + 1]->type == string_ )
    )
    {
        skip(space_); // to be verified
        left->token->arguments_len++;
        node->left = join();
        node = node->left;
    }
    return left;
}

// joined commands
Node *join()
{
    Node *left = prime();
    Node *node = left;
    while
    ( 
        tokens[exe_pos]->type == identifier_ ||
        tokens[exe_pos]->type == dollar_ ||
        tokens[exe_pos]->type == string_ )
    {
        // left->token->joined_len++;
        node->right = prime();
        node = node->right;
    }
    return left;
}

// if there is no space between command
// append on right
Node *prime()
{
    // printf("call prime\n");
    Node *left = NULL;
    if(tokens[exe_pos]->type == space_)
        skip(space_); // to be verified
    
    if (
        tokens[exe_pos]->type == dollar_ ||
        tokens[exe_pos]->type == identifier_ ||
        tokens[exe_pos]->type == string_)
    {
        Node *node = new_node(tokens[exe_pos]);
        skip(tokens[exe_pos]->type);
        return node;
    }
    
    ft_printf(err,"Error in prime\n");
    return left;
}
// Execution
char **PATH;
char **global_envp;

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
    int i = 0;
    while(arguments && arguments[i])
    {
        ft_printf(out, "%s ", arguments[i]);
        i++;
    }
}
void pwd_func(char **arguments)
{
    printf("%s\n", getcwd(NULL, 0));
}

void cd_func(char **arguments)
{
    // check argument if NULL
    // then go to home directory
    // ft_printf(out, "cd received '%s'\n", arguments);
#if 0
    int i = 0;
    while(node && node->token->arguments && node->token->arguments[i])
    {
        printf("%s ", node->token->arguments[i]);
        i++;
    }
    chdir(node->token->content);
#else
    // to be verified
    // pwd_func(NULL);
    chdir(arguments[0]);
    // pwd_func(NULL);
#endif
}

extern char **environ;
void env_func(char **arguments)
{
    int i = 0;
    while(environ && environ[i])
    {
        // check if ft_strchr(&evirment[i] , '=') // print only thos who
        printf("%s\n", environ[i]);
        i++;
    }
}

void export_func(char **arguments)
{
    // check if node ^ is NULL
    // if(node)
    // {

    // }
    // else
    // {
    //     // print only what in envirement
    //     int i = 0;
    //     while(environ && environ[i])
    //     {
    //         printf("declare -x %s\n", environ[i]);
    //         i++;
    //     }
    // }
    // else it's a variable declaration
}

void exit_func(Node *node)
{
    exit(0); // verify exit code after
}
// make this function return the address fo built in fucntions
void* built_in(char *cmd)
{
    // return
    // (
    //     ft_strcmp(cmd, "echo") == 0   ||
    //     ft_strcmp(cmd, "cd") == 0     ||
    //     ft_strcmp(cmd, "pwd") == 0    ||
    //     ft_strcmp(cmd, "export") == 0 ||
    //     ft_strcmp(cmd, "unset") == 0  ||
    //     ft_strcmp(cmd, "env") == 0    ||
    //     ft_strcmp(cmd, "exit") == 0 
    // );
    if(ft_strcmp(cmd, "echo") == 0)
        return &echo_func;
    if(ft_strcmp(cmd, "cd") == 0)
        return &cd_func;
    if(ft_strcmp(cmd, "pwd") == 0)
        return &pwd_func;
    if(ft_strcmp(cmd, "export") == 0)
        return &export_func;
    if(ft_strcmp(cmd, "env") == 0)
        return &env_func;
    if(ft_strcmp(cmd, "exit") == 0)
        return &exit_func;
    
    return NULL;
}

// execute command in external function
// this function must return it's exit code
void execute()
{
    /*
        execve takes command as follow
        char *arg[] = {"ls", "-la", NULL};
        execve("/bin/ls", arg, NULL);
    */
}
int fd[2];
int cmd;
char *evaluate(Node *node)
{
    switch(node->token->type)
    {
        case pipe_:
        {
            printf("\nopen pipe:\n");
            static int pipe_opened;
            

            // if(pipe_opened == 0)
            // {
                // pipe_opened = 1;
                if (pipe(fd) == -1)
                {
                    perror("Error in pipe \n");
                    exit(-1);
                }
            // }


        	// dup2(fd[1], STDOUT_FILENO);
            // close(fd[1]);
            dup2(fd[1], STDOUT_FILENO);
            
            char *left = evaluate(node->left); // when evaluating command fork and execute it
                                                // return it pid
            // close(fd[1]);
            
            // dup2(fd[0], STDIN_FILENO);
            // fd[1] = out;
            cmd++;
            // int tmp_fd = fd[0];
            // fd[0] = fd[1];
            // fd[1] = tmp_fd;
            dup2(fd[0], STDIN_FILENO);
            char *right = evaluate(node->right);
            // close(fd[1]);

            // close(fd[0]);
            // verify errors !!
            // execute left 

            return NULL;
        }
        case identifier_:
        {
            
            
            // check arrlen, because only head who will heve arrlen > 0
            // return it

            Value *curr = node->token;
            Node *tmp = node->right;
            char *command = curr->name;
            char **arguments = NULL;
            char *to_add = NULL;
            // join
            while(tmp)
            {
                // evaluate if $
                if(tmp->token->type == identifier_)
                    to_add = tmp->token->name;
                if(tmp->token->type == string_)
                    to_add = tmp->token->string;
                // char *to_add = evaluate(tmp);
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
                    to_add = tmp->token->name;
                if(tmp->token->type == string_)
                    to_add = tmp->token->string;
                arguments = ft_realloc(arguments, len * sizeof(char *), (len + 2)*sizeof(char*));
                arguments[len] = to_add;
                len++;
                tmp = tmp->left;
            }
            
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
                ft_printf(out, "command is '%s'\n", command);
                pid_t pid = fork();
                // check if fork fail
                if(pid == 0)
                {
                    // child process
                    // if(cmd == 0)
                    //     dup2(STDOUT_FILENO, fd[1]);
                    // else
                    //     dup2(STDIN_FILENO, fd[0]);
                    ft_printf(out, "child process\n");
                    if(execve(command_to_execute, arguments, global_envp ) < 0)
                        ft_printf(err, "Execve failed\n");
                }
                ft_printf(out, "parrent process\n");
            }    
            // printf("\n");
            return command;
        }
        case string_:
        {
            // 1- fork
            // 2- execute command
            // 3- return its pid
            return node->token->string;
        }
        case dollar_:
        {
            // check node left
            // search in envp
            printf("Search for variable\n");

            return NULL;
        }
        case redirect_input:
        {
            // don't evaluate right
            // printf("set %s as input\n", node->right->token->content);
            return NULL;
        }
        case redirect_output:
        {
            // don't evaluate right
            // printf("set %s as output\n", node->right->token->content);
            return NULL;
        }
        case heredoc_:
        {
            // don't evaluate right
            // printf("open heredoc with %s\n", node->right->token->content);
            return NULL;
        }
        case append_to_file:
        {
            // don't evaluate right
            // printf("append to file %s\n", node->right->token->content);
            return NULL;
        }
        default:
        {
            printf("Unknown token in evaluate\n");
            exit(0);
        }
    }
}

int main(int argc, char **argv, char **envp)
{
#if 0
    pid_t pid = fork();
    // check if fork fail
    if(pid == 0)
    {
        // child process
        ft_printf(out, "child process\n");
        char *arg[] =  {"ls", "-la", NULL};
        if(execve("/bin/ls", arg, global_envp ) < 0)
            ft_printf(err, "Execve failed\n");
    }
    ft_printf(out, "parrent process\n");
    exit(0);
#endif
    global_envp = envp;
    for(int i = 0; envp && envp[i] ;i++)
    {
        if(ft_strncmp("PATH", envp[i], ft_strlen("PATH")) == 0)
        {
            PATH = split(envp[i], ":");
            break;
        }
    }

    build_env_tokens(envp);
    ft_printf(out, "\n");
    while(1)
    {
        text = ft_readline(out);
        tk_pos = 0;
        // build tokens
        build_tokens();
        ft_printf(out, "\n");
        // evaluate
        while(tokens[exe_pos]->type != end_)
            evaluate(expr());

        txt_pos = 0;
        exe_pos = 0;
        ft_memset(tokens, 0, 1000*sizeof(Token *));
    }
}