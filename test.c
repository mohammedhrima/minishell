#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main(void)
{
    char	*new;

	new = getcwd(NULL, 0);
	printf("%s\n", new);

    chdir(".."); // verify if this little shit leaks
	
    new = getcwd(NULL, 0);
	printf("%s\n", new);
}