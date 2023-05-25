rm -rf a.out
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/Users/mhrima/Desktop/minishell/readline/lib
gcc minishell.c utils.c -L./readline/lib -lreadline -I./readline/include -fsanitize=address -fsanitize=null -g3
# gcc minishell.c utils.c -L./readline/lib -lreadline -I./readline/include

./a.out
rm -rf a.out