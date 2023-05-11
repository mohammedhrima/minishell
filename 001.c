#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// macros
#define in STDIN_FILENO
#define out STDOUT_FILENO
#define err STDERR_FILENO

typedef enum Type Type;
typedef struct Token Token;
typedef struct Token Value;
typedef struct Node Node;

enum Type
{
    skip_,
    identifier_,        // can be command, or file
    dollar_,            // $, expanding will be handled in double_quotes_
    double_quotes_,     // "
                        // if you fine $ expand it
                        // else search in envp (recognized as command)
    single_quotes_,     // '
    flag_,              // starts with -
    infile_,            // <
    outfile_,           // >
    heredoc_,           // <<
    append_,            // >>
    pipe_,              // |
    // will be readen as identifier
    // or double quotes and get expanded after is double quotes
    env_,               // env
    // curreent built take argument if there is one
    echo_,             
    cd_,                         
    pwd_,
    export_,            
    unset_,         
    exit_,          
    space_,
    join_,
    end_,
};

struct Token
{
    char *content;
    bool expand;
    Type type;
};

struct {
    char*value;
    Type type;
} symbol_lexic[] = {
    {"<<", heredoc_},
    {">>", append_},
    {"<", infile_},
    {">", outfile_},
    {"|", pipe_},
    {"$", dollar_},
    {"-", flag_},
    {0 , 0}
};

// struct {
//     char*value;
//     Type type;
// } alpha_lexic[] = {
//     {"echo", echo_},
//     {"cd", cd_},
//     {"pwd", pwd_},
//     {"export", export_},
//     {"unset", unset_},
//     {"env", env_},
//     {"exit", exit_},
//     {0 , 0}
// };

// characters methods
int ft_isnum(int c) { return (c >= '0' && c <= '9'); }
int ft_isupper(int c) { return (c >= 'A' && c <= 'Z'); }
int ft_islower(int c) { return (c >= 'a' && c <= 'z'); }
int ft_isalpha(int c) { return (ft_islower(c) || ft_isupper(c)); }
int ft_isalphanum(int c) { return (ft_isalpha(c) || ft_isnum(c)); }

//string methods
int     ft_strlen(char *str){ int i = 0; while (str && str[i]) i++; return i;}
void    ft_strncpy(char *dest, char *src, int size)
{
    if (dest == NULL || src == NULL)
    {    
        printf("receive NULL in strncpy\n");
        exit(0);
    }
    int len = ft_strlen(dest);
    int i = 0;
    while (src[i] && i < size)
    {
        dest[len + i] = src[i];
        i++;
    }
}
int     ft_strncmp(char *s1, char *s2, size_t n)
{
    size_t i = 0;
    while (i < n && s2[i] && s1[i] && (unsigned char)s1[i] == (unsigned char)s2[i])
        i++;
    if (i == n)
        return (0);
    return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}
int     ft_strcmp(char *s1, char *s2)
{
    size_t i = 0;
    while (s2[i] && s1[i] && (unsigned char)s1[i] == (unsigned char)s2[i])
        i++;
    return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}
void    *ft_calloc(size_t count, size_t size)
{
	void			*arr;
	unsigned char	*ptr;
	size_t			i;

	arr = (void *)malloc(count * size);
	if (!arr)
		return (NULL);
	i = 0;
	ptr = (unsigned char *)arr;
	while (i < count * size)
	{
		ptr[i] = 0;
		i++;
	}
	return (arr);
}

// readline
char *strjoin(char *string1, char *string2)
{
    char *res = ft_calloc(ft_strlen(string1) + ft_strlen(string2) + 1, sizeof(char));
    if (string1)
        ft_strncpy(res, string1, ft_strlen(string1));
    if (string2)
        ft_strncpy(res + ft_strlen(res), string2, ft_strlen(string2));
    return res;
}
char *readline(int fd)
{
    char *res = NULL;
    char *c = ft_calloc(2, sizeof(char));
    char *tmp;

    while (1)
    {
        int n = read(fd, c, sizeof(char));
        if (n <= 0)
            break;
        tmp = strjoin(res, c);
        free(res);
        res = tmp;
        if (c[0] == '\n' || c[0] == '\0')
            break;
    }
    if(tmp)
        tmp[ft_strlen(tmp) - 1] = '\0'; // replace newline with /0
    return res;
}

