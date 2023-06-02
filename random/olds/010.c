#include "header.h"

struct
{
    char *value;
    Type type;
} symbol_lexic[] = {
    {"<<", heredoc_}, {">>", append_}, {"<", redir_input}, 
    {">", redir_output}, {"&&", and_}, {"||", or_}, 
    {"(", lparent_}, {")", rparent_}, {"|", pipe_}, 
    {"*", star_beg}, {0, 0}};

struct
{
    char *value;
    Type type;
} alpha_lexic[] = {
    {"echo", echo_}, {"cd", cd_}, {"pwd", pwd_}, 
    {"export", export_}, {"unset", unset_}, 
    {"env", env_}, {"exit", exit_}, {0, 0}};

List *new_list(List *list, size_t size, Type type)
{
    list->pos = 0;
    list->len = 100;
    list->type = type;
    list->size = size;

    if(list->type == pointers_)
        list->pointers = ft_calloc(list->len, list->size);
    if(list->type == integers_)
        list->integers = ft_calloc(list->len, list->size);

    return list;
}

void clear_list(List *list)
{
    Type type = list->type;
    if (type == integers_)
        ft_memset(list->integers, 0, list->len * list->size);
    else if (type == pointers_)
        ft_memset(list->pointers, 0, list->len * list->size);
    else
        ft_printf(err, " \033[0;31mError: Received unknown type to clear\n\033[0m");
    list->pos = 0;
}

Token *new_token(Type type, char *source, int len)
{
    Token *new = ft_calloc(1, sizeof(Token));
    new->type = type;
    
    ft_printf(out, "new token with type %12t ", new->type);
    if(source)
    {
        new->value = ft_calloc(len + 1, sizeof(char));
        ft_strncpy(new->value, source, len);        
        ft_printf(out, "has value: '%k'", new);
    }
    ft_printf(out, "\n");

    return new;
};

void add_to_list(List *list, void *value)
{
    if(list->type == integers_)
        list->integers[list->pos++] = *((int *)value);
    if(list->type == pointers_)
        list->pointers[list->pos++] = value;

    if(list->pos + 10 > list->len)
    {
        list->len *= 2;
        if(list->type == integers_)
            list->integers = ft_realloc(list->integers, list->pos * list->size, list->len * list->size);
        if(list->type == pointers_)
            list->pointers = ft_realloc(list->pointers, list->pos * list->size, list->len * list->size);
    }
}

void add_to_addresses(void *pointer)
{
    if (global.addresses.len == 0)
    {
        global.addresses.pos = 0;
        global.addresses.len = 100;
        global.addresses.pointers = malloc(global.addresses.len * sizeof(void *));
    }
    global.addresses.pointers[global.addresses.pos++] = pointer;
    if (global.addresses.pos + 10 > global.addresses.len)
    {
        global.addresses.len *= 2;
        void *temporary = malloc(global.addresses.len * sizeof(void *));
        ft_memcpy(temporary, global.addresses.pointers, global.addresses.pos * sizeof(void *));
        free(global.addresses.pointers);
        global.addresses.pointers = temporary;
    }
}

