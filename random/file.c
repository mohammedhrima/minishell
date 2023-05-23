#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

// rl_clear_history
// rl_on_new_line
// rl_replace_line
// rl_redisplay

int main()
{
    // readline

    char *input = NULL;

    while (input == NULL || strcmp(input, "close"))
    {
        input = readline("> ");
        // add_history
        if (input)
            add_history(input);
        printf("input: %s\n", input);
        free(input);
    }
    
    return 0;
}
