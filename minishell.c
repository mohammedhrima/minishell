#include "header.h"


// exit codes
#define SUCCESS 0
#define NO_SUCH_FILE_OR_DIR 1
#define PERMISSION_DENIED 1
#define NOT_OPENED -200
#define COMMAND_NOT_FOUND 127

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

List *new_list(List *list, size_t size, Type type)
{
    list->pos = 0;
    list->len = 100;
    list->type = type;
    list->size = size;
    if(type == integers_)
        list->integers = ft_calloc(list->len, size);
    if(type == pointers_)
        list->pointer = ft_calloc(list->len, size);
    return list;
}


void free_list(List *list)
{
    // if get error use ft_memeset 0
    if(list->type == integers_)
        free(list->integers);
    if(list->type == pointers_)
    {
        int i = 0;
        while(i < list->pos)
        {
            free(((void**)list->pointer)[i]);
            i++;
        }
        free(list->pointer);
        list->size = 0;
    }
    list->pos = 0;
    list->len = 0;
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
    add_pointer_to_list(&global.tokens, new);

    return new;
};

void add_integer_to_list(List *list, int number)
{
    list->integers[list->pos++] = number;
    
    if(list->pos + 10 > list->len)
    {
        list->len *= 2;
        void *temporary = ft_calloc(list->len, list->size);

        ft_memcpy(temporary, list->integers, list->pos * list->size);
        free(list->integers);
        list->integers = temporary;
    }
}

void add_pointer_to_list(List *list, void *pointer)
{
    list->pointer[list->pos++] = pointer;
    
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
#if 0
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
#else
        char **array = split_by_two(envp[i], '='); // split by only two, to be verified after
        Node *node = new_node(new_token(assign_, NULL, 0));
        node->left = new_node(new_token(identifier_, array[0], ft_strlen(array[0])));
        node->right = new_node(new_token(identifier_, array[1], ft_strlen(array[1])));
        add_pointer_to_list(&global.envirement, node);
#endif
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
    if(new->token)
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
    return and_or();
}

Node *and_or()
{
    ft_printf(out, "call and or\n");    
    Token *token = (Token*)global.tokens.pointer[global.tokens.pos];
    if (token->type == or_)
    {
        Node *node = new_node(token);
        skip(token->type);
        node->left = pipe_node();
        return node;
    }
    if (token->type == and_)
    {
        Node *node = new_node(token);
        skip(token->type);
        node->left = pipe_node();
        return node;
    }
    return (pipe_node());
}

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

File new_file(char* name, int fd, Type type)
{
    File *new = ft_calloc(1, sizeof(File));
    new->name = name;
    new->fd = fd;
    new->type = type;

    add_pointer_to_list(&global.files, new);
    return *new;
}

// Execution
char *get_command(char *cmd)
{
    ft_printf(out, "\ncall get command to find %s\n", cmd);
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
    ft_printf(out, "command not found\n");
    return cmd;
}

// built in functions
void echo_func(File file, char **arguments)
{

    int i = 0;
    while(arguments && arguments[i])
    {
        // ft_putstr(out, arguments[i]);
        ft_printf(file.fd ,"%s ", arguments[i]);
        i++;
    }
}
void cd_func(File file, char **arguments)
{
    if(arguments[0])
        chdir(arguments[0]);
    else
    {
        int i = 0;
        char *location = "/";
        Node **envirement = (Node**)global.envirement.pointer;
        while (envirement && envirement[i])
        {
            char *value = envirement[i]->left->token->value;
            if(value && ft_strcmp(value, "HOME") == 0)
            {
                location = envirement[i]->right->token->value;
                break;
            }
            i++;
        }
        chdir(location);
    }
}

void pwd_func(File file, char **arguments)
{
    char *new = getcwd(NULL, 0);
    ft_printf(out, "%s\n", new);
    free(new);
}

void export_func(File file, char **arguments)
{
    if(arguments[0])
    {
        char **array = split(arguments[0], "=");
        Node *node = new_node(new_token(assign_, NULL, 0));
        node->left = new_node(new_token(identifier_, array[0], ft_strlen(array[0])));
        node->right = new_node(new_token(identifier_, array[1], ft_strlen(array[1])));

        ft_printf(out, "+ new node with type: %t \n", node->token->type);
        ft_printf(out, "           has  left: %k\n", node->left->token);
        ft_printf(out, "           has right: %k\n\n", node->right->token);

        return;
    }
    else
    {
        int i = 0;
        Node **envirement = (Node**)global.envirement.pointer;
        while (envirement && envirement[i])
        {
            if(envirement[i]->left->token->value)
                printf("declare -x %s=\"%s\"\n", envirement[i]->left->token->value, envirement[i]->right->token->value);
            i++;
        }
    }
}

