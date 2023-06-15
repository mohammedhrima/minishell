#include "../header.h"

void echo_func(char **arguments)
{
    int print_new_line;
    int i;

    print_new_line = 1, i = 0;
    while (arguments && arguments[i])
    {
        if (i == 0 && ft_strcmp(arguments[i], "-n") == 0)
            print_new_line = 0;
        else
            printf("%s", arguments[i]);
        i++;
    }
    if (print_new_line)
        printf("\n");
}

void cd_func(char **arguments)
{
    if (arguments[0])
        chdir(arguments[0]);
    else
        chdir(getenv("HOME"));
}

void pwd_func(char **arguments)
{
    char *new;

    new = getcwd(NULL, 0);
    printf("%s\n", new);
    free(new);
}

void new_envirement_variable(char **arguments)
{
    t_node *node;
    char **array;

    array = split_by_two(arguments[0], '=');
    node = new_node(new_token(assign_, NULL, 0));
    node->left = new_node(new_token(identifier_, NULL, 0));
    node->left->token->value = array[0];
    node->right = new_node(new_token(identifier_, NULL, 0));
    node->right->token->value = array[1];
    add_pointer(&global.envirement, node);
}

void export_func(char **arguments)
{
    t_node **envirement;
    int i;

    if (arguments[0])
        new_envirement_variable(arguments);
    else
    {
        i = 0;
        envirement = (t_node **)global.envirement.pointers;
        while (envirement && envirement[i])
        {
            if (envirement[i]->left->token->value)
                printf("declare -x %s=\"%s\"\n",
                       envirement[i]->left->token->value,
                       envirement[i]->right->token->value);
            i++;
        }
    }
}

void unset_func(char **argument)
{
    int i;
    t_node **envirement;
    char *value;

    envirement = (t_node **)global.envirement.pointers, i = 0;
    while (envirement && envirement[i])
    {
        value = envirement[i]->left->token->value;
        if (value && ft_strcmp(value, argument[0]) == 0)
        {
            envirement[i]->left->token->value = NULL;
            envirement[i]->right->token->value = NULL;
            break;
        }
        i++;
    }
    if (ft_strcmp(argument[0], "PATH") == 0)
        global.path = NULL;
}

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


void exit_func(char **arguments)
{
    ft_exit(ft_atoi(arguments[0]));
}

void *get_built_in_function(char *cmd)
{
    if (ft_strcmp(cmd, "echo") == 0)
        return (&echo_func);
    if (ft_strcmp(cmd, "cd") == 0)
        return (&cd_func);
    if (ft_strcmp(cmd, "pwd") == 0)
        return (&pwd_func);
    if (ft_strcmp(cmd, "export") == 0)
        return (&export_func);
    if (ft_strcmp(cmd, "unset") == 0)
        return (&unset_func);
    if (ft_strcmp(cmd, "env") == 0)
        return (&env_func);
    if (ft_strcmp(cmd, "exit") == 0)
        return (&exit_func);
    return (NULL);
}