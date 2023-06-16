#include "evaluate.h"

char	**build_args(int *len_ptr, int *hid_ptr, char **args, t_token *token)
{
	char	*argument;
	int		len;

	len = *len_ptr;
	args = ft_realloc(args, len * sizeof(char *), (len + 2) * sizeof(char *));
	argument = expand(token->value);
	if (ft_strchr(argument, '-') && ft_strchr(argument, 'a'))
		(*hid_ptr) = 1;
	if (argument && argument[0] == '.')
	{
		if (ft_strcmp(argument, ".") == 0 || ft_strcmp(argument, "..") == 0)
			args[len++] = argument;
		else if (*hid_ptr)
			args[len++] = argument;
	}
	else
		args[len++] = argument;
	args[len] = NULL;
	*len_ptr = len;
	return (args);
}

int	check_redirection_errors(t_token *token, bool is_child_process)
{
	int	pid;
	int	error;

	error = SUCCESS;
	pid = 0;
	if (token->type == star_)
	{
		printf("minishell: * ambiguous redirection\n");
		error = AMBIGIOUS_REDIRECTION;
	}
	else if (token->type != identifier_)
	{
		printf("minishell: Unexpected symbol after redirection\n");
		error = UNEXPECTED_ERROR;
	}
	if (error)
	{
		if (is_child_process == false)
			pid = fork();
		if (pid == 0)
			ft_exit(UNEXPECTED_ERROR);
	}
	return (error);
}

int	do_redirection(t_file *input, t_file *output, t_token *token, t_type type)
{
	if (type == redir_input)
		*input = *new_file(expand(token->value), NOT_OPENED, type);
	if (type == redir_output || type == append_)
		*output = *new_file(expand(token->value), NOT_OPENED, type);
	if (type == heredoc_)
		*input = *open_heredoc(input, expand(token->value));
	return (1);
}

bool	check_identifier_or_star(t_token **token_ptr, int *pos_ptr)
{
	t_token	*token;

	token = *token_ptr;
	if (token->type == identifier_ || token->type == star_)
	{
		if (token->type == star_)
		{
			(*pos_ptr)++;
			token = ((t_token **)global.tokens.pointers)[(*pos_ptr)];
		}
		*token_ptr = token;
		return (true);
	}
	return (false);
}

int	handle_redirection(int *redir_ptr, int *pos_ptr, t_file *in, t_file *out)
{
	t_type	type;
	t_token	*token;

	*redir_ptr = 1;
	type = token->type;
	token = ((t_token **)global.tokens.pointers)[*pos_ptr];
	if (check_redirection_errors(token, false))
		return (0);
	(*pos_ptr)++;
	return (do_redirection(in, out, token, type));
}

char	**get_arguments(t_node *node, t_file *in, t_file *out, int *redir_ptr)
{
	char	**args;
	int		i;
	int		len;
	int		pos;
	t_token	*token;
	t_type	type;
	int		hidden;

	args = NULL;
	i = -1;
	len = 0;
	pos = node->token->start;
	while (++i < node->token->len)
	{
		token = ((t_token **)global.tokens.pointers)[pos++];
		if (check_identifier_or_star(&token, &pos))
			args = build_args(&len, &hidden, args, token);
		else if (is_redirection(token->type))
			i += handle_redirection(redir_ptr, &pos, in, out);
	}
	return (args);
}

void	build_eval_args(t_node *node, t_file *input, t_file *output)
{
	char	**args;
	int		pid;
	int		there_is_redirection;

	args = NULL;
	there_is_redirection = 0;
	args = get_arguments(node, input, output, &there_is_redirection);
	if (args && args[0])
		execute(input, output, args);
	if (there_is_redirection)
	{
		pid = fork();
		add_number(&global.pids, pid);
		if (pid == 0)
		{
			check_redirection(input, output);
			ft_exit(SUCCESS);
		}
	}
}

void	evaluate_pipe(t_node *node, t_file *input, t_file *output)
{
	int	fd[2];

	if (pipe(fd))
		ft_exit(ERROR);
	add_number(&global.fds, fd[0]);
	add_number(&global.fds, fd[1]);
	evaluate(node->left, input, new_file(NULL, fd[1], redir_output));
	evaluate(node->right, new_file(NULL, fd[0], redir_input), output);
	close(fd[0]);
	close(fd[1]);
}

t_value	*evaluate_logic_operator(t_node *node, t_file *input, t_file *output)
{
	t_value	*value;
	int		status;

	value = evaluate(node->left, input, output);
	status = get_last_exit_code();
	if (status == 0 && node->token->type == and_)
		value = evaluate(node->right, input, output);
	if (status != 0 && node->token->type == or_)
		value = evaluate(node->right, input, output);
	return (value);
}

void	evaluate_parenteses(t_node *node, t_file *input, t_file *output)
{
	int		pid;
	int		i;
	int		pos;
	t_type	type;
	t_value	*value;
	t_token	*token;

	pid = fork();
	add_number(&global.pids, pid);
	if (pid == 0)
	{
		pos = node->token->start;
		i = -1;
		while (++i < node->token->len)
		{
			token = ((t_token **)global.tokens.pointers)[pos++];
			if (is_redirection(token->type))
			{
				type = token->type;
				token = ((t_token **)global.tokens.pointers)[pos];
				check_redirection_errors(token, true);
				pos++;
				i += do_redirection(input, output, token, type);
			}
		}
		if (output->type == redir_output)
		{
			open_file(output);
			write(output->fd, "", ft_strlen(""));
			close(output->fd);
			output->type = append_;
			output->fd = NOT_OPENED;
		}
		value = evaluate(node->left, input, output);
		while (global.pids.pos > 0)
			get_last_exit_code();
		ft_exit(SUCCESS);
	}
	waitpid(pid, NULL, 0);
}

t_value	*evaluate(t_node *node, t_file *input, t_file *output)
{
	t_type	type;

	type = node->token->type;
	if (is_redirection(type) || type == identifier_)
		build_eval_args(node, input, output);
	else if (type == pipe_)
		evaluate_pipe(node, input, output);
	else if (type == or_ || type == and_)
		return (evaluate_logic_operator(node, input, output));
	else if (type == lparent_)
		evaluate_parenteses(node, input, output);
	return (NULL);
}
