#include <stdio.h>
#include <dirent.h>

#if 0
int main() {
    DIR* dir = opendir("./");
    if (dir == NULL) {
        perror("Unable to open directory");
        return 1;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
    return 0;
}
#else

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char *argv[], char *envp[])
{

    char *array[] = {"/bin/ls", "random", NULL};
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

#endif