char *type_to_string(Type type)
{
    switch(type)
    {
        case identifier_:
            return "IDENTIFIER";
        case dollar_:
            return "DOLLAR";
        case double_quotes_:
            return "DOUBLE QUOTES";
        case single_quotes_:
            return "SINGLE QUOTES";
        case flag_:
            return "FLAG";
        case infile_:
            return "INFILE";
        case outfile_:
            return "OUTFILE";
        case heredoc_:
            return "HEREDOC";
        case append_:
            return "APPEND_OUTPUT";
        case pipe_:
            return "PIPE";
        case space_:
            return "SPACE";
        case end_:
            return "END"; 
        // case echo_:
        //     return "ECHO";
        // case cd_:
        //     return "CD";
        // case pwd_:
        //     return "PWD";
        // case export_:
        //     return "EXPORT";
        // case unset_:
        //     return "UNSET";
        // case env_:
        //     return "ENV";
        // case exit_:
        //     return "EXIT";
        case join_:
            return "JOIN";
        default:
            return "Unknown type";
    }
    return NULL;
}

void print_space_(int len)
{
    int i = 0;
    while(i < len)
    {
        printf(" ");
        i++;
    }
}

char *text;
int txt_pos;
int start;
Token *tokens[1000];

int tk_pos;
int exe_pos;

Token *new_token(Type type)
{
    // in new_token check alpha_tokens
    Token *new = ft_calloc(1, sizeof(Token));
    new->content = NULL;
    new->type = type;
    new->content = ft_calloc(txt_pos - start + 1, sizeof(char));
    ft_strncpy(new->content, text + start, txt_pos - start );
    start = txt_pos;
    // int i = 0;
    // while(alpha_lexic[i].value)
    // {
    //     if(ft_strcmp(alpha_lexic[i].value, new->content) == 0)
    //     {
    //         new->type = alpha_lexic[i].type;
    //         break;
    //     }  
    //     i++;
    // }
    printf("new token with type '%s', content '%s'\n", type_to_string(type), new->content);
    tokens[tk_pos] = new;
    tk_pos++;
    return new;
};

void error(char c)
{
    printf("\n");
    print_space_(txt_pos);
    printf("^\n");
    printf("Expected '%c'\n", c);
    exit(0);
}

void build_tokens()
{
    while(text && text[txt_pos])
    {
        start = txt_pos;
        if(text[txt_pos] == '\0')
            break;
        // skip spaces
        if(text[txt_pos] == ' ')
        {
            // start = txt_pos;
            while(text[txt_pos] == ' ')
                txt_pos++;
            new_token(space_);
            continue;
        }
        // double quotes command
        if(text[txt_pos] == '"')
        {
            txt_pos++;
            start = txt_pos;
            while(text[txt_pos] && text[txt_pos] != '"')
            {
                // if(text[txt_pos] == '$')
                txt_pos++;
            
            }
            if(text[txt_pos] != '"')
                error('"');
            new_token(double_quotes_);
            txt_pos++;
            continue;
        }
        // single quotes command
        if(text[txt_pos] == '\'')
        {
            txt_pos++;
            start = txt_pos;
            while(text[txt_pos] && text[txt_pos] != '\'')
                txt_pos++;
            if(text[txt_pos] != '\'')
                error('\'');
            new_token(single_quotes_);
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
                // start = txt_pos;
                type = symbol_lexic[i].type;
                txt_pos += ft_strlen(symbol_lexic[i].value);
                // printf("start from %d to %d\n", start, txt_pos);
                new_token(symbol_lexic[i].type);
                break;
            }
            i++;
        }
        if(type)
            continue;
        // start = txt_pos;
        while(ft_isalphanum(text[txt_pos]))
            txt_pos++;
        new_token(identifier_);
        continue;
    }
    new_token(end_);
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
    if(tokens[tk_pos]->type != type)
    {
        printf("Error Expected '%s'\n", type_to_string(type));
        exit(0);
    }
    tk_pos++;
}

