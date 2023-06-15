#include "header.h"

t_list	*new_list(t_list *list, size_t size, t_type type)
{
	list->pos = 0;
	list->len = 100;
	list->type = type;
	list->size = size;
	if (list->type == pointers_)
		list->pointers = ft_calloc(list->len, list->size);
	if (list->type == integers_)
		list->integers = ft_calloc(list->len, list->size);
	return (list);
}

t_node	*new_node(t_token *token)
{
	t_node	*new;

	new = ft_calloc(1, sizeof(t_node));
	new->token = token;
	return (new);
}

t_token	*new_token(t_type type, char *source, int len)
{
	t_token	*new;

	new = ft_calloc(1, sizeof(t_token));
	new->type = type;
	if (source)
	{
		new->value = ft_calloc(len + 1, sizeof(char));
		ft_strncpy(new->value, source, len);
	}
	return (new);
};
t_file	new_file(char *name, int fd, t_type type)
{
	t_file	new;

	new.name = name;
	new.fd = fd;
	new.type = type;
	add_to_list(&s_global.fds, &new.fd);
	return (new);
}

void	handle_signal(int signum)
{
	if (signum == CTRL_C)
	{
		ft_printf(OUT, "\n");
		rl_replace_line("", 0);
		rl_on_new_line();
		rl_redisplay();
	}
	if (signum == CTRL_SLASH)
	{
		ft_printf(OUT, "");
		rl_replace_line("", 0);
		rl_on_new_line();
		rl_redisplay();
	}
}
void	handle_heredoc_signal(int signum)
{
	if (signum == CTRL_C)
		ft_exit(SUCCESS);
	if (signum == CTRL_SLASH)
	{
		ft_printf(OUT, "");
		rl_replace_line("", 0);
		rl_on_new_line();
		rl_redisplay();
	}
}

void	add_to_list(t_list *list, void *value)
{
	if (list->type == integers_)
		list->integers[list->pos++] = *((int *)value);
	if (list->type == pointers_)
		list->pointers[list->pos++] = value;
	if (list->pos + 10 > list->len)
	{
		list->len *= 2;
		if (list->type == integers_)
			list->integers = ft_realloc(list->integers, list->pos * list->size,
					list->len * list->size);
		if (list->type == pointers_)
			list->pointers = ft_realloc(list->pointers, list->pos * list->size,
					list->len * list->size);
	}
}

void	clear_list(t_list *list)
{
	t_type	type;

	type = list->type;
	if (type == integers_)
		ft_memset(list->integers, 0, list->len * list->size);
	else if (type == pointers_)
		ft_memset(list->pointers, 0, list->len * list->size);
	list->pos = 0;
}

void	build_envirement(char **envp)
{
	int		i;
	char	**array;
	t_node	*node;

	i = 0;
	while (envp && envp[i])
	{
		array = split_by_two(envp[i], '=');
		node = new_node(new_token(assign_, NULL, 0));
		node->left = new_node(new_token(identifier_, NULL, 0));
		node->left->token->value = array[0];
		node->right = new_node(new_token(identifier_, NULL, 0));
		node->right->token->value = array[1];
		add_to_list(&s_global.envirement, node);
		i++;
	}
}

int	is_inclosed(char *text)
{
	int	quote;
	int	is_operator;
	int	i;

	quote = 0;
	is_operator = 0;
	i = 0;
	while (text && text[i])
	{
		if (text[i] == '\'' && quote == 0)
			quote = '\'';
		else if (text[i] == '"' && quote == 0)
			quote = '"';
		else if (text[i] == quote)
			quote = 0;
		if (ft_strchr("&|", text[i]))
			is_operator = 1;
		else if (text[i] && text[i] != '\n')
			is_operator = 0;
		i++;
	}
	if (quote || is_operator)
		return (true);
	return (false);
}

