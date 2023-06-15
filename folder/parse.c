#include "../header.h"

int is_inclosed(char *text)
{
    int quote;
    int is_operator;
    int is_parent;
    int i;

    quote = 0;
    is_operator = 0;
    is_parent = 0;
    i = 0;
    while (text && text[i])
    {
        if(text[i] == '(')
            is_parent++;
        else if(text[i] == ')')
            is_parent--;
        else if (text[i] == '\'' && quote == 0)
            quote = '\'';
        else if (text[i] == '"' && quote == 0)
            quote = '"';
        else if (text[i] == quote)
            quote = 0;
        else if (ft_strchr("&|", text[i]))
            is_operator = 1;
        else if (text[i] && text[i] != '\n')
            is_operator = 0;
        i++;
    }
    if (quote || is_operator || is_parent)
        return (true);
    return (false);
}

void reset(char *text)
{
    while (global.pids.pos > 0)
        get_last_exit_code();
    free(text);
    clear_list(&global.tokens);
    clear_list(&global.pids);
    clear_list(&global.fds);
}

void evaluate_input(char *text)
{
    t_file *input;
    t_file *output;
    t_token **tokens;
    t_node *curr;

    input = new_file(NULL, IN, 0);
    output = new_file(NULL, OUT, 0);
    global.tokens.pos = 0;
    tokens = (t_token **)global.tokens.pointers;
    while (tokens[global.tokens.pos]->type != end_)
    {
        curr = expr();
        if (curr == NULL)
            break;
        evaluate(curr, input, output);
    }
    reset(text);
}

void handle_input(char *text)
{
    if (is_inclosed(text))
    {
        printf("minishell: syntax error\n");
        add_history(text);
        reset(text);
    }
    else
    {
        add_history(text);
        if (build_tokens(text) != SUCCESS)
            reset(text);
        else
            evaluate_input(text);
    }
}