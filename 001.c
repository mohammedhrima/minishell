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
    identifier_,          // can be command, or file
    // flag_     ,
    dollar_,              // $, expanding will be handled in double_quotes_
    
    // expand
    expand_,

    string_,
    infile_,              // <
    outfile_,             // >
    heredoc_,             // <<
    append_,              // >>
    pipe_,                // |
    // curreent built take argument if there is one
    env_,               // env
    echo_,             
    cd_,                         
    pwd_,
    export_,            
    unset_,         
    exit_,          
    space_,
    join_, // used to join to nodes if there is noe space between them
    end_,
};


struct Token
{
    char *content;
    // bool expand;
    Type type;
    int tk_index;
    // arguments or to_append
    char **arguments;
    int arguments_len;
    int append_len;   
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
    {"$", dollar_}, // keep it splited because this case my cause problems $"PATH" vs $PATH vs $$
    // {"-", flag_},
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
        // case single_quotes_:
        //     return "SINGLE QUOTES";
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
        case string_:
            return "STRING";
        case echo_:
            return "ECHO";
        case cd_:
            return "CD";
        case pwd_:
            return "PWD";
        case export_:
            return "EXPORT";
        case unset_:
            return "UNSET";
        case env_:
            return "ENV";
        case exit_:
            return "EXIT";
        case join_:
            return "JOIN NODE";
        // case flag_:
        //     return "FLAG";
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
// int start;
Token *tokens[1000];

int tk_pos;
int exe_pos;
int tk_index;

Token *new_token(Type type, int start, int end)
{
    Token *new = ft_calloc(1, sizeof(Token));
    new->content = NULL;
    new->type = type;
    new->tk_index = tk_index;
    tk_index++;
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
    else if(type != join_)
        new->content = ft_calloc(1, sizeof(char));
    // printf("check '%s'\n", new->content);
    // if(
    //     type == identifier_ ||
    //     type == string_
    // )
    // {
    //     int i = 0;
    //     while(alpha_lexic[i].value)
    //     {
    //         if(ft_strcmp(alpha_lexic[i].value, new->content) == 0)
    //         {
    //             new->type = alpha_lexic[i].type;
    //             break;
    //         }  
    //         i++;
    //     }
    //     // if(new->content[0] == '-')
    //     //     new->type = flag_;
    // }
    
    printf("new token with type ");
    printf("%s", type_to_string(new->type));
    print_space_(18 - ft_strlen(type_to_string(new->type)));
    printf("content '%s'\n", new->content);


    tokens[tk_pos] = new;
    tk_pos++;
    return new;
};

void error(char c)
{
    printf("\n");
    printf("%s\n", text);
    print_space_(txt_pos);
    printf("^\n");
    printf("Expected '%c'\n", c);
    exit(0);
}

Token *build_tokens()
{
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
            // tk_index++;
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
        if(text[txt_pos])
        {
            start = txt_pos;
            while
            (
#if 0
                text[txt_pos] != '\0' &&
                text[txt_pos] != '\'' &&
                text[txt_pos] != '\"' &&
                text[txt_pos] != '$' &&
                text[txt_pos] != ' '
#else
                ft_isalphanum(text[txt_pos])
#endif
            )
            {
                txt_pos++;
            }
            new_token(identifier_, start, txt_pos);
            continue;
        }

    }
    return new_token(end_, 0, 0);
    // return NULL;
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
    {
        printf("Error Expected '%s'\n", type_to_string(type));
        exit(0);
    }
    exe_pos++;
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
Node *arguments();      // join two
Node *appends();
Node *prime();       // file, command, path
                     // built in commands: echo, cd, pwd, export, unset, env, exit

Node *expr()
{
    return pipe_node();
}

// split commad by |
Node *pipe_node()
{
    // printf("Call pipe\n");
    Node *left = redirection();
    if(tokens[exe_pos]->type == space_)
        skip(space_); // to be verified
    while (tokens[exe_pos]->type == pipe_)
    {
        // printf("found pipe\n");
        Node *node = new_node(tokens[exe_pos]);
        skip(pipe_);
        node->left = left;
        node->right = redirection();
        left = node;
        if(tokens[exe_pos]->type == space_)
            skip(space_); // to be verified
    }
    return left;
}

// to be verified
Node *redirection()
{
    // Node *left = arguments();
    Node *left = NULL;
    if(tokens[exe_pos]->type == space_)
        skip(space_); // to be verified
    int limit = 0;
    
    while(
        tokens[exe_pos]->type == heredoc_ ||
        tokens[exe_pos]->type == append_ ||
        tokens[exe_pos]->type == infile_ ||
        tokens[exe_pos]->type == outfile_ 
    )
    {
        Node *node = new_node(tokens[exe_pos]);
        // if(left == NULL)
        //     left = node;
        skip(tokens[exe_pos]->type);
        // node->left = left;
        node->left = left;
        node->right = arguments();
        left = node;
        limit++;
        if(limit > 14)
        {
            printf("You exited the limit of heredocs\n");
            exit(0);
        }
        if(tokens[exe_pos]->type == space_)
            skip(space_); // to be verified
    }
    if(limit)
        return left;
    return arguments();
}

// if there is space between commands
// append on left
Node *arguments()
{
    // printf("call join\n");
    Node *left = appends();

    Node *tmp = left;
    if
    ( 
        tokens[exe_pos]->type == space_ && (
        tokens[exe_pos + 1]->type == identifier_ ||
        tokens[exe_pos + 1]->type == dollar_ ||
        tokens[exe_pos + 1]->type == string_ )
    )
    {
        skip(space_); // to be verified
        left->token->arguments_len++;
        tmp->left = appends();
        tmp = tmp->left;
    }
    return left;
}

Node *appends()
{
    Node *left = prime();
    Node *tmp = left;
    while
    ( 
        tokens[exe_pos]->type == identifier_ ||
        tokens[exe_pos]->type == dollar_ ||
        tokens[exe_pos]->type == string_ )
    {
        left->token->append_len++;
        tmp->right = prime();
        tmp = tmp->right;
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
    
    if (tokens[exe_pos]->type == dollar_ || tokens[exe_pos]->type == identifier_ || tokens[exe_pos]->type == string_)
    {
        Node *node = new_node(tokens[exe_pos]);
        skip(tokens[exe_pos]->type);
        return node;
    }
    printf("Error in prime\n");
    exit(0);
    return left;
}

#if 1
Value *evaluate(Node *node)
{
    // printf("evaluate %s", type_to_string(node->token->type));
    // print_space_(18 - ft_strlen(type_to_string(node->token->type)));
    // printf("content '%s'\n", node->token->content);
    switch(node->token->type)
    {
        case pipe_:
        {
            // printf("pipe\n");
            Value *left = evaluate(node->left); // when evaluating command fork and execute it
                                                // return it pid
            Value *right = evaluate(node->right);
            // verify errors !!
            // execute left 
            // printf("pipe has left: %s\n", node->left->token->content);
            // printf("pipe has right: %s\n", node->right->token->content);

            return NULL;
            // exit(0);
        }
        case identifier_:
        {
            // 1- fork
            // 2- execute command
            // 3- return its pid from parrent process
            Value *curr = node->token;
            Node *tmp = node->left;
            if(node->right)
                curr->content = strjoin(curr->content, evaluate(node->right)->content);
            printf("command is %s\n", curr->content);
            if(tmp)
            {
                printf("    has current arguments:\n");
                curr->arguments = ft_calloc(node->token->arguments_len + 1, sizeof(char*));
                int i = 0;
                while(i < node->token->arguments_len)
                {
                    curr->arguments[i] = tmp->token->content;
                    printf("        %s\n", curr->arguments[i]);
                    tmp = tmp->left;
                    i++;
                }
            }
            printf("\n");
            // curr->arguments = argu
            // exit(0);
            return curr;
        }
        case string_:
        {
            // 1- fork
            // 2- execute command
            // 3- return its pid
            return node->token;
        }
        case dollar_:
        {
            // check node left
            // search in envp
            return node->left->token;
        }
        case infile_:
        {
            printf("set %s as input\n", node->right->token->content);
            return node->token;
        }
        case outfile_:
        {
            printf("set %s as output\n", node->right->token->content);
            return node->token;
        }
        case heredoc_:
        {
            printf("open heredoc with %s\n", node->right->token->content);
            return node->token;
        }
        case append_:
        {
            printf("append to file %s\n", node->right->token->content);
            return node->token;
        }
        default:
        {
            printf("Unknown token in evaluate\n");
            exit(0);
        }
    }
}
#endif

void execute()
{
    while(tokens[exe_pos]->type != end_)
        evaluate(expr());
}

int main(void)
{
    // printf("\n");
    // text = "\"ls\" \" -la\"";
    text = "cat <l|<<L<<L<<L \"c\"a\"t\" \"<\"M\"ake > a -e > main\"";
    // text = readline(in);
    // printf("\n%s\n", text);

    Token *curr = build_tokens();
    while(curr->type != end_)
        curr = build_tokens();
    tk_pos++;
    execute();
}