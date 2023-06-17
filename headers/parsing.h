#ifndef PARSING_H
#define PARSING_H

#include "minishell.h"
int build_tokens(char *text);
bool is_inclosed(char *text);
void reset(char *text);
void evaluate_input(char *text);
void handle_input(char *text);

#endif