void	add_to_addresses(void *pointer)
{
	void	*temporary;

	if (s_global.addresses.len == 0)
	{
		s_global.addresses.pos = 0;
		s_global.addresses.len = 100;
		s_global.addresses.pointers = malloc(s_global.addresses.len
				* sizeof(void *));
	}
	s_global.addresses.pointers[s_global.addresses.pos++] = pointer;
	if (s_global.addresses.pos + 10 > s_global.addresses.len)
	{
		s_global.addresses.len *= 2;
		temporary = malloc(s_global.addresses.len * sizeof(void *));
		ft_memcpy(temporary, s_global.addresses.pointers, s_global.addresses.pos
				* sizeof(void *));
		free(s_global.addresses.pointers);
		s_global.addresses.pointers = temporary;
	}
}

void	build_tokens(char *text)
{
	int				txt_pos;
	int				start;
	int				single_quotes;
	int				double_quotes;
	DIR				*curr_dir;
	struct dirent	*curr_dir_content;
	char			*new;
	int				i;
	t_type			type;

	txt_pos = 0;
	start = 0;
	while (text && text[txt_pos])
	{
		single_quotes = 0;
		double_quotes = 0;
		if (text[txt_pos] == '\0')
			break ;
		if (text[txt_pos] == '*')
		{
			add_to_list(&s_global.tokens, new_token(star_, text + txt_pos, 1));
			txt_pos++;
			new = getcwd(NULL, 0);
			curr_dir = opendir(new);
			free(new);
			curr_dir_content = readdir(curr_dir);
			while (curr_dir_content)
			{
				add_to_list(&s_global.tokens, new_token(identifier_,
							curr_dir_content->d_name,
							ft_strlen(curr_dir_content->d_name)));
				curr_dir_content = readdir(curr_dir);
			}
			closedir(curr_dir);
			continue ;
		}
		if (text[txt_pos] == ' ')
		{
			start = txt_pos;
			while (text[txt_pos] == ' ')
				txt_pos++;
			continue ;
		}
		i = 0;
		type = 0;
		while (s_global.values[i])
		{
			if (ft_strncmp(s_global.values[i], &text[txt_pos],
					ft_strlen(s_global.values[i])) == 0)
			{
				start = txt_pos;
				type = s_global.types[i];
				txt_pos += ft_strlen(s_global.values[i]);
				break ;
			}
			i++;
		}
		if (type)
		{
			add_to_list(&s_global.tokens, new_token(type, text + start, txt_pos
						- start));
			continue ;
		}
		start = txt_pos;
		while (text[txt_pos])
		{
			if (text[txt_pos] == '\'' && !double_quotes)
				single_quotes = !single_quotes;
			else if (text[txt_pos] == '"' && !single_quotes)
				double_quotes = !double_quotes;
			else if (!single_quotes && !double_quotes
					&& (ft_isspace(text[txt_pos]) || ft_strchr("|&<>()",
							text[txt_pos])))
				break ;
			txt_pos++;
		}
		add_to_list(&s_global.tokens, new_token(identifier_, text + start, txt_pos
					- start));
	}
	add_to_list(&s_global.tokens, new_token(end_, NULL, 0));
}

void	skip(t_type type)
{
	t_token	*token;

	token = ((t_token **)s_global.tokens.pointers)[s_global.tokens.pos];
	if (token->type != type)
	{
		ft_printf(OUT, "minishell: syntax error expected '%s'\n",
				type_to_string(type));
		ft_exit(SYNTAX_ERROR);
	}
	s_global.tokens.pos++;
}

t_node	*expr(void)
{
	return (and_or());
}

t_node	*and_or(void)
{
	t_node	*left;
	t_token	*token;
	t_node	*node;

	left = pipe_node();
	token = ((t_token **)s_global.tokens.pointers)[s_global.tokens.pos];
	while (token->type == and_ || token->type == or_)
	{
		node = new_node(token);
		skip(token->type);
		node->left = left;
		node->right = pipe_node();
		left = node;
		token = ((t_token **)s_global.tokens.pointers)[s_global.tokens.pos];
	}
	return (left);
}

t_node	*pipe_node(void)
{
	t_node	*left;
	t_token	*token;
	t_node	*node;

	left = prime();
	token = ((t_token **)s_global.tokens.pointers)[s_global.tokens.pos];
	while (token->type == pipe_)
	{
		node = new_node(token);
		skip(pipe_);
		node->left = left;
		node->right = prime();
		left = node;
		token = ((t_token **)s_global.tokens.pointers)[s_global.tokens.pos];
	}
	return (left);
}

