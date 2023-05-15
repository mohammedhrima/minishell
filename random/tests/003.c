#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define out STDOUT_FILENO
#define in STDIN_FILENO

typedef struct
{
    char *command;
    char **arguments;
} cmd;
#define len 3
int main(int argc, char *argv[], char **envp)
{

    cmd cmds[len];
    cmds[0].command = "/bin/ls";
    cmds[0].arguments = (char *[]){"ls", "-la", NULL};

    cmds[1].command = "/usr/bin/grep";
    cmds[1].arguments = (char *[]){"grep", "\\.c$", NULL};

    cmds[2].command = "/usr/bin/wc";
    cmds[2].arguments = (char *[]){"wc", "-w", NULL};

    int *fd = calloc(2, sizeof(int));
    if (pipe(fd) != 0)
    {
        printf("Error opennign pipe\n");
        exit(0);
    }

    int i = 0;
    while (i < len)
    {
        int pid = fork();
        if (pid < 0)
        {
            printf("Failed forking\n");
            exit(0);
        }
        else if (pid == 0)
        {
            printf("Execute: %s\n", cmds[i].command );
            // child process
            if (i + 1 < len)
                dup2(fd[1], STDOUT_FILENO);
            if (i > 0)
                dup2(fd[0], STDIN_FILENO);
            close(fd[0]);
            close(fd[1]);
            if (execve(cmds[i].command, cmds[i].arguments, envp) < 0)
            {
                printf("Execve failed %d\n", i);
            }
        }
        else
        {
            // parent process
            close(fd[0]);
            close(fd[1]);
            fd = calloc(2, sizeof(int));
            // if(i == 0)
            //     dup2(fd[0], 0);
            pipe(fd);
        }
        i++;
    }

    i = 0;
    while (i < len)
    {
        wait(NULL);
        i++;
    }

    return 0;
}