+ split env store it array of tokens
+ store env somewhere
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

+ use realloc:
    will be used to hold address
    will be used to e
    
+ loop throw env:
    + build nodes of assignement
    + add them to an array named env