bool	is_redirection(t_type type)
{
	return (type == redir_input || type == redir_output || type == heredoc_
		|| type == append_);
}

t_node	*prime(void)
{
	t_token	*token;
	t_node	*node;

	token = ((t_token **)s_global.tokens.pointers)[s_global.tokens.pos];
	if (token->type == identifier_ || token->type == star_
		|| is_redirection(token->type))
	{
		node = new_node(token);
		node->token->start = s_global.tokens.pos;
		node->token->len++;
		skip(token->type);
		token = ((t_token **)s_global.tokens.pointers)[s_global.tokens.pos];
		while (token->type == identifier_ || token->type == star_
			|| is_redirection(token->type))
		{
			node->token->len++;
			skip(token->type);
			token = ((t_token **)s_global.tokens.pointers)[s_global.tokens.pos];
		}
		return (node);
	}
	if (token->type == lparent_)
	{
		node = new_node(token);
		skip(token->type);
		node->left = expr();
		skip(rparent_);
		token = ((t_token **)s_global.tokens.pointers)[s_global.tokens.pos];
		if (token->type == identifier_ || token->type == star_)
		{
			ft_printf(OUT, "minishell: syntax error unexpected '%s'\n",
					token->value);
			ft_exit(SYNTAX_ERROR);
		}
		node->token->start = s_global.tokens.pos;
		node->token->len++;
		while (token->type == identifier_ || token->type == star_
			|| is_redirection(token->type))
		{
			node->token->len++;
			skip(token->type);
			token = ((t_token **)s_global.tokens.pointers)[s_global.tokens.pos];
		}
		return (node);
	}
	return (NULL);
}

char	*get_command_path(char *cmd)
{
	char	*res;
	int		i;

	if (cmd == NULL || ft_strchr(cmd, '/'))
		return (cmd);
	i = 0;
	while (s_global.path && s_global.path[i])
	{
		res = strjoin(s_global.path[i], "/", cmd);
		if (access(res, F_OK) == 0 && access(res, X_OK) == 0)
			return (res);
		i++;
	}
	return (cmd);
}

void	echo_func(int fd, char **arguments)
{
	int	print_new_line;
	int	i;

	print_new_line = 1, i = 0;
	while (arguments && arguments[i])
	{
		if (i == 0 && ft_strcmp(arguments[i], "-n") == 0)
			print_new_line = 0;
		else
			ft_printf(fd, "%s", arguments[i]);
		i++;
	}
	if (print_new_line)
		ft_printf(fd, "\n");
}

void	cd_func(int fd, char **arguments)
{
	if (arguments[0])
		chdir(arguments[0]); // verify if this little shit leaks
	else
		chdir(getenv("HOME")); // verify if it leaks
}

void	pwd_func(int fd, char **arguments)
{
	char	*new;

	new = getcwd(NULL, 0);
	ft_printf(fd, "%s\n", new);
	free(new);
}

void	export_func(int fd, char **arguments)
{
	char	**array;
	t_node	*node;
	t_node	**envirement;
	int		i;

	if (arguments[0])
	{
		array = split_by_two(arguments[0], '=');
		node = new_node(new_token(assign_, NULL, 0));
		node->left = new_node(new_token(identifier_, NULL, 0));
		node->left->token->value = array[0];
		node->right = new_node(new_token(identifier_, NULL, 0));
		node->right->token->value = array[1];
		add_to_list(&s_global.envirement, node);
		return ;
	}
	else
	{
		i = 0;
		envirement = (t_node **)s_global.envirement.pointers;
		while (envirement && envirement[i])
		{
			if (envirement[i]->left->token->value)
				ft_printf(fd, "declare -x %s=\"%s\"\n",
						envirement[i]->left->token->value,
						envirement[i]->right->token->value);
			i++;
		}
	}
}

