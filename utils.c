#include "header.h"

bool	is_redirection(Type type)
{
	return (type == redir_input || type == redir_output || type == append_ || type == heredoc_);
}

// memory
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
	size_t			i;
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

void	add_to_addresses(void *pointer)
{
	void	*temporary;

	if (Global.addresses.len == 0)
	{
		Global.addresses.pos = 0;
		Global.addresses.len = 100;
		Global.addresses.pointers = malloc(Global.addresses.len * sizeof(void *));
	}
	Global.addresses.pointers[Global.addresses.pos++] = pointer;
	if (Global.addresses.pos + 10 > Global.addresses.len)
	{
		Global.addresses.len *= 2;
		temporary = malloc(Global.addresses.len * sizeof(void *));
		ft_memcpy(temporary, Global.addresses.pointers, Global.addresses.pos * sizeof(void *));
		free(Global.addresses.pointers);
		Global.addresses.pointers = temporary;
	}
}

void	*ft_calloc(size_t count, size_t size)
{
	void			*new;
	unsigned char	*pointer;
	size_t			i;

	new = (void *)malloc(count * size);
	if (!new)
	{
		printf("Error in allocation\n");
		exit(ERROR);
	}
	add_to_addresses(new);
	i = 0;
	pointer = (unsigned char *)new;
	while (i < count * size)
	{
		pointer[i] = 0;
		i++;
	}
	return (new);
}

void	*ft_realloc(void *pointer, size_t oldlen, size_t newlen, size_t size)
{
	void	*new;

	new = ft_calloc(newlen, size);
	if (pointer)
		ft_memcpy(new, pointer, oldlen * size);
	return (new);
}

// string and character (protect them after)
int ft_strlen(char *string)
{
	int	i;

	i = 0;
	while (string && string[i])
		i++;
	return (i);
}