void unset_func(File file, char **argument)
{
    ft_printf(out, "in unset search for '%s'\n", argument[0]);
    // to be handled after
    int i = 0;
    Node **envirement = (Node**)global.envirement.pointer;
    while (envirement && envirement[i])
    {
        char *value = envirement[i]->left->token->value;
        if(value && ft_strcmp(value, argument[0]) == 0)
        {
            ft_printf(out, "found in unset\n");
            envirement[i]->left->token->value = NULL;
            break;
        }
        i++;
    }
}
void env_func(File file, char **arguments)
{
    int i = 0;
    Node **envirement = (Node**)global.envirement.pointer;
    while(envirement[i])
    {
        // print only those who have right token
        if(envirement[i]->left->token->value)
            printf("%s=%s\n", envirement[i]->left->token->value, envirement[i]->right->token->value);
        i++;
    }
}
void exit_func(File file, char **arguments)
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
    ft_printf(out, "Enter get_var, search for '%s'\n", name);
    Node **nodes = (Node **)global.envirement.pointer;
    if(ft_strcmp(name, "?") == 0)
    {
        return ft_itoa(get_last_exit_code());
    }
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

void open_file(File *file)
{
    if(file->fd == NOT_OPENED && file->name == NULL)
    {    
        ft_printf(out, "Error need a file name\n");
        exit(-700);
    }
    
    if(file->type == redir_input)
    {
        ft_putstr(out, "redir input\n");
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
        ft_putstr(out, "redir output\n");
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
                file->fd = open(file->name,  O_WRONLY | O_CREAT | O_TRUNC);
            }
        }   
    }
    if(file->type == append_)
    {
        ft_putstr(out, "redir append\n");
        if(access(file->name, F_OK) == 0 &&  access(file->name, W_OK)) // check if readable
        {
            ft_printf(out, "minishell: %s: Permission denied\n", file->name);
            ft_exit(PERMISSION_DENIED);
        }
        else
        {
            // ft_printf(out, "hey in duplacate");
            // verify access 0644 or 0777 // to check after
            file->fd = open(file->name, O_WRONLY | O_CREAT | O_APPEND);
        }
         
    }
    // dup2(file->fd, old);
}