void	unset_func(int fd, char **argument)
{
	int		i;
	t_node	**envirement;
	char	*value;

	envirement = (t_node **)s_global.envirement.pointers;
	i = 0;
	while (envirement && envirement[i])
	{
		value = envirement[i]->left->token->value;
		if (value && ft_strcmp(value, argument[0]) == 0)
		{
			free(envirement[i]->left->token->value);
			envirement[i]->left->token->value = NULL;
			free(envirement[i]->right->token->value);
			envirement[i]->right->token->value = NULL;
			break ;
		}
		i++;
	}
}

void	env_func(int fd, char **arguments)
{
	int		i;
	t_node	**envirement;

	envirement = (t_node **)s_global.envirement.pointers;
	i = 0;
	while (envirement[i])
	{
		if (envirement[i]->left->token->value)
			ft_printf(fd, "%s=%s\n", envirement[i]->left->token->value,
					envirement[i]->right->token->value);
		i++;
	}
}

void	exit_func(int fd, char **arguments)
{
	// to be verified because it's not working properly
	ft_printf(OUT, "exit with %d\n", ft_atoi(arguments[0]));
	ft_exit(ft_atoi(arguments[0])); // verify exit code after
}

void	*built_in(char *cmd)
{
	if (ft_strcmp(cmd, "echo") == 0)
		return (&echo_func);
	if (ft_strcmp(cmd, "cd") == 0)
		return (&cd_func);
	if (ft_strcmp(cmd, "pwd") == 0)
		return (&pwd_func);
	if (ft_strcmp(cmd, "export") == 0)
		return (&export_func);
	if (ft_strcmp(cmd, "unset") == 0)
		return (&unset_func);
	if (ft_strcmp(cmd, "env") == 0)
		return (&env_func);
	if (ft_strcmp(cmd, "exit") == 0)
		return (&exit_func);
	return (NULL);
}

char	*get_var(char *name)
{
	int		i;
	t_node	**nodes;

	nodes = (t_node **)s_global.envirement.pointers;
	i = 0;
	if (ft_strcmp(name, "?") == 0)
		return (ft_itoa(get_last_exit_code()));
	while (nodes && nodes[i])
	{
		if (ft_strcmp(nodes[i]->left->token->value, name) == 0)
			return (nodes[i]->right->token->value);
		i++;
	}
	return ("");
}

char	*expand(t_token *token)
{
	int		j;
	char	*var;
	char	*to_add;
	char	*res;
	int		i;
	int		single_quotes;
	int		double_quotes;
	char	*value;

	res = NULL;
	i = 0;
	single_quotes = 0;
	double_quotes = 0;
	value = token->value;
	while (value && value[i])
	{
		if (value[i] == '\'')
		{
			if (!double_quotes)
				single_quotes = !single_quotes;
			else
			{
				res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
				res[ft_strlen(res)] = value[i];
			}
			i++;
		}
		else if (value[i] == '"')
		{
			if (!single_quotes)
				double_quotes = !double_quotes;
			else
			{
				res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
				res[ft_strlen(res)] = value[i];
			}
			i++;
		}
		else if (token->value[i] == '$')
		{
			if (double_quotes || !single_quotes)
			{
				i++;
				j = i;
				while (token->value[j] && !ft_strchr(" \"", token->value[j]))
					j++;
				var = ft_calloc(j - i + 1, sizeof(char));
				ft_strncpy(var, token->value + i, j - i);
				to_add = get_var(var);
				res = ft_realloc(res, ft_strlen(res), ft_strlen(res)
						+ ft_strlen(to_add) + 2);
				ft_strncpy(res + ft_strlen(res), to_add, ft_strlen(to_add));
				i = j;
			}
			else if (single_quotes)
			{
				res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
				res[ft_strlen(res)] = token->value[i];
				i++;
			}
		}
		else
		{
			res = ft_realloc(res, ft_strlen(res), ft_strlen(res) + 2);
			res[ft_strlen(res)] = value[i];
			i++;
		}
	}
	return (res);
}

