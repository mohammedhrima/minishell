#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char *argv[], char *envp[])
{
    char *array[] = {"/bin/lws", "-la", NULL};
    pid_t pid = fork();
    if(pid == 0)
    {
        if(execve(array[0], array, envp) < 0)
        {
            printf("execve failed\n");
            exit(-1);
        }
    }
    else if(pid > 0)
    {
        int status = 0;
        waitpid(pid, &status, 0);
        printf("did exit 1 with code: %d\n", status);
        status = 0;
        waitpid(pid, &status, 0);
        printf("did exit 2 with code: %d\n", status);
    }
    else
    {
        printf("fork did failed\n");
    }
}