int get_last_exit_code()
{
    printf("\ncall get last exit code\n");
    int *pids = global.pids.integers;
    static int res;
    if(global.pids.pos == 0)
    {
        printf("Error there is no child processes\n");
        // exit(-1);
        return (res);
    }
    int status = 0;
    waitpid(pids[--global.pids.pos], &status, 0);
    res = WEXITSTATUS(status);
    return(res);
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
        case append_:
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
                    token->type == heredoc_     || 
                    token->type == append_
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
                        input = new_file(filename, NOT_OPENED, type);
                    }
                    if(type == redir_output || type == append_)
                    {
                        /*
                            be carefull here because maybe you will
                            get back to stdout 
                        */
                        output = new_file(filename, NOT_OPENED, type);
                    }
                    if(type == heredoc_)
                    {
                        ft_printf(out, "heredoc\n");
                        char *delimiter = filename;
                        filename = ft_strdup("/tmp/heredoc");
                        
                        input =  new_file(filename, open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644), type);
                        char *res = NULL;
                        while(res == NULL || ft_strcmp(res, delimiter))
                        {
                            if(res)
                                write(input.fd, res, ft_strlen(res));
                            res = readline("heredoc $> ");
                        }
                        close(input.fd);
                        input.type = redir_input;
                        input =  new_file(filename, NOT_OPENED, type);
                    }
                }
                pos++;
                i++;
            }
            if(arguments && arguments[0])
            {
                // built in function
                void (*func)(File, char**) = built_in(arguments[0]);
                if(func)
                {
                    // if inside_pipe fork
                    if(global.inside_pipe)
                    {
                        ft_printf(out, "built in function, with forking\n");
                        int pid = fork();
                        add_integer_to_list(&global.pids, pid);

                        node->token->type = pid_;
                        node->token->process_id = pid;
                        if (!pid)
                        {  
                            int *pipes = global.pipes.integers;
                            int i = 0;
                            while(i < global.pipes.pos)
                            {
                                if(pipes[i] != input.fd && pipes[i] != output.fd && pipes[i] != out && pipes[i] != in)
                                    close(pipes[i]);
                                i++;
                            }
                            if(input.type == redir_input)
                            {
                                // ft_putstr(out, "redir input\n");
                                open_file(&input);
                                dup2(input.fd, in);
                                close(input.fd);
                            }
                            if(output.type == redir_output || output.type == append_)
                            {
                                // ft_putstr(out, "redir output\n");
                                open_file(&output);
                                dup2(output.fd, out);
                                close(output.fd);
                            }
                            
                            func(output ,&arguments[1]); // does not make senec, caus you already did dup2, handle it
                            exit(SUCCESS);
                        }
                    }
                    else
                    {
                        ft_printf(out, "built in function, without forking\n");
                        open_file(&output);
                        func(output ,&arguments[1]);
                        int pid = fork();
                        add_integer_to_list(&global.pids, pid);
                        if(pid == 0)
                        {
                            int *pipes = global.pipes.integers;
                            int i = 0;
                            while(i < global.pipes.pos)
                            {
                                close(pipes[i]);
                                i++;
                            }
                            // close(input.fd);
                            // close(output.fd);
                            exit(SUCCESS);
                        }
                    }
                }
                // executable
                else
                {
                    char *full_command = get_command(arguments[0]);
                    ft_printf(out, "full command is %s\n    has  input: %F\n    has output: %F\n", full_command, input, output);
                
                    int pid = fork();
                    add_integer_to_list(&global.pids, pid);

                    node->token->type = pid_;
                    node->token->process_id = pid;

                    if (pid == 0)
                    {  
                        int *pipes = global.pipes.integers;
                        int i = 0;
                        while(i < global.pipes.pos)
                        {
                            if(pipes[i] != input.fd && pipes[i] != output.fd && pipes[i] != out && pipes[i] != in)
                                close(pipes[i]);
                            i++;
                        }
                        if(input.type == redir_input)
                        {
                            // ft_putstr(out, "redir input\n");
                            open_file(&input);
                            dup2(input.fd, in);
                            close(input.fd);
                        }
                        if(output.type == redir_output || output.type == append_)
                        {
                            // ft_putstr(out, "redir output\n");
                            open_file(&output);
                            dup2(output.fd, out);
                            close(output.fd);
                        }
                        // check exit code if command not valid
                        if(execve(full_command, arguments, global.env) < 0)
                        {
                            // ft_printf(err, "Execve failed\n");
                            ft_printf(out, "command not found\n");
                            exit(COMMAND_NOT_FOUND);
                        }
                    }
                }                
            }
            return node->token;
        }
        case pipe_:
        {
            ft_printf(out, "open pipe\n");
            // if there is no right output will be set to out

            int fd[2];

            if(pipe(fd) < 0 )
                ft_printf(err, "Error opening pipe\n");
            
            add_integer_to_list(&global.pipes, fd[0]);
            add_integer_to_list(&global.pipes, fd[1]);
            
            global.inside_pipe++;
            evaluate(node->left, input, new_file(NULL, fd[1], redir_output));

            evaluate(node->right, new_file(NULL, fd[0], redir_input), output);
            global.inside_pipe--;

            int *pipes = global.pipes.integers; 

            global.pipes.pos--;
            close(pipes[global.pipes.pos]);
            global.pipes.pos--;
            close(pipes[global.pipes.pos]);

            break;
        }
        case and_:
        case or_:
        {
            int status = get_last_exit_code();
            printf("last exit code is %d\n", status);
            // in case && if status != 0, add a number to exit code list
            if(node->token->type == and_ && status == 0)
                return evaluate(node->left, input, output);
            if(node->token->type == and_ && status != 0)
            {
                int pid = fork();
                add_integer_to_list(&global.pids, pid);
                if(pid == 0)
                {
                    int *pipes = global.pipes.integers;
                    int i = 0;
                    while(i < global.pipes.pos)
                    {
                        close(pipes[i]);
                        i++;
                    }
                    exit(status);
                }
            }
            if(node->token->type == or_ && status != 0)
                return evaluate(node->left, input, output);
            if(node->token->type == or_ && status == 0)
            {
                int pid = fork();
                add_integer_to_list(&global.pids, pid);
                if(pid == 0)
                {
                    int *pipes = global.pipes.integers;
                    int i = 0;
                    while(i < global.pipes.pos)
                    {
                        close(pipes[i]);
                        i++;
                    }
                    exit(status);
                }
            }
            return NULL;
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
    global.addresses.type = pointers_;
    global.addresses.pointer = malloc(global.addresses.len * global.addresses.size);

    new_list(&global.envirement, sizeof(Node*), pointers_);
    new_list(&global.tokens, sizeof(Token*), pointers_);

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
    ft_memset(&global.tokens, 0, sizeof(List));

#if 1
    while(1)
    {
        new_list(&global.tokens, sizeof(Token*), pointers_);
        new_list(&global.files, sizeof(File*), pointers_);
        new_list(&global.pids, sizeof(int), integers_);
        new_list(&global.pipes, sizeof(int), integers_);

        File input = new_file(NULL, in, 0);
        File output = new_file(NULL, out, 0);

        text = readline("minishell $> ");
        if(text)
            add_history(text);

        build_tokens(text);
        global.tokens.pos = 0;
        Token **tokens =  (Token**)global.tokens.pointer;
        while(tokens[global.tokens.pos]->type != end_)
            evaluate(expr(), input, output);
        
        free(text);
        text = NULL;

        int status;
        while(global.pids.pos > 0)
        {
            // int pid = global.pids.integers[global.pids.pos];
            // waitpid(pid, &status, 0);
            status = get_last_exit_code();
            ft_printf(out,"received status: %d\n", status);
        }
        free_list(&global.tokens);
        free_list(&global.pids);
        free_list(&global.files);
        free_list(&global.pipes);
    }
#endif
}