void	open_file(t_file *file)
{
	if (file->type == redir_input && file->name)
	{
		if (access(file->name, F_OK))
		{
			ft_printf(OUT, "minishell: %s: No such file or directory\n",
					file->name);
			ft_exit(NO_SUCH_FILE_OR_DIR);
		}
		else if (access(file->name, R_OK))
		{
			ft_printf(OUT, "minishell: %s: Permission denied\n", file->name);
			ft_exit(PERMISSION_DENIED);
		}
		else
			file->fd = open(file->name, O_RDONLY);
	}
	if (file->type == redir_output && file->name)
	{
		if (access(file->name, F_OK) == 0 && access(file->name, W_OK))
		{
			ft_printf(OUT, "minishell: %s: Permission denied\n", file->name);
			ft_exit(PERMISSION_DENIED);
		}
		else
			file->fd = open(file->name, O_WRONLY | O_CREAT | O_TRUNC);
	}
	if (file->type == append_ && file->name)
	{
		if (access(file->name, F_OK) == 0 && access(file->name, W_OK))
		{
			ft_printf(OUT, "minishell: %s: Permission denied\n", file->name);
			ft_exit(PERMISSION_DENIED);
		}
		else
			file->fd = open(file->name, O_WRONLY | O_CREAT | O_APPEND);
	}
}

int	get_last_exit_code(void)
{
	int			*pids;
	int			status;
	static int	res;

	pids = s_global.pids.integers;
	status = 0;
	res = 0;
	if (s_global.pids.pos > 0)
	{
		waitpid(pids[s_global.pids.pos--], &status, 0);
		res = WEXITSTATUS(status);
		return (res);
	}
	return (res);
}

