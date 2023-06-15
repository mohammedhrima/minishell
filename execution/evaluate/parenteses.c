#include "evaluate.h"



void open_parenteses(t_node *node, t_file *input, t_file *output)
{
    int pid;
    t_token *token;
    int pos;
    t_type type;
    int i;
    char *filename;
    t_value *value;
    int status;

    pid = fork();
    add_number(&global.pids, pid);
    if (pid == 0)
    {
        pos = node->token->start;
        i = 0;
        while (i < node->token->len)
        {
            token = ((t_token **)global.tokens.pointers)[pos];
            pos++;
            if (is_redirection(token->type))
            {
                type = token->type;
                token = ((t_token **)global.tokens.pointers)[pos];
                if (token->type == star_)
                {
                    printf("minishell: * ambiguous redirection\n");
                    ft_exit(AMBIGIOUS_REDIRECTION);
                }
                else if (token->type != identifier_)
                {
                    printf("minishell: Unexpected symbol after redirection\n");
                    ft_exit(UNEXPECTED_ERROR);
                }
                pos++;
                i++;
                filename = expand(token->value);
                if (type == redir_input)
                    input = new_file(filename, NOT_OPENED, type);
                if (type == redir_output || type == append_)
                    output = new_file(filename, NOT_OPENED, type);
                if (type == heredoc_)
                    input = open_heredoc(filename, input);
            }
            i++;
        }
        if (output->type == redir_output)
        {
            open_file(output);
            write(output->fd, "", ft_strlen(""));
            close(output->fd);
            output->type = append_;
            output->fd = NOT_OPENED;
        }
        value = evaluate(node->left, input, output);
        while (global.pids.pos > 0)
            status = get_last_exit_code();
        ft_exit(SUCCESS);
    }
    waitpid(pid, NULL, 0);
}