void build_envirement(char **envp)
{
    int i = 0;
    while(envp && envp[i])
    {
        char **array = split_by_two(envp[i], '='); // split by only two, because value my contains '='
        Node *node = new_node(new_token(assign_, NULL, 0));
        node->left = new_node(new_token(identifier_, NULL, 0));
        node->left->token->value = array[0];
        node->right = new_node(new_token(identifier_, NULL, 0));
        node->right->token->value = array[1];
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
    // Token *token;

    while(text && text[txt_pos])
    {
        single_quotes = 0;
        double_quotes = 0;
        
        if(text[txt_pos] == '\0')
            break;
        if (text[txt_pos] == '*')
        {
            add_to_list(&global.tokens, new_token(star_beg, text + txt_pos, 1));
            txt_pos++;
            char *new = getcwd(NULL, 0);
            DIR *curr_dir = opendir(new);
            free(new);
            struct dirent *curr_dir_content;
            curr_dir_content = readdir(curr_dir);
            while (curr_dir_content)
            {
                add_to_list(&global.tokens, new_token(identifier_, curr_dir_content->d_name, ft_strlen(curr_dir_content->d_name)));
                curr_dir_content = readdir(curr_dir);
            }
            add_to_list(&global.tokens, new_token(star_end, NULL, 0));
            closedir(curr_dir);
            continue;
        }
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
            add_to_list(&global.tokens, new_token(type, text + start, txt_pos - start));
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
        add_to_list(&global.tokens, new_token(identifier_, text + start, txt_pos - start));

    }
    add_to_list(&global.tokens, new_token(end_, NULL, 0));
}

// Execution
Node *new_node(Token *token)
{
    Node *new = ft_calloc(1, sizeof(Node));
    new->token = token;
    return new;
}

void skip(Type type)
{
    Token *token = ((Token**)global.tokens.pointers)[global.tokens.pos];
    if(token->type != type)
        ft_printf(err, "Error Expected '%s'\n", type_to_string(type));
    global.tokens.pos++;
}

Node *expr()
{
    return and_or();
}

Node *and_or()
{
    Token *token = ((Token**)global.tokens.pointers)[global.tokens.pos];
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
    Node *left = prime();
    Token *token = ((Token**)global.tokens.pointers)[global.tokens.pos];
    while(token->type == pipe_)
    {
        Node *node = new_node(token);
        skip(pipe_);
        node->left = left;
        node->right = prime();
        left = node;
        token = ((Token**)global.tokens.pointers)[global.tokens.pos];
    }
    return left;
}

bool is_redirection(Type type)
{
    return(type == redir_input || type == redir_output || type == heredoc_ || type == append_ );
}

Node *prime()
{
    Token *token = ((Token**)global.tokens.pointers)[global.tokens.pos];

    if (token->type == identifier_ || is_redirection(token->type))
    {
        Node *node = new_node(token);
        node->token->start = global.tokens.pos;
        node->token->end++;
        skip(token->type);
        token = ((Token**)global.tokens.pointers)[global.tokens.pos];
        while(token->type == identifier_ || is_redirection(token->type)) // don't cahnge it will break redirection
        {
            node->token->end++;
            skip(token->type);
            token = ((Token**)global.tokens.pointers)[global.tokens.pos];
        }
        // token = ((Token**)global.tokens.pointers)[global.tokens.pos];
        if(token->type == star_beg)
        {
            Node *all = new_node(token);
            all->left = node;
            skip(token->type);
            all->token->start = global.tokens.pos;
            token = ((Token**)global.tokens.pointers)[global.tokens.pos];
            while(token->type != star_end)
            {
                skip(token->type);
                all->token->end++;
                token = ((Token**)global.tokens.pointers)[global.tokens.pos];
            }
            node = all;
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
    return NULL;
}

File new_file(char* name, int fd, Type type)
{
# if 0
    File *new = ft_calloc(1, sizeof(File));
    new->name = name;
    new->fd = fd;
    new->type = type;
    add_integer_to_list(&global.fds, new->fd);
    return *new;
#else
    File new;
    new.name = name;
    new.fd = fd;
    new.type = type;
    add_to_list(&global.fds, &new.fd);
    return new;
#endif 
}

char *get_command_path(char *cmd)
{
    if(cmd == NULL || ft_strchr(cmd, '/'))
        return cmd;
    int i = 0;
    while(global.path && global.path[i])
    {
        char *res = strjoin(global.path[i], "/", cmd);
        if (access(res, F_OK) == 0 && access(res, X_OK) == 0)
            return (res);
        i++;
    }
    return cmd;
}

// built in functions
void echo_func(int fd, char **arguments)
{
    int print_new_line = 1;
    int i = 0;
    while(arguments && arguments[i])
    {
        if(i == 0 && ft_strcmp(arguments[i], "-n") == 0)
            print_new_line = 0;
        else
            ft_printf(fd, "%s", arguments[i]);
        i++;
    }
    if(print_new_line)
        ft_printf(fd, "\n");
}

void cd_func(int fd, char **arguments)
{
    if(arguments[0])
        chdir(arguments[0]); // verify if this little shit leaks
    else
    {
# if 0
    chdir(getenv("HOME"));
#else
        int i = 0;
        char *location = "/";
        Node **envirement = (Node**)global.envirement.pointers;
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
        chdir(location); // verify if this little shit leaks
#endif
    }
}

void pwd_func(int fd, char **arguments)
{
    char *new = getcwd(NULL, 0);
    ft_printf(fd, "%s\n", new);
    free(new);
}

void export_func(int fd, char **arguments)
{
    if(arguments[0])
    {
        char **array = split_by_two(arguments[0], '='); // split by only two, because value my contains '='
        Node *node = new_node(new_token(assign_, NULL, 0));
        node->left = new_node(new_token(identifier_, NULL, 0));
        node->left->token->value = array[0];
        node->right = new_node(new_token(identifier_, NULL, 0));
        node->right->token->value = array[1];
        add_to_list(&global.envirement, node);
        return;
    }
    else
    {
        int i = 0;
        Node **envirement = (Node**)global.envirement.pointers;
        while (envirement && envirement[i])
        {
            if(envirement[i]->left->token->value)
                ft_printf(fd ,"declare -x %s=\"%s\"\n", envirement[i]->left->token->value, envirement[i]->right->token->value);
            i++;
        }
    }
}

void unset_func(int fd, char **argument)
{
    int i = 0;
    Node **envirement = (Node**)global.envirement.pointers;
    while (envirement && envirement[i])
    {
        char *value = envirement[i]->left->token->value;
        if(value && ft_strcmp(value, argument[0]) == 0)
        {
            free(envirement[i]->left->token->value);
            envirement[i]->left->token->value = NULL;
            free(envirement[i]->right->token->value);
            envirement[i]->right->token->value = NULL;
            break;
        }
        i++;
    }
}

void env_func(int fd, char **arguments)
{
    int i = 0;
    Node **envirement = (Node**)global.envirement.pointers;
    while(envirement[i])
    {
        if(envirement[i]->left->token->value)
            ft_printf(fd, "%s=%s\n", envirement[i]->left->token->value, envirement[i]->right->token->value);
        i++;
    }
}

void exit_func(int fd, char **arguments)
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
    Node **nodes = (Node **)global.envirement.pointers;
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
    // if(file->fd == NOT_OPENED && file->name == NULL)
    // {    
    //     ft_printf(out, "Error need a file name\n");
    //     exit(-700);
    // }
    
    if(file->type == redir_input)
    {
        ft_putstr(out, "redir input\n");
        if(file->fd == NOT_OPENED)
        {
            if(access(file->name, F_OK)) // check existance
            {    
                ft_printf(err, "minishell: %s: No such file or directory\n", file->name);
                ft_exit(NO_SUCH_FILE_OR_DIR);
            }
            else if(access(file->name, R_OK)) // check if readable
            {
                ft_printf(err, "minishell: %s: Permission denied\n", file->name);
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
                ft_printf(err, "minishell: %s: Permission denied\n", file->name);
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
            ft_printf(err, "minishell: %s: Permission denied\n", file->name);
            ft_exit(PERMISSION_DENIED);
        }
        else
        {
            // verify access 0644 or 0777 // to check after
            file->fd = open(file->name, O_WRONLY | O_CREAT | O_APPEND);
        }
         
    }
}

int get_last_exit_code()
{
    int *pids = global.pids.integers;
    static int res;
    if(global.pids.pos == 0)
        return (res);
    int status = 0;
    waitpid(pids[--global.pids.pos], &status, 0);
    res = WEXITSTATUS(status);
    return(res);
}

Value *evaluate(Node *node, File input, File output)
{
    switch(node->token->type)
    {
        case identifier_:
        case redir_input: // verify if need to be removed
        case redir_output:
        case heredoc_: 
        case append_:
        {
            char **arguments = NULL;
            int len = 0;
            int pos = node->token->start;
            int i = 0;
            while(pos < node->token->end)
            {
                Token *token =((Token**)global.tokens.pointers)[pos];
                pos++;
                if (token->type == identifier_)
                {
                    arguments = ft_realloc(arguments, len * sizeof(char*), (len + 2) * sizeof(char*));
                    arguments[len] = expand(token);  
                    len++;
                    arguments[len] = NULL;
                }
                else if(is_redirection(token->type))
                {
                    Type type = token->type;
                    // i++;
                    // pos++;
                    token = ((Token**)global.tokens.pointers)[pos];
                    pos++;
                    char *filename = expand(token);
                    ft_printf(out, "is redirection type %s, has filename %s\n\n", type_to_string(type), filename);
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
                        // print erro if filename is NULL;
                        // ft_printf(out, "heredoc\n");
                        char *delimiter = filename;
                        input =  new_file(ft_strdup("/tmp/heredoc"), open("/tmp/heredoc", O_WRONLY | O_CREAT | O_TRUNC, 0644), redir_output);
                        // global.inside_heredoc++;
                        int pid = fork();
                        if(pid == 0)
                        {
                            signal(SIGINT, handle_heredoc_signal);
                            signal(SIGQUIT, handle_heredoc_signal);
                            char *res = NULL;
                            while((res == NULL || ft_strcmp(res, delimiter)))
                            {
                                res = readline("heredoc $> ");
                                if(res)
                                    write(input.fd, res, ft_strlen(res));
                            }
                            exit(SUCCESS);
                        }
                        // signal(SIGINT, handle_signal);
                        // signal(SIGQUIT, handle_signal);
                        int status;
                        waitpid(pid, &status , 0);
                        // global.inside_heredoc--;
                        close(input.fd);
                        input.type = redir_input;
                        input.fd = NOT_OPENED;
                    }
                }
                // i++;
            }
            if(arguments && arguments[0])
            {
                ft_printf(out,"\n1. input type: \'%s\'\n1. output type: \'%s\'\n\n", type_to_string(input.type), type_to_string(output.type));
                // built in function
                void (*func)(int, char**) = built_in(arguments[0]);
                if(func)
                {
                    // if inside_pipe fork
                    if(global.inside_pipe)
                    {
                        // ft_printf(out, "built in function, with forking\n");
                        int pid = fork();
                        add_to_list(&global.pids, &pid);

                        if (!pid)
                        {  
                            int *fds = global.fds.integers;
                            int i = 0;
                            while(i < global.fds.pos)
                            {
                                if(fds[i] != input.fd && fds[i] != output.fd && fds[i] != out && fds[i] != in)
                                    close(fds[i]);
                                i++;
                            }
                            if(input.type == redir_input)
                            {
                                open_file(&input);
                                dup2(input.fd, in);
                                close(input.fd);
                            }
                            if(output.type == redir_output || output.type == append_)
                            {
                                open_file(&output);
                                dup2(output.fd, out);
                                close(output.fd);
                            }                       
                            func(out, &arguments[1]);
                            exit(SUCCESS);
                        }
                    }
                    else
                    {
                        // ft_printf(out, "built in function, without forking\n");
                        open_file(&output);
                        func(output.fd ,&arguments[1]);
                        int pid = fork();
                        add_to_list(&global.pids, &pid);
                        if(pid == 0)
                        {
                            int *fds = global.fds.integers;
                            int i = 0;
                            while(i < global.fds.pos)
                            {
                                close(fds[i]);
                                i++;
                            }
                            exit(SUCCESS);
                        }
                    }
                }
                // executable
                else
                {
                    ft_printf(out,"2. input type: \'%s\'\n2. output type: \'%s\'\n\n", type_to_string(input.type), type_to_string(output.type));
                    char *full_command = get_command_path(arguments[0]);
                    int pid = fork();
                    add_to_list(&global.pids, &pid);

                    if (pid == 0)
                    {  
                        if(input.type == redir_input)
                        {
                            ft_printf(out, "call redirect input\n");
                            open_file(&input);
                            dup2(input.fd, in);
                            // close(input.fd);
                        }
                        if(output.type == redir_output || output.type == append_)
                        {
                            ft_printf(out, "call redirect output\n");
                            open_file(&output);
                            dup2(output.fd, out);
                            // close(output.fd);
                        }
                        // ft_printf(out, "full command is %s\n    has  input: %F\n    has output: %F\n", full_command, input, output);
                        int *fds = global.fds.integers;
                        int i = 0;
                        while(i < global.fds.pos)
                        {
                            if(fds[i] != out && fds[i] != in)
                                close(fds[i]);
                            i++;
                        }
                        // check exit code if command not valid
                        if(execve(full_command, &arguments[0], global.env) < 0)
                            exit(COMMAND_NOT_FOUND);
                    }
                }                
            }
            break;
        }
        case pipe_:
        {
            // ft_printf(out, "open pipe\n");
            int fd[2];

            if(pipe(fd) < 0 )
                ft_printf(err, "Error opening pipe\n");
            
            add_to_list(&global.fds, &fd[0]);
            add_to_list(&global.fds, &fd[1]);

            global.inside_pipe++;
            evaluate(node->left, input, new_file(NULL, fd[1], redir_output));

            evaluate(node->right, new_file(NULL, fd[0], redir_input), output);
            global.inside_pipe--;

            // to be checked
            int *fds = global.fds.integers;
            close(fds[--global.fds.pos]);
            close(fds[--global.fds.pos]);

            break;
        }
        case and_:
        case or_:
        {
            int status = get_last_exit_code();

            if(node->token->type == and_ && status == 0)
                return evaluate(node->left, input, output);
            if(node->token->type == and_ && status != 0)
            {
                int pid = fork();
                add_to_list(&global.pids, &pid);
                if(pid == 0)
                {
                    int *fds = global.fds.integers;
                    int i = 0;
                    while(i < global.fds.pos)
                    {
                        close(fds[i]);
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
                add_to_list(&global.pids, &pid);
                if(pid == 0)
                {
                    int *fds = global.fds.integers;
                    int i = 0;
                    while(i < global.fds.pos)
                    {
                        close(fds[i]);
                        i++;
                    }
                    exit(status);
                }
                return NULL;
            }
            break;
        }
        default:
            ft_printf(err, "Error in evaluate\n");
        }
    return NULL;
}

/*

ctrl-C displays a new prompt on a new line. -> 2
ctrl-D exits the shell.
ctrl-\ does nothing. -> 3

*/

void handle_signal(int signum) 
{
    if(signum == CTRL_C )
    {
        ft_putstr(out, "\n");
        rl_replace_line("", 0);
        rl_on_new_line();
        rl_redisplay();
    }
    if(signum == CTRL_SLASH)
    {
        ft_putstr(out, "");
        rl_replace_line("", 0);
        rl_on_new_line();
        rl_redisplay();
    }
   
}

void handle_heredoc_signal(int signum)
{
    if(signum == CTRL_C)
    {
        ft_exit(SUCCESS);
    }
    if(signum == CTRL_SLASH)
    {
        ft_putstr(out, "");
        rl_replace_line("", 0);
        rl_on_new_line();
        rl_redisplay();
    }
}

int main(int argc, char **argv, char **envp)
{
    // global.env = envp;
#if 0
    global.addresses.len = 100;
    global.addresses.size = sizeof(void**);
    global.addresses.type = pointers_;
    global.addresses.pointer = malloc(global.addresses.len * global.addresses.size);
#endif
    new_list(&global.envirement, sizeof(Node*), pointers_);
    new_list(&global.tokens, sizeof(Token*), pointers_);
    new_list(&global.pids, sizeof(int), integers_);
    new_list(&global.fds, sizeof(int), integers_);

    char *text;
    // text = NULL;
    int i = 0;
    while (envp && envp[i])
    {
        if(ft_strncmp("PATH", envp[i], ft_strlen("PATH")) == 0)
        {
            global.path = split(envp[i], ":");
            break;
        }
        i++;
    }
    build_envirement(envp);
    clear_list(&global.tokens);

#if 1
    while(1)
    {
        signal(SIGINT, handle_signal);
        signal(SIGQUIT, handle_signal);
        
        File input = new_file(NULL, in, 0);
        File output = new_file(NULL, out, 0);

        text = readline("minishell $> ");

        if(text)
            add_history(text);
        
        build_tokens(text);
        // exit(0);
        global.tokens.pos = 0;

        Token **tokens =  (Token**)global.tokens.pointers;

        while(tokens[global.tokens.pos]->type != end_)
            evaluate(expr(), input, output);
        
        int status;
        while(global.pids.pos > 0)
            status = get_last_exit_code();
        clear_list(&global.tokens);
        clear_list(&global.pids);
        clear_list(&global.fds);
        if(text == NULL)
            break; // exit if text is NULL
        free(text);
        text = NULL;
    }
#endif
    // free pointers here;
}