t_value	*evaluate(t_node *node, t_file input, t_file output)
{
	t_type	type;
	t_token	*token;
	t_value	*value;
	int		i;
	void	(*func)(int, char **);
	int		*fds;
	int		pid;
	char	**arguments;
	int		len;
	int		pos;
	int		include_hidden;
	char	*argument;
	char	*filename;
	char	*delimiter;
	char	*res;
	int		fd[2];
	int		status;

	type = node->token->type;
	if (
		type == identifier_ || type == redir_input ||
		type == redir_output || type == heredoc_ || type == append_)
	{
		pid = 0;
		arguments = NULL;
		len = 0;
		pos = node->token->start;
		i = 0;
		include_hidden = 0;
		while (i < node->token->len)
		{
			token = ((t_token **)s_global.tokens.pointers)[pos];
			pos++;
			if (token->type == identifier_ || token->type == star_)
			{
				if (token->type == star_)
				{
					pos++;
					token = ((t_token **)s_global.tokens.pointers)[pos];
				}
				arguments = ft_realloc(arguments, len * sizeof(char *), (len
							+ 2) * sizeof(char *));
				argument = expand(token);
				if (argument[0] != '.' || (argument[0] == '.'
						&& include_hidden))
					arguments[len++] = argument;
				if (arguments[len - 1] && arguments[len - 1][0] == '-'
					&& ft_strchr(arguments[len - 1], '-'))
					include_hidden = 1;
				arguments[len] = NULL;
			}
			else if (is_redirection(token->type)) // handle * here too
			{
				type = token->type;
				token = ((t_token **)s_global.tokens.pointers)[pos];
				if (token->type == star_)
				{
					ft_printf(OUT, "minishell: *: ambiguous redirection\n");
					ft_exit(AMBIGIOUS_REDIRECTION);
				}
				else if (token->type != identifier_)
				{
					ft_printf(OUT, "minishell: Unexpected symbol after redirection\n");
					ft_exit(UNEXPECTED_ERROR);
				}
				pos++;
				i++;
				filename = expand(token);
				if (type == redir_input)
					input = new_file(filename, NOT_OPENED, type);
				if (type == redir_output || type == append_)
					output = new_file(filename, NOT_OPENED, type);
				if (type == heredoc_)
				{
					delimiter = filename;
					input = new_file(ft_strdup("/tmp/heredoc"),
										open("/tmp/heredoc",
												O_WRONLY | O_CREAT | O_TRUNC),
										redir_output);
					pid = fork();
					if (pid == 0)
					{
						signal(SIGINT, handle_heredoc_signal);
						signal(SIGQUIT, handle_heredoc_signal);
						res = NULL;
						while ((res == NULL || ft_strcmp(res, delimiter)))
						{
							res = readline("heredoc $> ");
							if (res)
								write(input.fd, res, ft_strlen(res));
						}
						ft_exit(SUCCESS);
					}
					waitpid(pid, &status, 0);
					close(input.fd);
					input.type = redir_input;
					input.fd = NOT_OPENED;
				}
			}
			i++;
		}
		if (arguments && arguments[0])
		{
			void (*func)(int, char**) = built_in(arguments[0]);
			if (func)
			{
				// if inside_pipe fork
				if (s_global.inside_pipe)
				{
					pid = fork();
					add_to_list(&s_global.pids, &pid);
					if (pid == 0)
					{
						if (input.type == redir_input)
						{
							open_file(&input);
							dup2(input.fd, IN);
							close(input.fd);
						}
						if (output.type == redir_output
							|| output.type == append_)
						{
							open_file(&output);
							dup2(output.fd, OUT);
							close(output.fd);
						}
						func(OUT, &arguments[1]);
						ft_exit(SUCCESS);
					}
				}
				else
				{
					if (input.type == redir_input)
					{
						open_file(&input);
						dup2(input.fd, IN);
						close(input.fd);
					}
					if (output.type == redir_output || output.type == append_)
					{
						open_file(&output);
						dup2(output.fd, OUT);
						close(output.fd);
					}
					func(output.fd, &arguments[1]);
				}
			}
			else
			{
				arguments[0] = get_command_path(arguments[0]);
				pid = fork();
				add_to_list(&s_global.pids, &pid);
				if (pid == 0)
				{
					if (input.type == redir_input)
					{
						open_file(&input);
						dup2(input.fd, IN);
					}
					if (output.type == redir_output || output.type == append_)
					{
						open_file(&output);
						dup2(output.fd, OUT);
					}
					fds = s_global.fds.integers;
					i = 0;
					while (i < s_global.fds.pos)
					{
						if (fds[i] != OUT && fds[i] != IN)
							close(fds[i]);
						i++;
					}
					while (s_global.pids.pos > 0)
						get_last_exit_code();
					// when argument[1]=NULL, an error happen
					// when argument[1] and you give &argument[0] as argument, somwthing enexpectabe happen
					// check exit code if command not valid
					if (execve(arguments[0], &arguments[0], s_global.env) != 0)
					{
						ft_printf(IN, "minishell: '%s' command not found\n", arguments[0]);
						ft_exit(COMMAND_NOT_FOUND);
					}
				}
			}
		}
		return (node->token);
	}
	else if (type == pipe_)
	{
		if (pipe(fd) < 0)
			ft_printf(ERR, "Error opening pipe\n");
		add_to_list(&s_global.fds, &fd[0]);
		add_to_list(&s_global.fds, &fd[1]);
		s_global.inside_pipe++;
		evaluate(node->left, input, new_file(NULL, fd[1], redir_output));
		evaluate(node->right, new_file(NULL, fd[0], redir_input), output);
		s_global.inside_pipe--;
		fds = s_global.fds.integers;
		close(fds[--s_global.fds.pos]);
		close(fds[--s_global.fds.pos]);
	}
	else if (type == or_ || type == and_)
	{
		value = evaluate(node->left, input, output);
		status = get_last_exit_code();
		if (status == 0 && node->token->type == and_)
			value = evaluate(node->right, input, output);
		if (status != 0 && node->token->type == or_)
			value = evaluate(node->right, input, output);
		return (value);
	}
	else if (type == lparent_)
	{
		pos = node->token->start;
		i = 0;
		while (i < node->token->len)
		{
			token = ((t_token **)s_global.tokens.pointers)[pos];
			pos++;
			if (is_redirection(token->type))
			{
				type = token->type;
				token = ((t_token **)s_global.tokens.pointers)[pos];
				if (token->type == star_)
				{
					ft_printf(OUT, "minishell: *: ambiguous redirection\n");
					ft_exit(AMBIGIOUS_REDIRECTION);
				}
				else if (token->type != identifier_)
				{
					ft_printf(OUT, "minishell: Unexpected symbol after redirection\n");
					ft_exit(UNEXPECTED_ERROR);
				}
				pos++;
				i++;
				filename = expand(token);
				if (type == redir_input)
					input = new_file(filename, NOT_OPENED, type);
				if (type == redir_output || type == append_)
					output = new_file(filename, NOT_OPENED, type);
				if (type == heredoc_)
				{
					// print error if filename is NULL;
					delimiter = filename;
					input = new_file(ft_strdup("/tmp/heredoc"),
										open("/tmp/heredoc",
												O_WRONLY | O_CREAT | O_TRUNC),
										redir_output);
					pid = fork();
					if (pid == 0)
					{
						signal(SIGINT, handle_heredoc_signal);
						signal(SIGQUIT, handle_heredoc_signal);
						res = NULL;
						while ((res == NULL || ft_strcmp(res, delimiter)))
						{
							res = readline("heredoc $> ");
							if (res)
								write(input.fd, res, ft_strlen(res));
						}
						ft_exit(SUCCESS);
					}
					waitpid(pid, &status, 0);
					close(input.fd);
					input.type = redir_input;
					input.fd = NOT_OPENED;
				}
			}
			i++;
		}
		if (output.type == redir_output && output.name)
		{
			open_file(&output);
			write(output.fd, "", ft_strlen(""));
			close(output.fd);
			output.type = append_;
			output.fd = NOT_OPENED;
		}
		return (evaluate(node->left, input, output));
	}
	return (NULL);
}

