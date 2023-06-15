#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>

int main(void)
{
    int pid = fork();
    if(pid == 0)
    {
        while(1)
        {
            usleep(10000);
            printf("Child process\n");
        }
    }
    usleep(100000);
    kill(pid, SIGKILL);
    printf("main process\n");
}
