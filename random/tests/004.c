#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void swap(int *left, int *right)
{
    int tmp = *left;
    

}

int main(int argc , char *argv[], char **envp) {
    int pipes[2];
    int tmp;

    if(pipe(pipes) < 0)
    {
        printf("Error openning pipes\n");
        exit(0);
    }

    pid_t pid1 = fork();
    if (pid1 == 0) {
        // Child process 1
        dup2(pipes[1], STDOUT_FILENO); // Redirect stdout to pipe 1 write end

        close(pipes[0]); // Close unused read end of pipe 1
        close(pipes[1]); // Close pipe 1 write end

        // Execute command 1
        char *cmd = "/bin/ls";
        char *args[] = {"ls", "-la", NULL};
        if(execve(cmd, args, envp) < 0)
        {
            printf("Execve failed 1\n");
        }
        // exit(1);
    }

    // tmp = pipes[0];
    // pipes[0] = pipes[1];
    // pipes[1] = tmp;

    int *tmp = &pipes[0];
    

    pid_t pid2 = fork();
    if (pid2 == 0) {
        // Child process 2
        // close(pipes[1]); // Close pipe 1 write end
        dup2(pipes[1], STDIN_FILENO); // Redirect stdin to pipe 1 read end
        dup2(pipes[0], STDOUT_FILENO); // Redirect stdout to pipe 2 write end

        close(pipes[0]); // Close pipe 1 read end
        close(pipes[1]); // Close unused read end of pipe 2

        char *cmd = "/usr/bin/grep";
        char *args[] = {"grep", "\\.c$", NULL};
        if(execve(cmd, args, envp) < 0)
        {
            printf("Execve failed 2\n");
        }
    }

    // tmp = pipes[0];
    // pipes[0] = pipes[1];
    // pipes[1] = tmp;

    pid_t pid3 = fork();
    if (pid3 == 0) {
        // Child process 3
        // close(pipes[3]); // Close pipe 2 write end
        dup2(pipes[0], STDIN_FILENO); // Redirect stdin to pipe 2 read end
        
        close(pipes[0]); // Close pipe 2 read end
        close(pipes[1]);

        // Execute command 3
        char *cmd = "/usr/bin/wc";
        char *args[] = {"wc", "-w", NULL};
        if(execve(cmd, args, envp) < 0)
        {
            printf("Execve failed 3\n");
        }
    }

    // Parent process
    close(pipes[0]); // Close unused read end of pipe 1
    close(pipes[1]); // Close unused write end of pipe 1

    // Wait for all child processes to complete
    waitpid(pid1, NULL, 0);
    printf("pid1 finished\n");
    waitpid(pid2, NULL, 0);
    printf("pid2 finished\n");
    waitpid(pid3, NULL, 0);
    printf("pid3 finished\n");

    return 0;
}
