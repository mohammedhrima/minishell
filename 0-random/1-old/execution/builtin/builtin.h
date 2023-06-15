#ifndef BUILTIN_H
#define BUILTIN_H
#include "../../header.h"

void echo_func(char **arguments);
void cd_func(char **arguments);
void new_envirement_variable(char **arguments);
void export_func(char **arguments);
void unset_func(char **argument);
void env_func(char **arguments);
void exit_func(char **arguments);
void pwd_func(char **arguments);
void *get_built_in_func(char *cmd);

#endif