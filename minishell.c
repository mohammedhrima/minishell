#include "header.h"

// exit codes
#define SUCESS 0
#define NO_SUCH_FILE_OR_DIR 1
#define PERMISSION_DENIED 1
#define NOT_OPENED -200

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

List *new_list(List *list, size_t size)
{
    if(list->len == 0)
    {
        list->pos = 0;
        list->len = 100;
        list->size = size;
        list->pointer = ft_calloc(list->len, size);
    }
    return list;
}

void free_list(List *list)
{
    // if get error use ft_memeset 0
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
        void *temporary = ft_calloc(list->len, list->size);
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
        Node *node = ft_calloc(1, sizeof(Node));

        node->left = ft_calloc(1, sizeof(Node));
        node->left->token = ft_calloc(1, sizeof(Token));
        node->left->token->value = ft_calloc(ft_strlen(array[0]) + 1, sizeof(char));
        ft_strcpy(node->left->token->value, array[0]); // because I will free every time

        node->right = ft_calloc(1, sizeof(Node));
        node->right->token = ft_calloc(1, sizeof(Token));
        node->right->token->value = ft_calloc(ft_strlen(array[1]) + 1, sizeof(char));
        ft_strcpy(node->right->token->value, array[1]); // because I will free every time

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
    new->token = token;
    ft_printf(out, "new  node with type %15t has token: '%k'\n", token->type, new->token);
    return new;
}

void skip(Type type)
{
    Token *token = (Token*)global.tokens.pointer[global.tokens.pos];
    if(token->type != type)
        ft_printf(err, "Error Expected '%s'\n", type_to_string(type));
    global.tokens.pos++;
}

Node *expr()
{
    ft_printf(out, "call expr\n");
    return and();
}

Node *and()
{
    ft_printf(out, "call and\n");
    Node *left = pipe_node();
    Token *token = (Token*)global.tokens.pointer[global.tokens.pos];
#if 0
    if(token->type == and_ || token->type == or_)
    {
        ft_printf(out, "find %t\n", token->type);
        Node *node = new_node(token);
        skip(token->type);
        node->left = left;
        node->right = and();
        left = node;
        // token = (Token*)global.tokens.pointer[global.tokens.pos];
    }
    return left;

#elif 1
    while(token->type == and_ || token->type == or_)
    {
        node->right = new_node(token);
        node = node->right;

        node->token->start = global.tokens.pos;
        node->token->len = 0;
        while(token->type == and_ || token->type == or_)
        {
            skip(token->type);
            node->token->len++;
#if 0
            node->left = left;
            node->right = pipe_node();
            left = node;
#else
            pipe_node();
#endif
            token = (Token*)global.tokens.pointer[global.tokens.pos];
        }
#endif
    }
    return left;
}
// maybe || and && have the same precedence (check in testing)
#if 0
Node *or()
{
    ft_printf(out, "call or\n");
    Node *left = pipe_node();
    Token *token = (Token*)global.tokens.pointer[global.tokens.pos];
    while(token->type == or_)
    {
        Node *node = new_node(token);
        skip(token->type);
        node->left = left;
        node->right = pipe_node();
        left = node;
        token = (Token*)global.tokens.pointer[global.tokens.pos];
    }
    return left;
}
#endif
Node *pipe_node()
{
    ft_printf(out, "call pipe\n");
    Node *left = prime();
    Token *token = (Token*)global.tokens.pointer[global.tokens.pos];
    while(token->type == pipe_)
    {
        static int i = 0;
        ft_printf(out, "find pipe %d\n", i++);
        Node *node = new_node(token);
        skip(pipe_);
        node->left = left;
        node->right = prime();
        left = node;
        token = (Token*)global.tokens.pointer[global.tokens.pos];
    }
    return left;
}

bool check(Type type)
{
    return( type == identifier_ || type == redir_input || type == redir_output || type == heredoc_ || type == append_ );
}

