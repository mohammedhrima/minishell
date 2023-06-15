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

void build_envirement(char **envp)
{
    int i;
    char **array;
    t_node *node;

    i = 0;
    while (envp && envp[i])
    {
        array = split_by_two(envp[i], '=');
        node = new_node(new_token(assign_, NULL, 0));
        node->left = new_node(new_token(identifier_, NULL, 0));
        node->left->token->value = array[0];
        node->right = new_node(new_token(identifier_, NULL, 0));
        node->right->token->value = array[1];
        add_pointer(&global.envirement, node);
        i++;
    }
}