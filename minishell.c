#include "header.h"

// exit codes
#define SUCESS 0
#define NO_SUCH_FILE_OR_DIR 1
#define PERMISSION_DENIED 1

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

struct
{
    List addresses;
    List tokens;
    List envirement;
    List files;
    List pids;
    List pipes;
    char **path;
    char **env;
} global;


List *new_list(List *list, size_t size)
{
    if(list->len == 0)
    {
        list->pos = 0;
        list->len = 100;
        list->size = size;
        list->pointer = malloc(list->len * size);
        ft_memset(list->pointer, 0, list->len * list->size);
    }
    return list;
}

void free_list(List *list)
{
    int i = 0;
    while(i < list->pos)
    {
        free(((void**)list->pointer)[i]);
        i++;
    }
    list->pos = 0;
    list->len = 0;
    list->size = 0;
    free(list->pointer);
}

Token *new_token(Type type, char *source, int len)
{
    Token *new = ft_calloc(1, sizeof(Token));
    new->type = type;
    if(source)
    {
        new->value = ft_calloc(len + 1, sizeof(char));
        int i = 0;
        while(i < len)
        {
            new->value[i] = source[i];
            i++;
        }    
    }
    ft_printf(out, "new token with type %12t ", new->type);
    if(new->value)
        ft_printf(out, "has value: '%k'", new);
    ft_printf(out, "\n");

    add_to_list(&global.tokens, new);
    return new;
};

void add_to_list(List *list, void *pointer)
{
    ((void**)list->pointer)[list->pos++] = pointer;
    if(list->pos + 10 > list->len)
    {
        list->len *= 2;
        void *temporary = malloc(list->len * list->size);
        ft_memset(temporary, 0, list->len * list->size);
        ft_memcpy(temporary, list->pointer, list->pos * list->size);
        free(list->pointer);
        list->pointer = temporary;
    }
}


void build_envirement(char **envp)
{
    int i = 0;
    while(envp && envp[i])
    {
        // // ft_printf(out, "%s\n", envp[i]);
        char **array = split_by_two(envp[i], '='); // split by only two, to be verified after
        Node *node = malloc(sizeof(Node));
        ft_memset(node, 0, sizeof(Node));

        node->left = malloc(sizeof(Node));
        node->left->token = malloc(sizeof(Token));
        node->left->token->value = malloc(ft_strlen(array[0]) + 1);
        ft_memset(node->left->token->value, 0, ft_strlen(array[0]) + 1);
        ft_strcpy(node->left->token->value, array[0]); // because I will free every time
        free(array[0]);

        node->right = malloc(sizeof(Node));
        node->right->token = malloc(sizeof(Token));
        node->right->token->value = malloc(ft_strlen(array[1]) + 1);
        ft_memset(node->right->token->value, 0, ft_strlen(array[1]) + 1);
        ft_strcpy(node->right->token->value, array[1]); // because I will free every time
        free(array[1]);

        free(array);

        ft_printf(out, "%s is %s\n", node->left->token->value, node->right->token->value);
        add_to_list(&global.envirement, node);
        i++;
    }
}
void build_tokens(char *text)
{
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
        if(text[txt_pos] == ' ')
        {
            start = txt_pos;
            while(text[txt_pos] == ' ')
                txt_pos++;
            continue;
        }
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
            new_token(type, text + start, txt_pos - start);
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
        new_token(identifier_, text + start, txt_pos - start);
    }
    new_token(end_, NULL, 0);
}

Node *new_node(Token *token)
{
    Node *new = ft_calloc(1, sizeof(Node));
    add_to_list(&global.addresses, new);
    new->token = token;
    ft_printf(out, "new  node with type %15t has token: '%k'\n", token->type, new->token);
    return new;
}

void skip(Type type)
{
    if(((Token**)global.tokens.pointer)[global.tokens.pos]->type != type)
        ft_printf(err, "Error Expected '%s'\n", type_to_string(type));
    global.tokens.pos++;
}

Node *expr()
{
    ft_printf(out, "call expr\n");
    return or();
}

// maybe || and && have the same precedence (check in testing)
Node *or()
{
    ft_printf(out, "call or\n");
    Node *left = and();
    while(((Token**)global.tokens.pointer)[global.tokens.pos]->type == or_)
    {
        Node *node = new_node(((Token**)global.tokens.pointer)[global.tokens.pos]);
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
    while(((Token**)global.tokens.pointer)[global.tokens.pos]->type == and_)
    {
        Node *node = new_node(((Token**)global.tokens.pointer)[global.tokens.pos]);
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
    while(((Token**)global.tokens.pointer)[global.tokens.pos]->type == pipe_)
    {
        static int i = 0;
        ft_printf(out, "find pipe %d\n", i++);
        Node *node = new_node(((Token**)global.tokens.pointer)[global.tokens.pos]);
        skip(pipe_);
        node->left = left;
        node->right = prime();
        left = node;
    }
    return left;
}

bool check(Type type)
{
    return( type == identifier_ || type == redir_input || type == redir_output || type == heredoc_ || type == append_ );
}

Node *prime()
{
    ft_printf(out, "call prime: tokens[%d] has type %s\n", global.tokens.pos, type_to_string(((Token**)global.tokens.pointer)[global.tokens.pos]->type));
    
    if (check(((Token**)global.tokens.pointer)[global.tokens.pos]->type))
    {
        Node *node = new_node(((Token**)global.tokens.pointer)[global.tokens.pos]);
        node->token->start = global.tokens.pos;
        node->token->len = 0;
        while(check(((Token**)global.tokens.pointer)[global.tokens.pos]->type))
        {
            node->token->len++;
            skip(((Token**)global.tokens.pointer)[global.tokens.pos]->type);
        }
        return node;
    }
    if(((Token**)global.tokens.pointer)[global.tokens.pos]->type == lparent_)
    {
        skip(((Token**)global.tokens.pointer)[global.tokens.pos]->type);
        Node *node = expr();
        skip(rparent_);
        return node;
    }
    ft_printf(err, "Error in prime, tokens[%d] has type %t\n", global.tokens.pos ,((Token**)global.tokens.pointer)[global.tokens.pos]->type);
    return NULL;
}


Value *evaluate(Node *node, File input, File output)
{
    // int i = 0;

}

int main(int argc, char **argv, char **envp)
{
    global.env = envp;
    new_list(&global.addresses, sizeof(void*));
    
    new_list(&global.envirement, sizeof(Node*));
    new_list(&global.files, sizeof(File*));
    new_list(&global.pids, sizeof(int));
    new_list(&global.pipes, sizeof(int));
    char *text;

    int i = 0;
    while (envp && envp[i])
    {
        if(ft_strncmp("PATH", envp[i], ft_strlen("PATH")) == 0)
        {
            ft_printf(out, "found PATH\n");
            global.path = split(envp[i], ":");
            i = 0;
            while(global.path && global.path[i])
            {
                add_to_list(&global.addresses, global.path[i]);
                i++;
            }
            break;
        }
        i++;
    }
    build_envirement(envp);

    File input;
    File output;

    input.name = NULL;
    input.fd = in;

    output.name = NULL;
    output.fd = out;

    while(1)
    {
        new_list(&global.tokens, sizeof(Token*));
        text = readline("minishell $> ");
        if(text)
            add_history(text);
        ft_printf(out, "receive: %s\n", text);
        build_tokens(text);
        global.tokens.pos = 0;

        evaluate(expr(), file_in, file_out);

        free(text);
        text = NULL;
        free_list(&global.tokens);
    }
}