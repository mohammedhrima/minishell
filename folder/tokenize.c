#include "../header.h"

bool check_star(char *text, int *txt_pos_ptr)
{
    char *new;
    DIR *curr_dir;
    struct dirent *content;
    int txt_pos;

    txt_pos = *txt_pos_ptr;
    if (text[txt_pos] == '*')
    {
        add_pointer(&global.tokens, new_token(star_, text + txt_pos, 1));
        txt_pos++;
        new = getcwd(NULL, 0);
        curr_dir = opendir(new);
        free(new);
        content = readdir(curr_dir);
        while (content)
        {
            add_pointer(&global.tokens, new_token(identifier_, content->d_name, ft_strlen(content->d_name)));
            content = readdir(curr_dir);
        }
        closedir(curr_dir);
        *txt_pos_ptr = txt_pos;
        return (true);
    }
    return (false);
}

bool check_space(char *text, int *txt_pos_ptr)
{
    int txt_pos;

    txt_pos = *txt_pos_ptr;
    if (ft_isspace(text[txt_pos]))
    {
        while (ft_isspace(text[txt_pos]))
            txt_pos++;
        *txt_pos_ptr = txt_pos;
        return (true);
    }
    return (false);
}

bool check_special_characters(char *text, int *txt_pos_ptr)
{
    int i;
    t_type type;
    int start;
    int txt_pos;

    i = 0;
    type = 0;
    txt_pos = *txt_pos_ptr;
    while (global.values[i])
    {
        if (ft_strncmp(global.values[i], &text[txt_pos], ft_strlen(global.values[i])) == 0)
        {
            start = txt_pos;
            type = global.types[i];
            txt_pos += ft_strlen(global.values[i]);
            add_pointer(&global.tokens, new_token(type, text + start, txt_pos - start));
            *txt_pos_ptr = txt_pos;
            return (true);
        }
        i++;
    }
    return (false);
}

int check_quotes(char *text, int *txt_pos_ptr)
{
    int txt_pos;
    int single_quote;
    int double_quote;

    txt_pos = *txt_pos_ptr;
    single_quote = 0;
    double_quote = 0;
    while (text[txt_pos])
    {
        if (text[txt_pos] == '\'' && !double_quote)
            single_quote = !single_quote;
        else if (text[txt_pos] == '"' && !single_quote)
            double_quote = !double_quote;
        else if (!single_quote && !double_quote && (ft_isspace(text[txt_pos]) || ft_strchr("|&<>()", text[txt_pos])))
            break;
        txt_pos++;
    }
    add_pointer(&global.tokens, new_token(identifier_, text + *txt_pos_ptr, txt_pos - *txt_pos_ptr));
    *txt_pos_ptr = txt_pos;
    return SUCCESS;
}

int build_tokens(char *text)
{
    int txt_pos;

    txt_pos = 0;
    while (text && text[txt_pos])
    {
        if (text[txt_pos] == '\0')
            break;
        if (check_star(text, &txt_pos) || check_space(text, &txt_pos) || check_special_characters(text, &txt_pos))
            continue;
        if(check_quotes(text, &txt_pos) != 0)
            return SYNTAX_ERROR;
    }
    add_pointer(&global.tokens, new_token(end_, NULL, 0));
    return SUCCESS;
}