void	ft_strncpy(char *destination, char *source, int len)
{
	int	i;

	i = 0;
	while (source[i] && i < len)
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

int	ft_strncmp(char *left, char *right, size_t len)
{
	size_t	i;

	i = 0;
	while (i < len && left[i] && right[i] && left[i] == right[i])
		i++;
	if (i == len)
		return (0);
	return (left[i] - right[i]);
}

int	ft_strcmp(char *left, char *right)
{
	size_t	i;

	i = 0;
	while (left[i] && right[i] && left[i] == right[i])
		i++;
	return (left[i] - right[i]);
}

char *ft_strnstr(char *string, char *to_find)
{
	int i = 0;
	while(string && string[i])
	{
		if(ft_strncmp(&string[i], to_find, ft_strlen(to_find)) == 0)
			return &string[i];
		i++;
	}
	return NULL;
}

bool	ft_isspace(int c)
{
	if(ft_strchr("\t\n\f\r\v ", c))
		return true;
	return false;
}

bool ft_isalpha(int c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

char	*strjoin(char *string1, char *string2, char *string3)
{
	char	*res;

	res = ft_calloc(ft_strlen(string1) + ft_strlen(string2) + ft_strlen(string3)
			+ 1, sizeof(char));
	if (string1)
		ft_strncpy(res, string1, ft_strlen(string1));
	if (string2)
		ft_strncpy(res + ft_strlen(res), string2, ft_strlen(string2));
	if (string3)
		ft_strncpy(res + ft_strlen(res), string3, ft_strlen(string3));
	return (res);
}

// split
char **split(char*string, char *spliter)
{
    char    **res;
    int     i;
    int     j;
    int     start;
    char    *subString;
    
    res = NULL;
    i = 0;
    j = 0;
    while(string && ft_strncmp(string + i, spliter, ft_strlen(spliter)) == 0)
        i += ft_strlen(spliter);
    while(string && string[i])
    {
        start = i;
        while(string[i] && ft_strncmp(string + i, spliter, ft_strlen(spliter)))
            i++;
        if(res == NULL)
            res = ft_calloc(j + 2, sizeof(char*));
        else
            res = ft_realloc(res, j, j + 2, sizeof(char*));
        res[j] = ft_calloc(i - start + 1, sizeof(char));
        ft_memcpy(res[j], string + start, i - start);
        j++;
        while(string[i] && ft_strncmp(string + i, spliter, ft_strlen(spliter)) == 0)
            i += ft_strlen(spliter);
    }
    res[j] = NULL;
    return res;
}

char	**split_by_two(char *str, char c)
{
	int		i;
	char	**res;

	i = 0;
	res = ft_calloc(2, sizeof(char *));
	while (str && str[i])
	{
		if (str[i] == c)
		{
			res[0] = ft_calloc(i + 1, sizeof(char));
			ft_strncpy(res[0], str, i);
			i++;
			res[1] = ft_calloc(ft_strlen(str) - i + 1, sizeof(char));
			ft_strncpy(res[1], str + i, ft_strlen(str) - i);
			break ;
		}
		i++;
	}
	return (res);
}


// list
void	add_pointer(List *list, void *value)
{
	list->pointers[list->pos++] = value;
	if (list->pos + 10 > list->len)
	{
		list->len *= 2;
		list->pointers = ft_realloc(list->pointers, list->pos, list->len, list->size);
	}
}

void	add_number(List *list, int number)
{
	list->integers[list->pos++] = number;
	if (list->pos + 10 > list->len)
	{
		list->len *= 2;
		list->integers = ft_realloc(list->integers, list->pos, list->len, list->size);
	}
}
void	clear_list(List *list)
{
	Type	type;
    int     i;

	type = list->type;
	if (type == integers_)
        ft_memset(list->integers, 0, list->len * list->size);
	else if (type == pointers_)
		ft_memset(list->pointers, 0, list->len * list->size);
	list->pos = 0;
}

//
int	new_child_process(void)
{
	int	pid;

	pid = fork();
	if (pid < 0)
	{
		printf("Error forking\n");
		ft_exit(ERROR);
	}
	add_number(&Global.pids, pid);
	return (pid);
}

int	*open_pipe(void)
{
	int	*fd;

	fd = ft_calloc(2, sizeof(int));
	if (pipe(fd))
	{
		printf("Error openning pipe\n");
		ft_exit(ERROR);
	}
	return (fd);
}

void	close_fds(void)
{
	int	i;

	i = 0;
	while (i < Global.fds.pos)
	{
		if (Global.fds.integers[i] != IN && Global.fds.integers[i] != OUT)
			close(Global.fds.integers[i]);
		i++;
	}
}

void	ft_exit(int code)
{
	int	i;
	
	close_fds();
	i = 0;
	while (i < Global.addresses.pos)
	{
		free(Global.addresses.pointers[i]);
		i++;
	}
    // system("leaks exe");
	exit(code);
}
 // convert
char	*ft_itoa(int num)
{
	char	*res;
	char	*left;

	if (num < 10)
	{
		res = ft_calloc(2, sizeof(char));
		res[0] = num + '0';
		return (res);
	}
	left = ft_itoa(num / 10);
	left = ft_realloc(left, ft_strlen(left), ft_strlen(left) + 2, sizeof(char));
	left[ft_strlen(left)] = num % 10 + '0';
	return (left);
}

int	ft_atoi(char *str)
{
	int	result;
	int	sign;
	int	i;

	result = 0;
	sign = 1;
	i = 0;
	if (str == NULL)
		return (0);
	while (ft_isspace(str[i]))
		i++;
	if (ft_strchr("+-", str[i]))
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (sign * result);
}

char	*ft_strdup(char *string, int len)
{
	char	*pointer;
	int		i;

	if (!string)
		return (NULL);
	pointer = ft_calloc(len + 1, sizeof(char));
	if (!pointer)
		return (NULL);
	i = 0;
	while (string && string[i] && i < len)
	{
		pointer[i] = string[i];
		i++;
	}
	pointer[i] = '\0';
	return (pointer);
}