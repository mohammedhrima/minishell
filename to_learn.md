+ linux shell made in 1991
+ Parts:
    1- Learning the shell
    2- Configuring and the envirement
    3- common tasks and essential tools
    4- writing shell scripts

+ bash: Burn Again Shell
+ command history remember last 500 command by default
+ bash organizes all directories in some sort of tree

+ Commands in linux are treated as follow:
    command -option arguments

+ Changing current directory:
    cd + pathname: pathname will be considred as root of the tree to the desired location
    example:
        /a/b/c/d
        a is parent of b  (b is child of a)
        b is parrent of c
        ...

TODO
+ List directory:
    when ls takes multiple arguments:
        + it prints there name + new line + there content (will need that to simulate *)


+ do redirection even without command
    + > out : will create a file (that's it)
    + cmd >> out: will append to file / if it doesn't exists it will create it

TODO
    try using somewhere:
        dev/random
        dev/null

FIXME
    * is not expandable

FIXME  
    how bash handle input:
        1st loop:
            1- check " and ' if arrent closed
            2- check && || | if they aren't folowed by something
            2- wait if none of the above is closed

        2nd loop:
            1- tokenize: (build 1st array of tokens)

        3rd loop: split tokens that way
            1- command with it arguments (1st one always could be command, or built in)
            the rest is arguments

        4th loop: (build 2rd array of tokens)
            1- whenever you find export add what after to env
            2- whenever you find $ expand the value after it
            3- whenever you find * expand it
        
        5th loop:
            seperate with && and || and |
            sepreate also with redirections

        6th loop:
            execute

Page 100