int	main(int argc, char **argv, char **envp)
{
	char	*text;
	int		i;
	t_file	input;
	t_file	output;
	char	*tmp;
	t_token	**tokens;
	int status;

	s_global.values = (char *[]){"<<", ">>", "<", ">", "&&", "||", "(", ")", "|", "*", 0};
	s_global.types = (t_type[]){heredoc_, append_, redir_input, redir_output, and_, or_, lparent_, rparent_, pipe_, star_, 0};
	s_global.env = envp;
	new_list(&s_global.envirement, sizeof(t_node *), pointers_);
	new_list(&s_global.tokens, sizeof(t_token *), pointers_);
	new_list(&s_global.pids, sizeof(int), integers_);
	new_list(&s_global.fds, sizeof(int), integers_);
	i = 0;
	while (envp && envp[i])
	{
		if (ft_strncmp("PATH", envp[i], ft_strlen("PATH")) == 0)
		{
			s_global.path = split(envp[i], ":");
			break ;
		}
		i++;
	}
	build_envirement(envp);
	clear_list(&s_global.tokens);
	while (1)
	{
		signal(SIGINT, handle_signal);
		signal(SIGQUIT, handle_signal);
		input = new_file(NULL, IN, 0);
		output = new_file(NULL, OUT, 0);
		text = readline("minishell $> ");
		add_to_addresses(text);
		if (is_inclosed(text))
		{
			text = ft_realloc(text, ft_strlen(text), ft_strlen(text) + 2);
			text[ft_strlen(text)] = '\n';
			tmp = text;
			while (tmp && is_inclosed(text))
			{
				tmp = ft_readline("> ");
				text = ft_realloc(text, ft_strlen(text), ft_strlen(text)
						+ ft_strlen(tmp) + 1);
				ft_strcpy(text + ft_strlen(text), tmp);
			}
			text[ft_strlen(text) - 1] = 0;
		}
		if (text)
			add_history(text);
		build_tokens(text);
		// exit(0);
		s_global.tokens.pos = 0;
		tokens = (t_token **)s_global.tokens.pointers;
		while (tokens[s_global.tokens.pos]->type != end_)
			evaluate(expr(), input, output);
		while (s_global.pids.pos > 0)
			status = get_last_exit_code();
		if (text == NULL)
			break ; // exit if text is NULL
		text = NULL;
		clear_list(&s_global.tokens);
		clear_list(&s_global.pids);
		clear_list(&s_global.fds);
	}
	ft_exit(0);
}
