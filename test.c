#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char **argv, char **envp)
{
# if 1
    char *array[] = {"./", "random", "random1", "random2", "random3", "random4",
                        "random5", "random6", "random7", "random8", "test", NULL};
    // int i = 0;
    // while(array[i])
    // {
        // printf("->%s\n", array[i]);
        int pid = fork();
        if(pid == 0)
        {
            char *cmd = "/bin/ls";
            // char *args[] = {"ls", &array[i], NULL};
            if(execve(cmd, &array[0], envp) < 0)
                printf("Execve failed 1\n");
        }
        // i++;
    // }
    // while(--i > 0)
    //     wait(NULL);
#else
    char *array[] = {"./", "random", "test", NULL};
    int i = 0;
    while(array[i])
    {
        printf("->%s\n", array[i]);
        int pid = fork();
        if(pid == 0)
        {
            char *cmd = "/bin/ls";
            char *args[] = {"ls", array[i], NULL};
            if(execve(cmd, args, envp) < 0)
                printf("Execve failed 1\n");
        }
        i++;
    }
    while(--i > 0)
        wait(NULL);
#endif
    // waitpid(pid, NULL, 0);
}
