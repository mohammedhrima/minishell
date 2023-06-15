#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define in STDIN_FILENO
#define out STDOUT_FILENO
#define err STDERR_FILENO

int main(int argc, char **argv, char **envp)
{
    char *cmd1 = "/bin/ls";
    char *cmd2 = "/usr/bin/wc";

    char *args1[] = {"ls", "-la", NULL};
    char *args2[] = {"wc", "-w", NULL};
    int fd[2];
    if(pipe(fd) < 0)
    {
        printf("Error opening pipe\n");
        exit(0);
    }


    pid_t pid1 = fork();
    
    if(pid1 == 0)
    {
        printf("Enter fork 1\n");

        dup2(fd[1], out);
        close(fd[0]);
        close(fd[1]);

        char *cmd1 = "/bin/ls";
        char *args1[] = {"ls", "-la", NULL};

        if(execve(cmd1, args1, envp) < 0)
        {
            printf("Execve failed 1\n");
        }
    }
    pid_t pid2 = fork();
    if(pid2 == 0)
    {
        printf("Enter fork 2\n");

        dup2(fd[0], in);
        close(fd[0]);
        close(fd[1]);
        
        char *cmd2 = "/usr/bin/wc";
        char *args2[] = {"wc", "-w", NULL};

        if(execve(cmd2, args2, envp) < 0)
        {
            printf("Execve failed 2\n");
        }
    }
    close(fd[0]);
    close(fd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}