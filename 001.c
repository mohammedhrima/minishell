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
typedef struct Node Node;

enum Type
{
    skip_,
    identifier_,        // can be command, or file
    dollar_,            // $,
                        // if in double quotes it get expand
                        // else search in envp (recognized as command)
    double_quotes_,     // "
    single_quotes_,     // '
    flag_,              // starts with -
    infile_,            // <
    outfile_,           // >
    heredoc_,           // <<
    append_,            // >>
    pipe_,              // |
    env_,               // $
    echo_,
    cd_,
    pwd_,
    export_,
    unset_,
    exit_,
    space_,
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
int ft_strlen(char *str){ int i = 0; while (str && str[i]) i++; return i;}
void ft_strncpy(char *dest, char *src, int size)
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
int ft_strncmp(char *s1, char *s2, size_t n)
{
    size_t i = 0;
    while (i < n && s2[i] && s1[i] && (unsigned char)s1[i] == (unsigned char)s2[i])
        i++;
    if (i == n)
        return (0);
    return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}
int ft_strcmp(char *s1, char *s2)
{
    size_t i = 0;
    while (s2[i] && s1[i] && (unsigned char)s1[i] == (unsigned char)s2[i])
        i++;
    return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}
void	*ft_calloc(size_t count, size_t size)
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
bool expand = false;
Token *new_token(Type type)
{
    // in new_token check alpha_tokens
    Token *new = calloc(1, sizeof(Token));
    new->content = NULL;
    new->type = type;
    new->content = ft_calloc(txt_pos - start + 1, sizeof(char));
    ft_strncpy(new->content, text + start, txt_pos - start );
    start = txt_pos;
    int i = 0;
    while(alpha_lexic[i].value)
    {
        if(ft_strcmp(alpha_lexic[i].value, new->content) == 0)
        {
            new->type = alpha_lexic[i].type;
            break;
        }  
        i++;
    }
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
                txt_pos++;
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

int main(void)
{
    printf("\n");
    text = "cat <<l|<<L<<L<<L cat <Make > a -e > main";
    printf("%s\n", text);
    build_tokens();
}