Node *prime()
{
    Token *token = (Token*)global.tokens.pointer[global.tokens.pos];

    ft_printf(out, "call prime: tokens[%d] has type %s\n", global.tokens.pos, type_to_string(token->type));
    if (check(token->type))
    {
        Node *node = new_node(token);
        node->token->start = global.tokens.pos;
        node->token->len = 0;
        while(check(token->type))
        {
            node->token->len++;
            skip(token->type);
            token = (Token*)global.tokens.pointer[global.tokens.pos];
        }
        return node;
    }
    if(token->type == lparent_)
    {
        skip(token->type);
        Node *node = expr();
        skip(rparent_);
        return node;
    }
    ft_printf(err, "Error in prime, tokens[%d] has type %t\n", global.tokens.pos , token->type);
    return NULL;
}

// Execution
char *get_command(char *cmd)
{
    ft_printf(out, "call get command to find %s\n", cmd);
    if(cmd == NULL || ft_strchr(cmd, '/'))
        return cmd;
    int i = 0;
    while(global.path && global.path[i])
    {
        char *res = strjoin(global.path[i], "/", cmd);
        if (access(res, F_OK) == 0 && access(res, X_OK) == 0)
        {
            ft_printf(out, "Command found\n");
            // exit(0);
            return (res);
        }
        i++;
    }
    ft_printf(err, "allo\n");
    return cmd;
}

