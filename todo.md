+ create  list of exit_codes always check last exit status
+ built in function must att to exit code list (how about create child process and exit with 0)


|| : check all previous status, if ones fails it get executed
&& : check only th previous one

+ Value should return it's pid if command
+ remove ft_printf att the end, keep printf instead

      <             >              <<        >>
redirect input, redirect output, heredoc, append to file, export

echo, cd, pwd, export, unset, env (command), exit

bonus:
    && || 
    *  (will be handled using readir, opendir)

+ verify norm before pushing
+ check all opens, read, fork if they fail
+ set all pointers to NULL at beginning
+ add minishell to all error messages