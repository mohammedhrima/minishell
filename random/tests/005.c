#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

enum PIPES
{
    READ,
    WRITE
};

int main(int argc, char **argv)
{
    int hpipe[2];
    pipe(hpipe);

    if (fork())
    {
        /*parent*/
        char ch;
        close(hpipe[WRITE]);
        dup2(hpipe[READ], 0);
        close(hpipe[READ]);
        while ((ch = fgetc(stdin)) != EOF)
        {
            fprintf(stdout, "parent rec->%c\n", ch);
        }
    }
    else
    {
        /*child*/
        char ch;
        int hpipe2[2];
        pipe(hpipe2);

        close(hpipe[READ]);
        dup2(hpipe[WRITE], 1);
        close(hpipe[WRITE]);

        if (fork())
        {
            close(hpipe2[WRITE]);
            dup2(hpipe2[READ], 0);
            close(hpipe2[READ]);

            while ((write(1, (char *)&ch, read(0, (char *)&ch, sizeof(char)))))
            {
                fprintf(stderr, "middle child rec->%c\n", ch);
            }
        }
        else
        {
            /*child2*/
            close(hpipe2[READ]);
            dup2(hpipe2[WRITE], 1);
            close(hpipe2[WRITE]);
            fputs("this is from way down the line", stdout);
        }
    }
    exit(EXIT_SUCCESS);
}