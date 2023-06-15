#include "../header.h"

t_list *new_list(t_list *list, size_t size, t_type type)
{
    list->pos = 0;
    list->len = 100;
    list->type = type;
    list->size = size;
    if (list->type == pointers_)
        list->pointers = ft_calloc(list->len, list->size);
    if (list->type == integers_)
        list->integers = ft_calloc(list->len, list->size);
    return (list);
}

t_node *new_node(t_token *token)
{
    t_node *new;

    new = ft_calloc(1, sizeof(t_node));
    new->token = token;
    if (new->token)
        ft_printf(OUT, "new  node with type %6t has token: '%k'\n", token->type,
                  new->token);
    return (new);
}

t_token *new_token(t_type type, char *source, int len)
{
    t_token *new;

    new = ft_calloc(1, sizeof(t_token));
    new->type = type;
    ft_printf(OUT, "new token with type %6t ", new->type);
    if (source)
    {
        new->value = ft_calloc(len + 1, sizeof(char));
        ft_strncpy(new->value, source, len);
        ft_printf(OUT, "has value: '%k'", new);
    }
    ft_printf(OUT, "\n");
    return (new);
}

t_file *new_file(char *name, int fd, t_type type)
{
    t_file *new;

    new = ft_calloc(1, sizeof(t_file));
    new->name = name;
    new->fd = fd;
    new->type = type;
    add_number(&global.fds, new->fd);
    return (new);
}