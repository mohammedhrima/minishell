#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void *ft_realloc(void *pointer, size_t oldsize, size_t newsize)
{
    void *new = calloc(1, newsize);
    if(pointer)
    {   
        memcpy(new, pointer, oldsize);
        free(pointer);
    }
    pointer = new;
    return pointer;
}

int main(int argc, char *argv[], char *envp[])
{
    char *dest = "abcdef";
    char *src = "ghijklmno";
    char *str = strdup(dest);

    str = ft_realloc(str, strlen(str) , strlen(str) + strlen(src) + 2);
    strcpy(str + strlen(str), src);
    printf("str: %s\n", str);
    system("leaks a.out");
}