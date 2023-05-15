#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[], char **envp) {
    int fd[2];
    if (pipe(fd) == -1) {
        return 0;
    }
    
    int pid1 = fork();
    if (pid1 < 0) {
        return 2;
    }
    
    if (pid1 == 0) {
        // Child process 1 (ping)
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        char *cmd1 = "/bin/ls";
        char *args1[] = {"ls", "-la", NULL};
        if(execve(cmd1, args1, envp) < 0)
        {
            printf("Execve failed 1\n");
        }
    }
    
    int pid2 = fork();
    if (pid2 < 0) {
        return 3;
    }
    
    if (pid2 == 0) {
        // Child process 2 (grep)
        dup2(fd[0], STDIN_FILENO);
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
    
    return 0;
}