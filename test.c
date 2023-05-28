#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#define out 1
#define in 0

int     ft_strlen(char *string){ int i = 0; while (string && string[i]) i++; return i;}
void    ft_putstr(int fd, char *str){ write(fd, str, ft_strlen(str));}
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
void *ft_realloc(void *pointer, size_t oldsize, size_t newsize)
{
    void *new = ft_calloc(1, newsize);
    if(pointer)
    {
        ft_memcpy(new, pointer, oldsize);
        free(pointer);
    }
    return new;
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
    return res;
}
int main(void)
{
    while(1)
    {
        char *str = ft_readline("> ");
        // printf("exit readline\n");
        if(str)
        {
            ft_putstr(out, "Input: ");
            ft_putstr(out, str);
            // ft_putstr(out ," ");
        }
        free(str);
    }
}