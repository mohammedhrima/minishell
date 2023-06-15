#include "utils.h"

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

// exit function
int ft_exit(int code)
{
    int i;

    i = 0;
    while(i < global.fds.pos)
    {
        if(global.fds.integers[i] != OUT && global.fds.integers[i] != IN)
            close(global.fds.integers[i]);
        i++;
    }
    i = 0;
    while(i < global.addresses.pos)
    {
        free(global.addresses.pointers[i]);
        i++;
    }
    // if(DEBUG == 0)
        // system("leaks bonus");
    // ft_printf(OUT, "call exit with code %d\n",code);
    exit(code);
    return code;
}

// char methods
int ft_isnum(int c) { return (c >= '0' && c <= '9'); }
// int ft_isupper(int c) { return (c >= 'A' && c <= 'Z'); }
// int ft_islower(int c) { return (c >= 'a' && c <= 'z'); }
// int ft_isalpha(int c) { return (ft_islower(c) || ft_isupper(c)); }
// int ft_isalphanum(int c) { return (ft_isalpha(c) || ft_isnum(c)); }
int	ft_isspace(int c){ return (c == '\t' || c == '\n' || c == '\f' || c == '\r' || c == '\v' || c == ' ');}

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
	unsigned char	*pointer1;
	unsigned char	*pointer2;

	if (!destination)
		return (source);
	if (!source)
		return (destination);
	pointer1 = (unsigned char *)destination;
	pointer2 = (unsigned char *)source;
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
void add_to_addresses(void *pointer)
{
    if (global.addresses.len == 0)
    {
        global.addresses.pos = 0;
        global.addresses.len = 100;
        global.addresses.pointers = malloc(global.addresses.len * sizeof(void *));
    }
    global.addresses.pointers[global.addresses.pos++] = pointer;
    if (global.addresses.pos + 10 > global.addresses.len)
    {
        global.addresses.len *= 2;
        void *temporary = malloc(global.addresses.len * sizeof(void *));
        ft_memcpy(temporary, global.addresses.pointers, global.addresses.pos * sizeof(void *));
        free(global.addresses.pointers);
        global.addresses.pointers = temporary;
    }
}
void    *ft_calloc(size_t count, size_t size)
{
	void			*new;
	unsigned char	*pointer;
	size_t			i;

	new = (void *)malloc(count * size);
    add_to_addresses(new);
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

void *ft_realloc(void *pointer, size_t oldsize, size_t newsize)
{
    void *new = ft_calloc(1, newsize);
    if(pointer)
        ft_memcpy(new, pointer, oldsize);
    return new;
}

// string methods
int     ft_strlen(char *string){ int i = 0; while (string && string[i]) i++; return i;}
void    ft_strncpy(char *destination, char *source, int len)
{
    int i = 0;
    while (source[i] && i < len)
    {
        destination[i] = source[i];
        i++;
    }
}
void    ft_strcpy(char *destination, char *source)
{
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
            res = ft_realloc(res, j * sizeof(char*), ( j + 2 ) * sizeof(char*));
        res[j] = ft_calloc(i - start + 2, sizeof(char));
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
        if(string[i] == c)
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

// ft_itoa ft_atoi
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
    left[ft_strlen(left)] = num % 10 + '0';
    return left;
}
int ft_atoi(char *str) {
    int result = 0;
    int sign = 1;
    int i = 0;

    if(str == NULL)
        return 0;
    while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
        i++;

    if (str[i] == '+' || str[i] == '-') {
        sign = (str[i] == '-') ? -1 : 1;
        i++;
    }
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }
    return sign * result;
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
#if 1
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
}
#endif