Node *new_node(Token *token)
{
    Node *new = ft_calloc(1, sizeof(Node));
    new->token = token;
    printf("new node with type '%s', content '%s'\n", type_to_string(token->type), new->token->content);
    return new;
}

Node *expr();        // expression
Node *pipe_node();   // |
Node *redirection(); // < > << >>
Node *flag();        // flag
Node *join();        // join two
Node *prime();       // file, command, path
                     // built in commands: echo, cd, pwd, export, unset, env, exit

Node *expr()
{
    return pipe_node();
}

Node *pipe_node()
{
    Node *left = redirection();
    while (tokens[exe_pos]->type == pipe_)
    {
        Node *node = new_node(tokens[exe_pos]);
        skip(pipe_);
        node->left = left;
        node->right = redirection();
        left = node;
    }
    return left;
}

Node *redirection()
{
    Node *left = flag();
    while(
        tokens[exe_pos]->type == heredoc_ ||
        tokens[exe_pos]->type == append_ ||
        tokens[exe_pos]->type == infile_ ||
        tokens[exe_pos]->type == outfile_ 
    )
    {
        Node *node = new_node(tokens[exe_pos]);
        skip(tokens[exe_pos]->type);
        node->left = left;
        node->right = flag();
        left = node;
    }
    return left;
}

Node *flag()
{
    Node *left = join();
    while(tokens[exe_pos]->type == flag_)
    {
        Node *node = new_node(tokens[exe_pos]);
        skip(flag_);
        node->left = left;
        node->right = join();
        // expect something here
        left = node;
    }
    return left;
}

Node *join()
{
    Node *left = prime();
    // check it may segfault
    while(
        tokens[exe_pos]->type == identifier_ ||
        tokens[exe_pos]->type == dollar_     ||
        tokens[exe_pos]->type == double_quotes_ ||
        tokens[exe_pos]->type == single_quotes_
    )
    {
        Node *node = new_node(new_token(join_));
        node->left = left;
        node->right = prime();
        left = node;
    }
    return left;
}

Node *prime()
{
    Node *left = NULL;
    if(tokens[exe_pos]->type == space_)
        skip(space_);
    if
    (
        tokens[exe_pos]->type == identifier_    ||
        tokens[exe_pos]->type == dollar_        ||
        tokens[exe_pos]->type == double_quotes_ ||
        tokens[exe_pos]->type == single_quotes_
    )
    {
        left = new_node(tokens[exe_pos]); // to check after
        skip(tokens[exe_pos]->type);
        return left;
    }
    if(tokens[exe_pos]->type == echo_)
    {
        Node *left = new_node(tokens[exe_pos]);
        skip(tokens[exe_pos]->type);
        Node *node = left;
        while
        (
            tokens[exe_pos]->type == identifier_    ||
            tokens[exe_pos]->type == dollar_        ||
            tokens[exe_pos]->type == double_quotes_ ||
            tokens[exe_pos]->type == single_quotes_
        )
        {
            node->left = new_node(tokens[exe_pos]);
            skip(tokens[exe_pos]->type);
            if(tokens[exe_pos]->type == space_)
                skip(space_);
            node = node->left;
        }
    }
    return left;
}

Value *evaluate(Node *node)
{
    switch(node->token->type)
    {
        
    }
}

void execute()
{
    while(tokens[exe_pos]->type != end_)
        evaluate(expr());
}

int main(void)
{
    printf("\n");
    text = "cat <<l|<<L<<L<<L cat <Make > a -e > main";
    printf("%s\n", text);
    build_tokens();
    tk_pos++;
}