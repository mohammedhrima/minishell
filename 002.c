#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[], char *envp[]) {
    char *args[] = { "/bin/ls", "-l", NULL };

    if (execve(args[0], args, envp) == -1) {
        perror("execve");
        return 1;
    }

    // This code will only be reached if execve fails
    printf("Execve failed\n");
    return 1;
}