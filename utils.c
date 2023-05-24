#include "header.h"

char *type_to_string(Type type)
{
    struct {
        char *value;
        Type type;
    } lexic[] = {
        {"ASSIGN", assign_},  {"IDENTIFIER", identifier_},
        {"INPUT REDIR", redir_input}, {"OUTPUT REDIR", redir_output},  
        {"HEREDOC", heredoc_}, {"APPEND_OUTPUT", append_}, {"PIPE", pipe_}, 
        {"ENV", env_}, {"ECHO", echo_}, {"PROCECESS ID", pid_}, {"AND", and_}, 
        {"OR", or_}, {"CD", cd_}, {"PWD", pwd_},  {"EXPORT", export_}, {"UNSET", unset_}, 
        {"EXIT", exit_}, {"END", end_}, 
        {0, 0}
    };
    for(int i = 0; lexic[i].value; i++)
    {
        if(lexic[i].type == type)
            return lexic[i].value;
    }
    return NULL;
}

// exit function
void ft_exit(int code)
{
    // make sure to free your shit before exiting
    // address_pos--;
    // while(address_pos >= 0)
    // {
    //     free((void*)(addresses[address_pos]));
    //     address_pos--;
    // }
    exit(code);
}

// char methods
int ft_isnum(int c) { return (c >= '0' && c <= '9'); }
int ft_isupper(int c) { return (c >= 'A' && c <= 'Z'); }
int ft_islower(int c) { return (c >= 'a' && c <= 'z'); }
int ft_isalpha(int c) { return (ft_islower(c) || ft_isupper(c)); }
int ft_isalphanum(int c) { return (ft_isalpha(c) || ft_isnum(c)); }
int ft_isspace(int c) { return (c == '\n' || c == ' ' || c == '\v' || c == '\r' || c == '\t');}

// memes
void	ft_memset(void *pointer, int c, size_t len)
{
	size_t			i;
	unsigned char	*temporary;

	temporary = (unsigned char *)pointer;
	i = 0;
	while (temporary && i < len)
	{
		temporary[i] = c;
		i++;
	}
}
void	*ft_memcpy(void *destination, void *source, size_t len)
{
	size_t	i;
	char	*pointer1;
	char	*pointer2;

	if (!destination)
		return (source);
	if (!source)
		return (destination);
	pointer1 = (char *)destination;
	pointer2 = (char *)source;
	i = 0;
	while (i < len)
	{
		pointer1[i] = pointer2[i];
		i++;
	}
	return (destination);
}

// memory allocation
// this one add to addresses
void    *ft_calloc(size_t count, size_t size)
{
	void			*new;
	unsigned char	*pointer;
	size_t			i;

	new = (void *)malloc(count * size);
	if (!new)
		return (NULL);
	i = 0;
	pointer = (unsigned char *)new;
	while (i < count * size)
	{
		pointer[i] = 0;
		i++;
	}
	return (new);
}

// void    *ft_calloc2(size_t count, size_t size)
// {
// 	void			*new;
// 	unsigned char	*pointer;
// 	size_t			i;

// 	new = (void *)malloc(count * size);
// 	if (!new)
// 		return (NULL);
// 	i = 0;
// 	pointer = (unsigned char *)new;
// 	while (i < count * size)
// 	{
// 		pointer[i] = 0;
// 		i++;
// 	}
// 	return (new);
// }

void *ft_realloc(void *pointer, size_t old_size ,size_t new_size)
{
    if (pointer == NULL)
        return ft_calloc(1, new_size);

    void* new = ft_calloc(1, new_size);
    ft_memcpy(new, pointer, old_size);
    free(pointer);
    return new;
}

