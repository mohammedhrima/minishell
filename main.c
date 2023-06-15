#include "header.h"

int main(int argc, char **argv, char **envp)
{
    char *text;
    t_file *input;
    t_file *output;
    int i;
    t_token **tokens;
    t_node *curr;

    global.values = (char *[]){"<<", ">>", "<", ">", "&&", "||", "(", ")", "|",
                               "*", 0};
    global.types = (t_type[]){heredoc_, append_, redir_input, redir_output,
                              and_, or_, lparent_, rparent_, pipe_, star_, 0};
    global.env = envp;
    new_list(&global.envirement, sizeof(t_node *), pointers_);
    new_list(&global.tokens, sizeof(t_token *), pointers_);
    new_list(&global.pids, sizeof(int), integers_);
    new_list(&global.fds, sizeof(int), integers_);
    i = 0;
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

    while (argc && argv)
    {
        signal(SIGINT, handle_signal);
        signal(SIGQUIT, handle_signal);

        input = new_file(NULL, IN, 0);
        output = new_file(NULL, OUT, 0);

        text = readline("minishell $> ");
        if (text && is_inclosed(text))
        {
            printf("minishell: syntax error\n");
            add_history(text);
            free(text);
            clear_list(&global.tokens);
            clear_list(&global.pids);
            clear_list(&global.fds);
            continue;
        }
        if (text == NULL)
            break;
        else
            add_history(text);
        if (build_tokens(text) != SUCCESS)
        {
            while (global.pids.pos > 0)
                get_last_exit_code();
            free(text);
            clear_list(&global.tokens);
            clear_list(&global.pids);
            clear_list(&global.fds);
            continue;
        }
        global.tokens.pos = 0;
        tokens = (t_token **)global.tokens.pointers;
        while (tokens[global.tokens.pos]->type != end_)
        {
            curr = expr();
            if(curr == NULL)
                break;
            evaluate(curr, input, output);
        }

        // wait for all child processes
        while (global.pids.pos > 0)
            get_last_exit_code();
        ft_printf(OUT, "finish evaluating\n");
        free(text);
        clear_list(&global.tokens);
        clear_list(&global.pids);
        clear_list(&global.fds);
    }

    ft_exit(0);
}