#include "../header.h"

//to be removed
char *type_to_string(t_type type)
{
    char **values;
    t_type *types;
    int i;

    values = (char*[]){"=", "identifier", "<", ">", "<<",  ">>", "|", "env", "echo", "&&", "||", 
    "cd", "pwd", "export", "unset", "exit", "end",  "*", "(", ")", NULL};
    types = (t_type[]){assign_, identifier_, redir_input, redir_output, heredoc_, append_, 
    pipe_, env_, echo_, and_, or_, cd_, pwd_, export_, unset_, exit_, end_, star_, lparent_, rparent_, 0};
    i = 0;
    while(types[i])
    {
        if(types[i] == type)
            return values[i];
        i++;
    }
    return NULL;
}

// printf
void ft_putchar(int fd, int c){ write(fd, &c, sizeof(char));}
void ft_putstr(int fd, char *str){ write(fd, str, ft_strlen(str));}
void ft_putnbr(int fd, long num)
{
    if (num < 0)
    {
        ft_putchar(fd, '-');
        num = -num;
    }
    if (num < 10) ft_putchar(fd, num + '0');
    else
    {
        ft_putnbr(fd, num / 10);
        ft_putnbr(fd, num % 10);
    }
}
void print_space(int fd, int len)
{
    int i = 0;
    while(i < len)
    {
        ft_putchar(fd, ' ');
        i++;
    }
}
int ft_isnum(int c) 
{ 
    return (c >= '0' && c <= '9');
}
void ft_printf(int file_descriptor, char *fmt, ...)
{
#if 1
    va_list ap;

    va_start(ap, fmt);
    int i = 0;
    while (fmt && fmt[i])
    {
        if (fmt[i] == '%')
        {
            i++;
            int space = 0;
            if(ft_isnum(fmt[i]))
            {
                while(ft_isnum(fmt[i]))
                {
                    space = 10 * space + fmt[i] - '0';
                    i++;
                }
            }
            if (fmt[i] == 'k')
            {
                t_token *variable = va_arg(ap, t_token *);
                if (variable)
                {
                    switch (variable->type)
                    {
                    case assign_:
                    case identifier_:
                    case pipe_:
                    case end_:
                    case star_:
                    case redir_input:
                    case redir_output:
                    case heredoc_:
                    case append_:
                    case and_:
                    case or_:
                    case lparent_:
                    case rparent_:
                        print_space(file_descriptor, space - ft_strlen(variable->value));
                        ft_putstr(file_descriptor, variable->value);
                        break;
                    default:
                        ft_putstr(ERR, "Unkown given token type: ");
                        ft_putnbr(ERR, variable->type);
                        ft_putstr(ERR, "\n");
                        ft_exit(0);
                        break;
                    }
                }
                else
                    ft_putstr(file_descriptor, "(null token)");
            }
            if (fmt[i] == 't')
            {
                char* type = type_to_string(va_arg(ap, t_type));
                print_space(file_descriptor, space - ft_strlen(type));
                ft_putstr(file_descriptor, type);
            }
            if (fmt[i] == 'd')
            {
                int num = va_arg(ap, int);
                ft_putnbr(file_descriptor, (long)num);
            }
            if (fmt[i] == 'c')
            {
                int c = va_arg(ap, int);
                ft_putchar(file_descriptor, c);
            }
            if (fmt[i] == 's')
            {
                char *str = va_arg(ap, char *);
                ft_putstr(file_descriptor, str);
            }
        }
        else
            ft_putchar(file_descriptor, fmt[i]);
        i++;
    }
    va_end(ap);
#endif
}