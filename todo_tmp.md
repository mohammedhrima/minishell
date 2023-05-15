+ $ get what after '$' and search for it in env, and return it
+ unset remove from env
+ export:
    + without arguments:
        + print (declare -x) + what in env 
    + with arguments:
        + if assignement
            + if exists:
                - modify it
            + else:
                - add variable to env
+ env:
    + print only those who have value

+ step 1:
    do evualation by returning it's content as string

evaluate: return var
    + var can have type command_
    + type pid_

+ plan changed:
    Token will have:
        char *command
        int isexpandable
        int join_len (see if you can remove it)
        int arguments_len (see if you can remove it)

+ in $ :
    - check left
    - expand it
    - return expanded left
    - check if has left to join them (risky !!!)

+ Value should return it's pid if command