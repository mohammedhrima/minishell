ls -> ls
l's' -> ls
l"s" -> ls

eval what after $

' : preserves the literal value of each character withen the quote like it doesn't exists
" : preserves the literal value of all characters with the exception of $ ` \

starts from left " keep going till right ", if found 

+ when evaluating:
    evaluate, if it's a variable: replace it with its content if it's between ""
    while there isn't space evaluate current and join it to the next

+ store env somewhere
+ $ get what after '$' and search for it in env, and return it
+ unset remove from env
+ export:
    + env and export share the same data
    + add variable to stock if it exists only modify else create it
    + if call export -> print declare -x + print everything (has value or don't have)
    + if call env -> print only that has value

    