// expanding
char *get_var(char *name)
{
    int i = 0;
    ft_printf(out, "Enter get_var, search for '%s'\n", name);
    Node **nodes = (Node **)global.envirement.pointer;
    // if(ft_strcmp(name, "?") == 0)
    //     return ft_itoa(status);
    while(nodes && nodes[i])
    {
        if(ft_strcmp(nodes[i]->left->token->value, name) == 0)
        {
            return nodes[i]->right->token->value;
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

    while(token->value && token->value[i])
    {
        // ft_printf(out, "loop '%c'\n", token->content[i]);
        if(token->value[i] == '\'')
        {
            if(!double_quotes)
                single_quotes = !single_quotes;
            else
            {
                res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
                res[ft_strlen(res)] =  token->value[i];
            }
            i++;
        }
        else if(token->value[i] == '"')
        {
            if(!single_quotes)
                double_quotes = !double_quotes;
            else
            {
                res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
                res[ft_strlen(res)] =  token->value[i];
            }
            i++;
        }
        else if (token->value[i] == '$')
        {
            if(double_quotes || !single_quotes)
            {
                i++;
                int j = i;
                while (token->value[j] && !ft_strchr(" \"", token->value[j]))
                    j++;
                char *var = ft_calloc(j - i + 1, sizeof(char));
                ft_strncpy(var, token->value + i, j - i);
                char *to_add = get_var(var);
                if(to_add)
                {
                    res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + ft_strlen(to_add) + 2);
                    ft_strncpy(res + ft_strlen(res), to_add, ft_strlen(to_add));
                }
                i = j;
            }
            else if (single_quotes)
            {
                res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
                res[ft_strlen(res)] =  token->value[i];
                i++;
            }
        }
        else
        {
            res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
            res[ft_strlen(res)] =  token->value[i];
            i++;
        }
    }

    return res;
}

File *new_file(char* name, int fd, Type type)
{
    File *new = ft_calloc(1, sizeof(File));
    new->name = name;
    new->fd = fd;
    new->type = type;

    add_to_list(&global.files, new);
    return new;
}

void duplicate(File *file, int old)
{
    if(file->fd == NOT_OPENED && file->name == NULL)
    {    
        ft_printf(out, "Error need a file name\n");
        exit(-700);
    }
    
    if(file->type == redir_input)
    {
        if(file->fd == NOT_OPENED)
        {
            if(access(file->name, F_OK)) // check existance
            {    
                ft_printf(out, "minishell: %s: No such file or directory\n", file->name);
                ft_exit(NO_SUCH_FILE_OR_DIR);
            }
            else if(access(file->name, R_OK)) // check if readable
            {
                ft_printf(out, "minishell: %s: Permission denied\n", file->name);
                ft_exit(PERMISSION_DENIED);
            }
            else
            {
                // verify access type
                file->fd = open(file->name, O_RDONLY);
            }
        }
    }
    if(file->type == redir_output)
    {
        if(file->fd == NOT_OPENED)
        {
            if(access(file->name, F_OK) == 0 &&  access(file->name, W_OK)) // check if readable
            {
                ft_printf(out, "minishell: %s: Permission denied\n", file->name);
                ft_exit(PERMISSION_DENIED);
            }
            else
            {
                // verify access 0644 or 0777 // to check after
                file->fd = open(file->name,  O_WRONLY | O_CREAT | O_TRUNC, 0644);
            }
        }   
    }
    dup2(file->fd, old);
}

Value *evaluate(Node *node, File input, File output)
{
    ft_printf(out, "Evaluate %k\n", node->token);

    switch(node->token->type)
    {
        case identifier_:
        case redir_input: // verify if need to be removed
        case redir_output:
        case heredoc_: 
        {
            int len = 0;
            char **arguments = NULL;
            int i = 0;
            int pos = node->token->start;
            while(i < node->token->len)
            {
                Token *token = (Token*)global.tokens.pointer[pos];
                if (token->type == identifier_)
                {
                    arguments = ft_realloc(arguments, len * sizeof(char*), (len + 2) * sizeof(char*));
                    arguments[len] = expand(token);
                    ft_printf(out,"         %s from '%s'\n", arguments[len], token->value);
                    len++;
                    arguments[len] = NULL;
                }
                else if
                (
                    token->type == redir_input ||
                    token->type == redir_output ||
                    token->type == heredoc_
                )
                {
                    Type type = token->type;
                    ft_printf(out,"%s : ", type_to_string(token->type));
                    i++;
                    pos++;
                    token = (Token*)global.tokens.pointer[pos];
                    char *filename = expand(token);
                    ft_printf(out,"%s\n", filename);
                    if(type == redir_input)
                    {
                        /*
                            be carefull here because maybe you will
                            get back to stdin 
                        */
                        input = *new_file(filename, NOT_OPENED, type);
                    }
                    if(type == redir_output)
                    {
                        /*
                            be carefull here because maybe you will
                            get back to stdout 
                        */
                        output = *new_file(filename, NOT_OPENED, type);
                    }
                    if(type == heredoc_)
                    {
                        ft_printf(out, "heredoc\n");
                        char *delimiter = filename;
                        filename = ft_strdup("/tmp/heredoc");
                        
                        input =  *new_file(filename, open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644), type);
                        char *res = NULL;
                        while(res == NULL || ft_strcmp(res, delimiter))
                        {
                            if(res)
                                write(input.fd, res, ft_strlen(res));
                            res = readline("heredoc $> ");
                        }
                        close(input.fd);
                        input.type = redir_input;
                        input =  *new_file(filename, NOT_OPENED, type);
                    }
                }
                pos++;
                i++;
            }
            if(arguments && arguments[0])
            {
                char *full_command = get_command(arguments[0]);
                ft_printf(out, "full command is %s\n    has  input: %F\n    has output: %F\n", full_command, input, output);
                
                int *ptr = ft_calloc(1,sizeof(int));
                add_to_list(&global.pids, ptr);
                int pid = fork();
                *ptr = pid;
                node->token->type = pid_;
                node->token->process_id = pid;
                if (pid == 0)
                {  
                    int **pipes = (int**)global.pipes.pointer;
                    int i = 0;
                    while(i < global.pipes.pos)
                    {
                        int fd = *(pipes[i]);
                        if(fd != input.fd && fd != output.fd && fd != out && fd != in)
                            close(fd);
                        i++;
                    }
                    if(input.type == redir_input)
                    {
                        ft_putstr(out, "redir input\n");
                        duplicate(&input, in);
                        close(input.fd);
                    }
                    if(output.type == redir_output)
                    {
                        ft_putstr(out, "redir output\n");
                        duplicate(&output, out);
                        close(output.fd);
                    }
                    // check exit code if command not valid
                    if(execve(full_command, arguments, global.env) < 0)
                        ft_printf(err, "Execve failed\n");
                }
                // waitpid(pid, 0, 0);
            }
            return node->token;
        }
        case pipe_:
        {
            ft_printf(out, "open pipe\n");
            // if there is no right output will be set to out

            int *fd;
            fd = ft_calloc(2 , sizeof(int));

            if(pipe(fd) < 0 )
                ft_printf(err, "Error opening pipe\n");
            
            add_to_list(&global.pipes, &fd[0]);
            add_to_list(&global.pipes, &fd[1]);
            
            global.inside_pipe++;
            evaluate(node->left, input, *new_file(NULL, fd[1], redir_output));

            evaluate(node->right, *new_file(NULL, fd[0], redir_input), output);
            global.inside_pipe--;

            int **pipes = (int **)global.pipes.pointer; 

            global.pipes.pos--;
            close(*(pipes[global.pipes.pos]));
            global.pipes.pos--;
            close(*(pipes[global.pipes.pos]));

            break;
        }
        case and_:
        case or_:
        {
# if 1
            Node *tmp = node;
            Token* token = tmp->token;
            int status = 1;
            
            if(tmp)
            {
                Value *left = evaluate(tmp, input, output);
                if(left->type != pid_)
                    ft_printf(err, "Expected pid in and/or\n");
                
                waitpid(left->process_id, &status, 0);
                ft_printf(out,"and_or: received status: %d\n", status);
                global.pids.pos--;
                if(token->type == or_ && status != 0)
                {
                    evaluate(tmp->right);
                }
                if(token->type == or_ && status == 0)
                {

                }
                if(token->type == and_ && status != 0)
                    tmp = tmp->right;

                // tmp = tmp->right;
                // token = tmp->token;         
                // status = 1;
            }
            return token;
#elif 0
            if(node->token->type == and_ && status == 0)
            {
                Value *right = evaluate(node->right, input, output);
                return right;
            }
            if(node->token->type == or_ && status != 0)
            {
                Value *right = evaluate(node->right, input, output);
                return right;
            }
            return left;
#elif 0
            int i = 0;
            int pos = node->token->start;
            while(i < node->token->len)
            {
                Token *token = (Token*)global.tokens.pointer[pos];
                Value *value = evaluate(token, input, output);
                if(value->type != pid_)
                    ft_printf(err, "Expected pid in and/or\n");
                
            }
#endif
            break;
        }
        default:
            ft_printf(err, "Error in evaluate\n");
        }
    return NULL;
}


