#include "utils.h"

void handle_signal(int signum)
{
    if (signum == CTRL_C)
    {
        printf("\n");
        rl_replace_line("", 0);
        rl_on_new_line();
        rl_redisplay();
    }
    if (signum == CTRL_SLASH)
    {
        printf("");
        rl_replace_line("", 0);
        rl_on_new_line();
        rl_redisplay();
    }
}

void handle_heredoc_signal(int signum)
{
    if (signum == CTRL_C)
    {
        printf("\n");
        rl_replace_line("", 0);
        rl_on_new_line();
        rl_redisplay();
        ft_exit(SUCCESS);
    }
    if (signum == CTRL_SLASH)
    {
        printf("");
        rl_replace_line("", 0);
        rl_on_new_line();
        rl_redisplay();
    }
}
