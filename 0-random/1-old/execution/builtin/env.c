#include "builtin.h"

void env_func(char **arguments)
{
    int i;
    t_node **envirement;

    arguments = NULL;
    envirement = (t_node **)global.envirement.pointers, i = 0;
    while (envirement[i])
    {
        if (envirement[i]->left->token->value)
            printf("%s=%s\n", envirement[i]->left->token->value,
                   envirement[i]->right->token->value);
        i++;
    }
}
