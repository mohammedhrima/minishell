rm -rf a.out
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/Users/mhrima/Desktop/minishell/readline/lib
DEBUG=$1

if [[ $DEBUG == 1 ]]; then
  gcc minishell.c utils.c -L./readline/lib -lreadline -I./readline/include -fsanitize=address -fsanitize=null -g3
else
  gcc minishell.c utils.c -L./readline/lib -lreadline -I./readline/include -D DEBUG=0
fi

./a.out
rm -rf a.out*
