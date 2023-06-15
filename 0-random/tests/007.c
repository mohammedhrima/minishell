#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define in STDIN_FILENO
#define out STDOUT_FILENO
#define test 3

#if test == 2
    #define len 2
#endif

#if test == 3
    #define len 3
#endif

typedef struct {
    char *command;
    char **arguments;
} cmd;

int main(int argc, char *argv[], char **envp)
{
    cmd cmds[len];

    cmds[0].command = "/bin/ls";
    cmds[0].arguments = (char *[]){"ls", "-la", NULL};

#if test == 3
    cmds[1].command = "/usr/bin/grep";
    cmds[1].arguments = (char *[]){"grep", "\\.c$", NULL};

    cmds[2].command = "/usr/bin/wc";
    cmds[2].arguments = (char *[]){"wc", "-w", NULL};
#endif

#if test == 2
    cmds[1].command = "/usr/bin/wc";
    cmds[1].arguments = (char *[]){"wc", "-w", NULL};
#endif

    int curr[2];
    int next[2];
    int fd[2];

    if(pipe(curr) < 0)
    {
        printf("Error opening pipe 1\n");
        exit(0);
    }
    if(pipe(next) < 0)
    {
        printf("Error opening pipe 2\n");
        exit(0);
    }

    int number_of_pipes = len;

    for (int i = 0; i < number_of_pipes; i++)
    {
        if(i % 2 == 0)
        {
            if(i == 0) fd[0] = in;
            else fd[0] = curr[0];

            if(i == number_of_pipes - 1) fd[1] = out;
            else fd[1] = next[1];
        }
        else
        {
            fd[0] = next[0];

            if(i == number_of_pipes - 1) fd[1] = out;
            else fd[1] = curr[1];
        }
        if (fork() == 0)
        {
            printf("Child process: %d\n", i);
            dup2(fd[0], in);
            dup2(fd[1], out);
            
            if (execve(cmds[i].command, cmds[i].arguments, envp) < 0) {
                printf("Execve failed %d\n", i);
            }
        }
        else
        {
            close(fd[0]);
            close(fd[1]);
        }
    }
    int i = 0;
    while (i < len) {
        wait(NULL);
        printf("wait child %d\n", i);
        i++;
    }
}