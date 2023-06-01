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
        {"EXIT", exit_}, {"END", end_}, {"STAR", star_}, //{"STAR END", star_end},
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
    int i = 0;
    while(i < global.addresses.pos)
    {
        free(global.addresses.pointers[i]);
        i++;
    }
    if(DEBUG == 0)
        system("leaks a.out");
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



char *ft_readline(char *msg)
{
    char *res = ft_calloc(2 , sizeof(char));
    int i = 0;
    char c = 1;
    ft_putstr(out, msg);
    while(c && c != '\n')
    {
        read(in, &c, sizeof(char));
        res[i] = c;
        res = ft_realloc(res, i + 1, i + 2);
        i++;
    }
    // res[i - 1] = c;
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
void ft_printf(int fd, char *fmt, ...)
{
# if 1
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
                    case append_:
                    case and_:
                    case or_:
                    case star_:
                    case lparent_:
                    case rparent_:
                    // case star_end:
                        print_space(fd, space - ft_strlen(variable->value));
                        ft_putstr(fd, variable->value);
                        break;
                    default:
                        ft_putstr(err, "Error in ft_printf: Unkown given token type: ");
                        ft_putnbr(err, variable->type);
                        ft_putstr(err, "\n");
                        ft_exit(0);
                        break;
                    }
                }
                else
                    ft_putstr(fd, "(null token)");
            }
            if (fmt[i] == 't')
            {
                char* type = type_to_string(va_arg(ap, Type));
                print_space(fd, space - ft_strlen(type));
                ft_putstr(fd, type);
            }
            if(fmt[i] == 'F')
            {
                File file = va_arg(ap, File);
                if(file.name)
                    ft_printf(out, "filename: %s, ", file.name);
                if(file.fd >= 0)
                    ft_printf(out, "fd: %d, ", file.fd);
                else
                    ft_printf(out, "fd: NOT OPENED, ");
                if(file.type )
                    ft_printf(out, "with type %s", type_to_string(file.type));
            
            }
            if (fmt[i] == 'd')
            {
                int num = va_arg(ap, int);
                ft_putnbr(fd, (long)num);
            }
            if (fmt[i] == 'c')
            {
                int c = va_arg(ap, int);
                ft_putchar(fd, c);
            }
            if (fmt[i] == 's')
            {
                char *str = va_arg(ap, char *);
                ft_putstr(fd, str);
            }
        }
        else
            ft_putchar(fd, fmt[i]);
        i++;
    }
    va_end(ap);
    // if (fd == err)
    //     ft_exit(1);
#endif
}