// string methods
int     ft_strlen(char *string){ int i = 0; while (string && string[i]) i++; return i;}
void    ft_strncpy(char *destination, char *source, int len)
{
    if (destination == NULL || source == NULL)
        ft_printf(err, "receive NULL in strncpy\n");
    int i = 0;
    while (source[i] && i < len)
    {
        destination[i] = source[i];
        i++;
    }
}
void    ft_strcpy(char *destination, char *source)
{
    if (destination == NULL || source == NULL)
        ft_printf(err, "receive NULL in strcpy\n");
    int i = 0;
    while (source[i])
    {
        destination[i] = source[i];
        i++;
    }
}
char	*ft_strchr(char *string, int c)
{
	int	i;

	i = 0;
	if (!string)
		return (NULL);
	while (string[i])
	{
		if (string[i] == (char)c)
			return (string + i);
		i++;
	}
	if (c == 0 && string[i] == 0)
		return (string + i);
	return (NULL);
}
int     ft_strncmp(char *left, char *right, size_t len)
{
    size_t i = 0;
    while (i < len && left[i] && right[i] && left[i] == right[i])
        i++;
    if (i == len)
        return (0);
    return (left[i] - right[i]);
}
int     ft_strcmp(char *left, char *right)
{
    size_t i = 0;
    while (left[i] && right[i] && left[i] == right[i])
        i++;
    return (left[i] - right[i]);
}
char **split(char *string, char *spliter)
{
    int i = 0;
    int j = 0;
    char **res = NULL;
    int start = 0;
    if(ft_strncmp(string + i, spliter, ft_strlen(spliter)) == 0)
        i += ft_strlen(spliter);
    start = i;
    while(string && string[i])
    {
        while(ft_strncmp(string + i, spliter, ft_strlen(spliter)) == 0)
            i += ft_strlen(spliter);
        start = i;
        while(string[i] && ft_strncmp(string + i, spliter, ft_strlen(spliter)) != 0)
            i++;
        if(res == NULL)
            res = ft_calloc(j + 2, sizeof(char*));
        else
            res = ft_realloc(res, (j) * sizeof(char*), ( j + 3 ) * sizeof(char*));
        res[j] = ft_calloc(i - start + 1, sizeof(char));
        ft_strncpy(res[j], string + start, i - start);
        j++;
        res[j] = NULL;
    }
    return res;
}
char **split_by_two(char *string, char c)
{
    int i = 0;
    char **res = ft_calloc(2, sizeof(char *));
    while(string && string[i])
    {
        if(string[i] == '=')
        {
            res[0] = ft_calloc(i + 1, sizeof(char));
            ft_strncpy(res[0], string, i);
            i++;
            res[1] = ft_calloc(ft_strlen(string) - i + 1 , sizeof(char));
            ft_strncpy(res[1], string + i, ft_strlen(string) - i);
            break;
        }
        i++;
    }
    return res;
}
char *strjoin(char *string1, char *string2, char *string3)
{
    char *res = ft_calloc(ft_strlen(string1) + ft_strlen(string2) + ft_strlen(string3) + 1, sizeof(char));
    if (string1)
        ft_strncpy(res, string1, ft_strlen(string1));
    if (string2)
        ft_strncpy(res + ft_strlen(res), string2, ft_strlen(string2));
    if (string3)
        ft_strncpy(res + ft_strlen(res), string3, ft_strlen(string3));
    return res;
}

char *charjoin(char *string, char c)
{
    char *res = ft_calloc(ft_strlen(string) + 2, sizeof(char));
    if (string)
        ft_strncpy(res, string, ft_strlen(string));
    res[ft_strlen(res)] = c;
    return res;
}

char	*ft_strdup(char *string)
{
	char	*pointer;
	int		i;

	if (!string)
		return (NULL);
	pointer = ft_calloc((ft_strlen(string) + 1), sizeof(char));
	if (!pointer)
		return (NULL);
	i = 0;
	while (string && string[i])
	{
		pointer[i] = string[i];
		i++;
	}
	pointer[i] = '\0';
	return (pointer);
}

// ft_itoa
char *ft_itoa(int num)
{
    if(num < 10)
    {
        char *res = ft_calloc(2, sizeof(char));
        res[0] = num + '0';
        return res;
    }
    char *left = ft_itoa(num / 10);
    left = ft_realloc(left, ft_strlen(left), ft_strlen(left) + 2);
    left[ft_strlen(left)] = num % 10 + '0';;
    return left;
}

// printf
void ft_putchar(int file_descriptor, int c){ write(file_descriptor, &c, sizeof(char));}
void ft_putstr(int file_descriptor, char *str){ write(file_descriptor, str, ft_strlen(str));}
void ft_putnbr(int file_descriptor, long num)
{
    if (num < 0)
    {
        ft_putchar(file_descriptor, '-');
        num = -num;
    }
    if (num < 10) ft_putchar(file_descriptor, num + '0');
    else
    {
        ft_putnbr(file_descriptor, num / 10);
        ft_putnbr(file_descriptor, num % 10);
    }
}
void print_space(int file_descriptor, int len)
{
    int i = 0;
    while(i < len)
    {
        ft_putchar(file_descriptor, ' ');
        i++;
    }
}
void ft_printf(int file_descriptor, char *fmt, ...)
{
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
                Token *variable = va_arg(ap, Token *);
                if (variable)
                {
                    switch (variable->type)
                    {
                    case assign_:
                    case identifier_:
                    case pipe_:
                    case end_:
                    // case dollar_:
                    case redir_input:
                    case redir_output:
                    case heredoc_:
                    case and_:
                    case or_:
                        print_space(file_descriptor, space - ft_strlen(variable->value));
                        ft_putstr(file_descriptor, variable->value);
                        break;
                    default:
                        ft_putstr(err, "Unkown given token type: ");
                        ft_putnbr(err, variable->type);
                        ft_putstr(err, "\n");
                        ft_exit(0);
                        break;
                    }
                }
                else
                    ft_putstr(file_descriptor, "(null token)");
            }
            if (fmt[i] == 't')
            {
                char* type = type_to_string(va_arg(ap, Type));
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
    if (file_descriptor == err)
        ft_exit(1);
}