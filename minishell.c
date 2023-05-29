#include "header.h"

struct
{
    char *value;
    Type type;
} symbol_lexic[] = {
    {"<<", heredoc_}, {">>", append_}, {"<", redir_input}, {">", redir_output}, {"&&", and_}, {"||", or_}, {"(", lparent_}, {")", rparent_}, {"|", pipe_}, {"*", star_beg}, {0, 0}};

struct
{
    char *value;
    Type type;
} alpha_lexic[] = {
    {"echo", echo_}, {"cd", cd_}, {"pwd", pwd_}, {"export", export_}, {"unset", unset_}, {"env", env_}, {"exit", exit_}, {0, 0}};

// new structs functions
List *new_list(List *list, size_t size, Type type)
{
    list->pos = 0;
    list->len = 100;
    list->type = type;
    list->size = size;
    if (list->type == pointers_)
        list->pointers = ft_calloc(list->len, list->size);
    if (list->type == integers_)
        list->integers = ft_calloc(list->len, list->size);

    return list;
}

Node *new_node(Token *token)
{
    Node *new = ft_calloc(1, sizeof(Node));
    new->token = token;
    return new;
}

Token *new_token(Type type, char *source, int len)
{
    Token *new = ft_calloc(1, sizeof(Token));
    new->type = type;

    // ft_printf(out, "new token with type %12t ", new->type);
    if (source)
    {
        new->value = ft_calloc(len + 1, sizeof(char));
        ft_strncpy(new->value, source, len);
        // ft_printf(out, "has value: '%k'", new);
    }
    // ft_printf(out, "\n");
    return new;
};

