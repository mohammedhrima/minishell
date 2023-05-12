ls -> ls
l's' -> ls
l"s" -> ls
"$PATH"ls


starts from left " keep going till right ", if found 

+ when evaluating:
    - evaluate, if it's a variable: replace it with its content 
        if it's between ""
    - while there isn't space evaluate current and join it 
        to the next

+ store env somewhere
+ $ get what after '$' and search for it in env, and return it
+ unset remove from env
+ export:
    + env and export share the same data
    + add variable to stock if it exists 
        only modify else create it
    + if call export -> print declare -x + print everything 
        (has value or don't have)
    + if call env -> print only that has value

+ step 1:
do evualation by returning it's content as string