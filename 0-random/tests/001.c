#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// memory allocation
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
    add_to_list(&global.addresses, new);
	return (new);
}
void *ft_realloc(void *pointer, size_t size)
{
    if (pointer == NULL)
        return ft_calloc(1, size);

    void* new = ft_calloc(1, size);
    ft_memcpy(new, pointer, size);
    free(pointer);
    return new;
}

// string methods
int     ft_strlen(char *string){ int i = 0; while (string && string[i]) i++; return i;}
void    ft_strncpy(char *destination, char *source, int len)
{
    // if (destination == NULL || source == NULL)
    //     ft_printf(err, "receive NULL in strncpy\n");
    int dest_len = ft_strlen(destination);
    int i = 0;
    while (source[i] && i < len)
    {
        destination[dest_len + i] = source[i];
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
char **split(char *str, char *spliter)
{
    int i = 0;
    int j = 0;
    char **res = NULL;
    int start = 0;
    if(ft_strncmp(str + i, spliter, ft_strlen(spliter)) == 0)
        i += ft_strlen(spliter);
    start = i;
    while(str && str[i])
    {
        printf("allo\n");
        while(ft_strncmp(str + i, spliter, ft_strlen(spliter)) == 0)
            i += ft_strlen(spliter);
        start = i;
        while(str[i] && ft_strncmp(str + i, spliter, ft_strlen(spliter)) != 0)
            i++;
        if(i > start)
        {
            res = ft_realloc(res, (j + 2) * sizeof(char*));
            res[j] = ft_calloc(i - start + 1, sizeof(char));
            ft_strncpy(res[j], str + start, i - start);
            printf("res[%d] -> %s\n",j, res[j]);
            j++;
            res[j] = NULL;
        }
    }
    return res;
}

int main(void)
{
    char **array = split("abdcd", "d");
    int i = 0;
    while(array[i])
    {
        printf("allo 2");
        printf("-> %s\n", array[i]);
        i++;
    }
}