File new_file(char *name, int fd, Type type)
{
#if 0
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

void add_to_list(List *list, void *value)
{
    if (list->type == integers_)
        list->integers[list->pos++] = *((int *)value);
    if (list->type == pointers_)
        list->pointers[list->pos++] = value;

    if (list->pos + 10 > list->len)
    {
        list->len *= 2;
        if (list->type == integers_)
            list->integers = ft_realloc(list->integers, list->pos * list->size, list->len * list->size);
        if (list->type == pointers_)
            list->pointers = ft_realloc(list->pointers, list->pos * list->size, list->len * list->size);
    }
}

// clear list
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

void build_envirement(char **envp)
{
    int i = 0;
    while (envp && envp[i])
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

void handle_signal(int signum)
{
    if (signum == CTRL_C)
    {
        ft_putstr(out, "\n");
        rl_replace_line("", 0);
        rl_on_new_line();
        rl_redisplay();
    }
    if (signum == CTRL_SLASH)
    {
        ft_putstr(out, "");
        rl_replace_line("", 0);
        rl_on_new_line();
        rl_redisplay();
    }
}
void handle_heredoc_signal(int signum)
{
    if (signum == CTRL_C)
        ft_exit(SUCCESS);
    if (signum == CTRL_SLASH)
    {
        ft_putstr(out, "");
        rl_replace_line("", 0);
        rl_on_new_line();
        rl_redisplay();
    }
}

int is_inclosed(char *text)
{
    int quote = 0;
    int is_operator = 0;
    int i = 0;
    while (text && text[i])
    {
        if (text[i] == '\'' && quote == 0)
            quote = '\'';
        else if (text[i] == '"' && quote == 0)
            quote = '"';
        else if (text[i] == quote)
            quote = 0;
        if (ft_strchr("&|", text[i]))
            is_operator = 1;
        else if (text[i] && text[i] != '\n')
            is_operator = 0;
        i++;
    }
    if (quote || is_operator)
        return true;
    return false;
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

void build_tokens(char *text)
{
    int txt_pos = 0;
    int start = 0;
    int single_quotes = 0;
    int double_quotes = 0;

    while (text && text[txt_pos])
    {
        single_quotes = 0;
        double_quotes = 0;

        if (text[txt_pos] == '\0')
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
        if (text[txt_pos] == ' ')
        {
            start = txt_pos;
            while (text[txt_pos] == ' ')
                txt_pos++;
            continue;
        }
        int i = 0;
        Type type = 0;
        while (symbol_lexic[i].value)
        {
            if (ft_strncmp(symbol_lexic[i].value, &text[txt_pos], ft_strlen(symbol_lexic[i].value)) == 0)
            {
                start = txt_pos;
                type = symbol_lexic[i].type;
                txt_pos += ft_strlen(symbol_lexic[i].value);
                break;
            }
            i++;
        }
        if (type)
        {
            add_to_list(&global.tokens, new_token(type, text + start, txt_pos - start));
            continue;
        }
        start = txt_pos;
        while (text[txt_pos])
        {
            if (text[txt_pos] == '\'')
            {
                if (!double_quotes)
                    single_quotes = !single_quotes;
            }
            else if (text[txt_pos] == '"')
            {
                if (!single_quotes)
                    double_quotes = !double_quotes;
            }
            else
            {
                if (!single_quotes && !double_quotes && (ft_isspace(text[txt_pos]) || ft_strchr("|&<>", text[txt_pos])))
                    break;
            }
            txt_pos++;
        }
        add_to_list(&global.tokens, new_token(identifier_, text + start, txt_pos - start));
    }
    add_to_list(&global.tokens, new_token(end_, NULL, 0));
}

void skip(Type type)
{
    Token *token = ((Token **)global.tokens.pointers)[global.tokens.pos];
    if (token->type != type)
        ft_printf(err, "Error Expected '%s'\n", type_to_string(type));
    global.tokens.pos++;
}

Node *expr()
{
    return and_or();
}

Node *and_or()
{
    Token *token = ((Token **)global.tokens.pointers)[global.tokens.pos];
    if (token->type == or_)
    {
        Node *node = new_node(token);
        skip(or_);
        node->left = pipe_node();
        Node *res = node;

        token = ((Token **)global.tokens.pointers)[global.tokens.pos];
        while (token->type == or_)
        {
            node->right = new_node(token);
            skip(or_);
            node = node->right;
            node->left = pipe_node();
            token = ((Token **)global.tokens.pointers)[global.tokens.pos];
        }
        return res;
    }
    if (token->type == and_)
    {
        Node *node = new_node(token);
        skip(and_);
        node->left = pipe_node();
        Node *res = node;

        token = ((Token **)global.tokens.pointers)[global.tokens.pos];
        while (token->type == and_)
        {
            node->right = new_node(token);
            skip(and_);
            node = node->right;
            node->left = pipe_node();
            token = ((Token **)global.tokens.pointers)[global.tokens.pos];
        }
        return res;
    }
    return (pipe_node());
}

Node *pipe_node()
{
    Node *left = redirection();
    Token *token = ((Token **)global.tokens.pointers)[global.tokens.pos];
    while (token->type == pipe_)
    {
        Node *node = new_node(token);
        skip(pipe_);
        node->left = left;
        node->right = redirection();
        left = node;
        token = ((Token **)global.tokens.pointers)[global.tokens.pos];
    }
    return left;
}

bool is_redirection(Type type)
{
    return (type == redir_input || type == redir_output || type == heredoc_ || type == append_);
}

Node *redirection()
{
    Node *left = prime();
    Token *token = ((Token **)global.tokens.pointers)[global.tokens.pos];
    if (is_redirection(token->type))
    {
        Node *node = new_node(token);

        // node->left = left;
        node->token->start = global.tokens.pos;

        skip(token->type);

        Node *right = prime();
        if(right && right->token->type != identifier_)
        {
            if(right->token->type == star_beg)
            {
                ft_printf(out, "minishell: *: ambiguous redirect\n");
                ft_exit(AMBEGIOUS_REDIRECTION);
            }
            else
            {
                ft_printf(out, "minishell: Expected identifier after redirection symbol\n"); // this error message to be checked
                ft_exit(UNEXPECTED_ERROR);
            }
        }
        
        token = ((Token **)global.tokens.pointers)[global.tokens.pos];
        // check right type , must be identifier
        while (is_redirection(token->type))
        {
            // ft_printf(out, "is redirection\n");
            skip(token->type);
            prime(); // expect file or delimiter
            token = ((Token **)global.tokens.pointers)[global.tokens.pos];
        }

        node->token->end = global.tokens.pos;
        if(left)
        {
            left->right = node;
            return left;
        }
        return node;
    }
    return left;
}

Node *prime()
{
    Token *token = ((Token **)global.tokens.pointers)[global.tokens.pos];

    if (token->type == identifier_ || token->type == star_end || token->type == star_beg)
    {
        Node *node = new_node(token);
        node->token->start = global.tokens.pos;

        // don't change it will break redirection
        while(token->type == star_end || token->type == star_beg || token->type == identifier_)
        {
            skip(token->type);
            token = ((Token **)global.tokens.pointers)[global.tokens.pos];
        }
        node->token->end = global.tokens.pos;

        return node;
    }
    // TOFIX : parenteses must have append mode on output
    // when evaluating save first output redirection
    if (token->type == lparent_)
    {
        Node *node = new_node(token);
        skip(token->type);
        node->left = expr();
        skip(rparent_);
        return node;
    }
    return NULL;
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

char *expand(Token *token)
{
    char *res = NULL;
    int i = 0;
    int single_quotes = 0;
    int double_quotes = 0;

    char *value = token->value;
    while(value && value[i])
    {
        if(value[i] == '\'')
        {
            if(!double_quotes)
                single_quotes = !single_quotes;
            else
            {
                res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
                res[ft_strlen(res)] =  value[i];
            }
            i++;
        }
        else if(value[i] == '"')
        {
            if(!single_quotes)
                double_quotes = !double_quotes;
            else
            {
                res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
                res[ft_strlen(res)] =  value[i];
            }
            i++;
        }
        else
        {
            res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
            res[ft_strlen(res)] =  value[i];
            i++;
        }
    }
    return res;
}

int open_file(char *name, Type type)
{
    // don't exit if you are in main process
    int fd = NOT_OPENED;
    if(type == redir_input)
    {
        // ft_putstr(out, "redir input\n");
        if(access(name, F_OK)) // check existance
        {    
            ft_printf(err, "minishell: %s: No such file or directory\n", name);
            ft_exit(NO_SUCH_FILE_OR_DIR);
        }
        else if(access(name, R_OK)) // check if readable
        {
            ft_printf(err, "minishell: %s: Permission denied\n", name);
            ft_exit(PERMISSION_DENIED);
        }
        else
        {
            // verify access type
            fd = open(name, O_RDONLY);
            dup2(fd, in);
        }
        
    }
    if(type == redir_output)
    {
        // ft_putstr(out, "redir output\n");
        if(access(name, F_OK) == 0 &&  access(name, W_OK)) // check if readable
        {
            ft_printf(err, "minishell: %s: Permission denied\n", name);
            ft_exit(PERMISSION_DENIED);
        }
        else
        {
            // verify access 0644 or 0777 // to check after
            fd = open(name,  O_WRONLY | O_CREAT | O_TRUNC);
            dup2(fd, out);
        }
 
    }
    if(type == append_)
    {
        // ft_putstr(out, "redir append\n");
        if(access(name, F_OK) == 0 &&  access(name, W_OK)) // check if readable
        {
            ft_printf(err, "minishell: %s: Permission denied\n", name);
            ft_exit(PERMISSION_DENIED);
        }
        else
        {
            // verify access 0644 or 0777 // to check after
            fd = open(name, O_WRONLY | O_CREAT | O_APPEND);
            dup2(fd, in);
        }
         
    }
    return fd;
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
    // ft_printf(out, "Evaluate %k has type %t\n", node->token, node->token->type);
    switch (node->token->type)
    {
    case redir_input:
    case redir_output:
    case heredoc_:
    case append_:
    {
        // iterate and call expand
        int start = node->token->start;
        while (start < node->token->end)
        {
            Token *token = global.tokens.pointers[start];
            if(is_redirection(token->type))
            {
                // create file here
                start++;
                char *name = expand(global.tokens.pointers[start]);
                if(token->type == redir_input)
                {
                    int fd = open_file(name, token->type);
                    input = new_file(name, fd, token->type);
                }
                else if(token->type == redir_output || token->type == append_)
                {
                    int fd = open_file(name, token->type);
                    output = new_file(name, fd , token->type);
                }
                else if(token->type == heredoc_)
                {
                    int pid = fork();
                    if(pid == 0)
                    {
                        input =  new_file("/tmp/heredoc", open("/tmp/heredoc", O_WRONLY | O_CREAT | O_TRUNC), redir_output);
                        signal(SIGINT, handle_heredoc_signal);
                        signal(SIGQUIT, handle_heredoc_signal);
                        char *res = NULL;
                        while((res == NULL || ft_strcmp(res, name)))
                        {
                            if(res)
                            {    
                                write(input.fd, res, ft_strlen(res));
                                write(input.fd, "\n", sizeof(char));
                            }
                            res = readline("heredoc $>");
                        }
                        int i = 0;
                        while(i < global.fds.pos)
                        {
                            if(global.fds.integers[i] != in && global.fds.integers[i] != out)
                                close(global.fds.integers[i]);
                            i++;
                        }  
                        ft_exit(SUCCESS);
                    }
                    // must close inut file here or it will hang
                    int status;
                    waitpid(pid, &status , 0);
                    input = new_file("/tmp/heredoc" , open_file("/tmp/heredoc", redir_input), redir_input);
                }
            }
            start++;
        }
        ft_printf(out, "exit evaluate\n");
        break;
    }
    case identifier_:
    {
        // identifier must execute the command
        Value *curr = node->token;
        curr->value = expand(curr);
        int len = 0;
        char **arguments = ft_calloc(len++ + 1 , sizeof(char*));
        arguments[0] = get_command_path(curr->value);

        ft_printf(out, "Command is %s, has full path: %s, has arguments:\n", curr->value, get_command_path(curr->value));
        int start = node->token->start;
        while(start < node->token->end)
        {
            Token *token = global.tokens.pointers[start];
            arguments = ft_realloc(arguments, len * sizeof(char*), (len + 2) * sizeof(char*));
            arguments[len] = expand(token);
            ft_printf(out,"         %s\n", arguments[len]);
            start++;
            len++;
            arguments[len] = NULL;
        }
        // fork then check left->right, and evaluate it
        
        int pid = fork();
        add_to_list(&global.pids, &pid);

        if (pid == 0)
        {  
            // and dup2 there
            if(node->right)
            {
                evaluate(node->right, input, output);
            }
            int *fds = global.fds.integers;
            int i = 0;
            while(i < global.fds.pos)
            {
                if(fds[i] != out && fds[i] != in)
                    close(fds[i]);
                i++;
            }
            // check exit code if command not valid
            if(execve(arguments[0], &arguments[1], global.env) < 0)
                ft_exit(COMMAND_NOT_FOUND);
        }
        return curr;
        break;
    }
    case pipe_:
    {

        break;
    }
    case and_:
    case or_:
    {

        break;
    }
    default:
        ft_printf(err, "Error in evaluate\n");
    }
    return NULL;
}

int main(int argc, char **argv, char **envp)
{
    global.addresses.len = 0;
    global.env = envp;
    new_list(&global.envirement, sizeof(Node *), pointers_);
    new_list(&global.tokens, sizeof(Token *), pointers_);
    new_list(&global.pids, sizeof(int), integers_);
    new_list(&global.fds, sizeof(int), integers_);

    char *text;
    int i = 0;
    while (envp && envp[i])
    {
        if (ft_strncmp("PATH", envp[i], ft_strlen("PATH")) == 0)
        {
            global.path = split(envp[i], ":");
            break;
        }
        i++;
    }
    build_envirement(envp);
    clear_list(&global.tokens);
    while (1)
    {
        signal(SIGINT, handle_signal);
        signal(SIGQUIT, handle_signal);
       

        text = readline("minishell $> ");
        if (text == NULL)
            break;
        add_to_addresses(text);
        if (is_inclosed(text))
        {
            text = ft_realloc(text, ft_strlen(text), ft_strlen(text) + 2);
            text[ft_strlen(text)] = '\n';
            while (is_inclosed(text))
            {
                char *tmp = text;
                text = ft_readline("> ");
                tmp = ft_realloc(tmp, ft_strlen(tmp), ft_strlen(tmp) + ft_strlen(text) + 1);
                ft_strcpy(tmp + ft_strlen(tmp), text);
                text = tmp;
            }
            text[ft_strlen(text) - 1] = 0;
        }
        if (text)
            add_history(text);

        build_tokens(text);

        File input = new_file(NULL, in, 0);
        File output = new_file(NULL, out, 0);

        global.tokens.pos = 0;
        Token **tokens = (Token **)global.tokens.pointers;

        while(tokens[global.tokens.pos]->type != end_)
            evaluate(expr(), input, output);
        int i = 0;
        while(i < global.fds.pos)
        {
            if(global.fds.integers[i] != in && global.fds.integers[i] != out)
                close(global.fds.integers[i]);
            i++;
        }
        int status;
        while(global.pids.pos > 0)
            status = get_last_exit_code();
        
        clear_list(&global.tokens);
        clear_list(&global.pids);
        clear_list(&global.fds);
        // dup2(in, in);
        // dup2(out, out);
        // exit(0);
    }
    ft_exit(0);
}

