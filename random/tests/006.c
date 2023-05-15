#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdlib.h>

/*
how to use
#> gcc 006.c
#> cat /tmp/file2 
apple
orange
apple
mango
apple
orange
guava
kiwi
#>./a.out 'cat /tmp/file2 | sort | uniq -c | sort -bgr'
       3 apple
       2 orange
       1 mango
       1 kiwi
       1 guava
*/

void execute_command(char *command)
{
    int i = 0;
    int argument_count = 0;

    /* Strip white spaces */
    while (command[i] == ' ')
    {
        i++;
    }
    command = command + i;

    i = 0;

    /* Count the number of arguments to the command */
    while (command[i] != '\0')
    {
        if (command[i] == ' ')
            argument_count++;
        i++;
    }

    char **argv = calloc(argument_count + 2, sizeof(char *));
    char *argument = NULL;
    i = 0;
    while ((argument = strsep(&command, " ")) != NULL)
    {
        if (strlen(argument) != 0)
        {
            argv[i] = calloc(strlen(argument) + 1, sizeof(char));
            strncpy(argv[i], argument, strlen(argument));
        }
        i++;
    }
    /* Need to set the last argument as NULL */
    argv[i] = NULL;

    if (execvp(argv[0], argv) != 0)
    {
        fprintf(stderr, "Error creating pipe. %s", strerror(errno));
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage pipe <commands to execute>");
        exit(-1);
    }

    int *fd = calloc(2, sizeof(int));
    if (pipe(fd) != 0)
    {
        printf("Error creating pipe. %s", strerror(errno));
        exit(errno);
    }
    const char *command = argv[1];
    int prev_commands_length = 0;
    int i = 0;
    int quote_begin = 0;
    while (1)
    {
        if (command[i] == '|')
        {
            /*  End of a command */
            int pid = fork();
            if (pid == -1)
            {
                printf("Error forking. %s", strerror(errno));
                exit(errno);
            }
            else if (pid > 0)
            {
                /*
                    Parent will take care of command seen.
                    And send its output to child.
                 */
                dup2(fd[1], 1);
                close(fd[0]);
                close(fd[1]);
                char *current_command = calloc(i + 1 - prev_commands_length, sizeof(char));
                strncpy(current_command, command + prev_commands_length, i - prev_commands_length);
                execute_command(current_command);
            }
            else
            {
                dup2(fd[0], 0);
                close(fd[1]);
                close(fd[0]);
                /* Create new pipe for chaining the next two commands */
                fd = calloc(2, sizeof(int));
                pipe(fd);
            }
            prev_commands_length = i + 1;
        }
        else if (command[i] == '\0')
        {
            char *current_command = calloc(i + 1 - prev_commands_length, sizeof(char));
            strncpy(current_command, command + prev_commands_length, i - prev_commands_length);
            execute_command(current_command);
        }
        i++;
    }
}



























#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct {
    char *command;
    char **arguments;
} cmd;

#define len 3

int main(int argc, char *argv[], char **envp) {
    cmd cmds[len];
    cmds[0].command = "/bin/ls";
    cmds[0].arguments = (char *[]){"ls", "-la", NULL};

    cmds[1].command = "/usr/bin/grep";
    cmds[1].arguments = (char *[]){"grep", "\\.c$", NULL};

    cmds[2].command = "/usr/bin/wc";
    cmds[2].arguments = (char *[]){"wc", "-w", NULL};

    int fd[2];
    if (pipe(fd) != 0) {
        printf("Error opening pipe\n");
        exit(0);
    }

    int i = 0;
    while (i < len) {
        int pid = fork();
        if (pid < 0) {
            printf("Failed forking\n");
            exit(0);
        } else if (pid == 0) {
            printf("Execute: %s\n", cmds[i].command);
            // child process
            if (i + 1 < len)
                dup2(fd[1], STDOUT_FILENO);
            if (i > 0)
                dup2(fd[0], STDIN_FILENO);
            close(fd[0]);
            close(fd[1]);
            if (execve(cmds[i].command, cmds[i].arguments, envp) < 0) {
                printf("Execve failed %d\n", i);
            }
        } else {
            // parent process
            close(fd[0]);
            close(fd[1]);
            if (i < len - 1) {
                if (pipe(fd) != 0) {
                    printf("Error opening pipe\n");
                    exit(0);
                }
            }
        }
        i++;
    }

    i = 0;
    while (i < len) {
        wait(NULL);
        i++;
    }

    return 0;
}