int main(int argc, char **argv, char **envp)
{
    global.env = envp;

    global.addresses.len = 100;
    global.addresses.size = sizeof(void**);
    global.addresses.pointer = malloc(global.addresses.len * global.addresses.size);

    new_list(&global.envirement, sizeof(Node*));

    char *text;
    int i = 0;
    while (envp && envp[i])
    {
        if(ft_strncmp("PATH", envp[i], ft_strlen("PATH")) == 0)
        {
            ft_printf(out, "found PATH\n");
            global.path = split(envp[i], ":");
            break;
        }
        i++;
    }
    build_envirement(envp);
    
    while(1)
    {
#if 1
        new_list(&global.tokens, sizeof(Token*));
        new_list(&global.files, sizeof(File*));
        new_list(&global.pids, sizeof(int*));
        new_list(&global.pipes, sizeof(int*));

        File *input = new_file(NULL, in, 0);
        File *output = new_file(NULL, out, 0);

        text = readline("minishell $> ");
        // printf("text: %p\n", text);
        if(text)
            add_history(text);

        build_tokens(text);
        global.tokens.pos = 0;

        evaluate(expr(), *input, *output);
        
        free(text);
        text = NULL;

        int status;
        while(--global.pids.pos >= 0)
        {
            int *pid_ptr = global.pids.pointer[global.pids.pos];
            waitpid(*pid_ptr, &status, 0);
            ft_printf(out,"received status: %d\n", status);
        }
        free_list(&global.tokens);
        free_list(&global.pids);
        free_list(&global.files);
        free_list(&global.pipes);
#endif
    }
}