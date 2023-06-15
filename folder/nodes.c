#include "../header.h"

int	skip(t_type type)
{
	t_token	*token;
	int		pid;

	token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
	if (token->type != type)
	{
		printf("minishell: syntax error expected '%s'\n", type_to_string(type));
		return (SYNTAX_ERROR);
	}
	global.tokens.pos++;
	return (SUCCESS);
}

bool	check(t_type type, t_type types[])
{
	int	i;

	i = 0;
	while (types[i])
	{
		if (types[i] == type)
			return (true);
		i++;
	}
	return (false);
}

bool	is_redirection(t_type type)
{
	return (type == redir_input || type == redir_output || type == heredoc_
		|| type == append_);
}

int	check_token_type(t_token *token)
{
	int	pid;
	int	pos;

	if (token->type == identifier_ || token->type == star_)
	{
		pid = fork();
		add_number(&global.pids, pid);
		printf("minishell: syntax error unexpected '%s'\n", token->value);
		if (pid == 0)
			ft_exit(SYNTAX_ERROR);
		pos = global.tokens.pos;
		while (((t_token **)global.tokens.pointers)[pos]->type != end_)
			pos++;
		global.tokens.pos = pos;
		return (SYNTAX_ERROR);
	}
	return (SUCCESS);
}

t_node	*advance(t_node *node)
{
	t_token	*token;

	token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
	while (token->type == identifier_ || token->type == star_
		|| is_redirection(token->type))
	{
		node->token->len++;
		skip(token->type);
		token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
	}
	return (node);
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
	token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
	while (left && (token->type == and_ || token->type == or_))
	{
		node = new_node(token);
		skip(token->type);
		node->left = left;
		node->right = pipe_node();
		left = node;
		token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
	}
	return (left);
}

t_node	*pipe_node(void)
{
	t_node	*left;
	t_token	*token;
	t_node	*node;

	left = prime();
	token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
	while (left && token->type == pipe_)
	{
		node = new_node(token);
		skip(pipe_);
		node->left = left;
		node->right = prime();
		left = node;
		token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
	}
	return (left);
}

t_node	*prime(void)
{
	t_token	*token;
	t_node	*node;

	token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
	if (token->type == identifier_ || token->type == star_
		|| is_redirection(token->type))
	{
		node = new_node(token);
		node->token->start = global.tokens.pos;
		node->token->len++;
		skip(token->type);
		return (advance(node));
	}
	return (parentses());
}

t_node	*parentses(void)
{
	t_token	*token;
	t_node	*node;
	int		pid;

	token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
	if (token->type == lparent_)
	{
		node = new_node(token);
		skip(token->type);
		node->left = expr();
		token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
		if (skip(rparent_))
			return (NULL);
		token = ((t_token **)global.tokens.pointers)[global.tokens.pos];
		if (check_token_type(token))
			return (NULL);
		node->token->start = global.tokens.pos;
		return (advance(node));
	}
	return (NULL);
}
