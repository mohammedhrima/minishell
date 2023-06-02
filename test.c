#include<stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
// #include "libft/libft.h"
#include "test.h"
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

enum e_token_type
{
	TOKEN_AND = 256,
	TOKEN_OR,
	TOKEN_WORD,
	TOKEN_APPEND,
	TOKEN_HEREDOC,
};

typedef struct s_token
{
	int type;
	int start;
	int end;
	char *word;
	struct s_token *next;
}	t_token;


typedef struct s_node
{
	int type;
	t_token *token;
	t_token *args;
	struct s_node *left;
	struct s_node *right;
	int arg_count;
}	t_node;

typedef struct s_process
{
	int id;
	struct s_process *next;
} t_process;

typedef struct s_pipe
{
	int in;
	int out;
	struct s_pipe *next;
} t_pipe;

typedef struct s_var
{
	char *name;
	char *value;
	struct s_var *next;
} t_var;

typedef struct s_msh
{
	t_token *first_token;	
	t_token *ct;
	t_node *root;
	int failed;
	char **env;

	t_pipe *first_pipe;
	t_pipe *last_pipe;
	t_process *first_process;
	t_process *last_process;
	t_var *first_var;
} t_msh;

int get_token_type(char *s)
{
	char	c1;
	char	c2;

	c1 = s[0];
	c2 = s[1];
	if (c1 == '&' && c2 == '&')
		return TOKEN_AND;
	else if (c1 == '|' && c2 == '|')
		return TOKEN_OR;
	else if (c1 == '<' && c2 == '<')
		return TOKEN_HEREDOC;
	else if (c1 == '>' && c2 == '>')
		return TOKEN_APPEND;
	else if (c1 && ft_strchr("()<>|", c1))
		return c1;
	else
		return TOKEN_WORD;
}

void tokenize(t_msh *msh, char *s)
{
	t_token *head = 0;
	t_token  *last = 0;
	int	i;

	i = 0;
	while (s[i])
	{
		while (ft_isspace(s[i]))
			i++;
		if (!s[i])
			break ;
		t_token *token = calloc(1, sizeof(*token));
		if (!head)
			head = last = token;
		else
			last = last->next = token;
		token->type = get_token_type(s + i);
		token->start = i;
		if (token->type == TOKEN_WORD)
		{
			int inside_double_quotes = 0;
			int inside_single_quotes = 0;

			while (s[i])
			{
				if (s[i] == '"')
				{
					if (!inside_single_quotes)
						inside_double_quotes = !inside_double_quotes;
					i++;
				}
				else if (s[i] == '\'')
				{
					if (!inside_double_quotes)
						inside_single_quotes = !inside_single_quotes;
					i++;
				}
				else if (s[i] == '$' && !inside_single_quotes)
				{
					i++;
					while (s[i] && !isspace(s[i]) && get_token_type(s + i) == TOKEN_WORD
							&& s[i] != '"' && s[i] != '\'')
						i++;
				}
				else
				{
					if (!inside_double_quotes && !inside_single_quotes &&
							(isspace(s[i]) || get_token_type(s + i) != TOKEN_WORD))
						break ;
					i++;
				}
			}
			if (inside_double_quotes || inside_single_quotes)
			{
				fprintf(stderr, "missing single or double quotes\n");
				msh->failed = 1;
				return ;
			}
		}
		else
		{
			if (token->type < 256)
				i++;
			else
				i += 2;
		}
		token->end = i;
		token->word = malloc(i - token->start + 1);
		memcpy(token->word, s + token->start, i - token->start);
		token->word[i - token->start] = 0;
	}
	if (!head)
		head = last = calloc(1, sizeof(t_token));
	else
		last = last->next = calloc(1, sizeof(t_token));
	last->next = last;
	msh->first_token = head;
}

t_node *make_node(t_msh *msh)
{
	t_node *node = calloc(1, sizeof(t_node));
	node->token = msh->ct;
	node->type = node->token->type;
	return node;
}

t_node *parse_and_or(t_msh *msh);
t_node *parse_pipe(t_msh *msh);
t_node *parse_cmd(t_msh *msh);

void parse(t_msh *msh)
{
	msh->ct = msh->first_token;
	msh->root = parse_and_or(msh);
}

t_node *parse_and_or(t_msh *msh)
{
	t_node *left = parse_pipe(msh);

	while (msh->ct->type == TOKEN_AND || msh->ct->type == TOKEN_OR)
	{
		t_node *node = make_node(msh);
		msh->ct = msh->ct->next;
		node->left = left;
		node->right = parse_pipe(msh);
		left = node;
	}
	return left;
}

t_node *parse_pipe(t_msh *msh)
{
	t_node *left = parse_cmd(msh);

	while (msh->ct->type == '|')
	{
		t_node *node = make_node(msh);
		msh->ct = msh->ct->next;
		node->left = left;
		node->right = parse_cmd(msh);
		left = node;
	}
	return left;
}

int is_command_type(int type)
{
	return  (type == TOKEN_WORD ||
			type == '<' ||
			type == '>' || 
			type == TOKEN_APPEND ||
			type == TOKEN_HEREDOC);
}

t_node *parse_cmd(t_msh *msh)
{
	t_node *node = 0;

	if (msh->ct->type == '(')
	{
		msh->ct = msh->ct->next;
		node = parse_and_or(msh);	
		if (msh->ct->type != ')')
		{
			fprintf(stderr, "expected ')'\n");
			msh->failed = 1;
		}
		else
			msh->ct = msh->ct->next;
	}
	else if (is_command_type(msh->ct->type))
	{
		node = make_node(msh);
		node->type = TOKEN_WORD;
		node->args = msh->ct;
		while (is_command_type(msh->ct->type))
		{
			if (msh->ct->type != TOKEN_WORD && 
					msh->ct->next->type != TOKEN_WORD)
			{
				fprintf(stderr, "parse error\n");
				msh->failed = 1;
			}
			msh->ct = msh->ct->next;
			node->arg_count++;
		}
	}
	else
	{
		fprintf(stderr, "parse error\n");
		msh->failed = 1;
		msh->ct = msh->ct->next;
	}
	return node;	
}

typedef struct s_exec_result
{
	pid_t pid;
	int status;
} t_exec_result;

typedef struct s_cmd
{
	char *command;
	char **args;
	int in;
	int out;
	int valid;
} t_cmd;



//check this function
char *get_fullpath(t_msh *msh, char *s)
{
	// handle errors, is a directory, not a directoty?
	if (ft_strchr(s, '/'))
	{
		struct stat path_stat;
		stat(s, &path_stat); 
		if (!access(s, F_OK) && !access(s, X_OK) && S_ISREG(path_stat.st_mode))
			return s;
		return (0);
	}
	//for (int i = 0; i < msh->vars_count; i++)
	for (int i = 0; msh->env[i]; i++)
	{
		if (!ft_strncmp("PATH=", msh->env[i], 5)) // check
		{
			char **path = split(msh->env[i], ":");
			for (int j = 0; path[j]; j++)
			{
				int l1 = ft_strlen(path[j]);
				int l2 = ft_strlen(s);
				char *full = malloc(l1 + l2 + 2);
				ft_memcpy(full, path[j], l1);
				full[l1] = '/';
				ft_memcpy(full + l1 + 1, s, l2);
				full[l1 + l2 + 1] = 0;

				//todo: lstat vs stat (check bash on symlinks)
				struct stat path_stat;
				stat(full, &path_stat); 

				// check if directory? what if s have / in the end?
				if (!access(full, F_OK) && !access(full, X_OK) && S_ISREG(path_stat.st_mode))
				{
					return full;
				}
			}
			break;
		}
	}
	return (0);
}

void push_char_to_str(char **str, int *size, char c)
{
	char *new = malloc(*size + 2);
	memcpy(new, *str, *size);
	new[*size] = c;
	new[*size + 1] = 0;
	free(*str);
	*str = new;
	*size = *size + 1;
}

char *get_var_value(t_msh *msh, char *name, int len)
{
	for (t_var *var = msh->first_var; var; var = var->next)
	{
		if (ft_strlen(var->name) == (size_t)len &&
			!ft_strncmp(var->name, name, len))
			return var->value;
	}
	return "";
}

void add_var(t_msh *msh, char *name, char *value)
{
	t_var *last = 0;
	for (t_var *var = msh->first_var; var; var = var->next)
	{
		if (!strcmp(var->name, name))
		{
			var->value = value;
			return ;
		}
		if (!var->next)
			last = var;
	}
	t_var *new = calloc(1, sizeof(*new));
	new->name = name;
	new->value = value;
	if (!msh->first_var)
		msh->first_var = new;
	else
		last->next = new;
}

char *expand(t_msh *msh, char *s)
{
	char *res = calloc(1 , 1);
	int size = 0;
	int inside_double_quotes = 0;
	int inside_single_quotes = 0;

	int i = 0;
	while (s[i])
	{
		if (s[i] == '"')
		{
			if (!inside_single_quotes)
				inside_double_quotes = !inside_double_quotes;
			else
				push_char_to_str(&res, &size, '"');
			i++;
		}
		else if (s[i] == '\'')
		{
			if (!inside_double_quotes)
				inside_single_quotes = !inside_single_quotes;
			else
				push_char_to_str(&res, &size, '\'');
			i++;
		}
		else if (s[i] == '$' && !inside_single_quotes)
		{
			i++;
			int j = i;
			while (s[i] && !isspace(s[i]) && get_token_type(s + i) == TOKEN_WORD
					&& s[i] != '"' && s[i] != '\'')
				i++;
			char *value = get_var_value(msh, s + j, i - j);
			for (int k = 0; value[k]; k++)
				push_char_to_str(&res, &size, value[k]);
		}
		else
		{
			if (!inside_double_quotes && !inside_single_quotes &&
					(isspace(s[i]) || get_token_type(s + i) != TOKEN_WORD))
				break ;
			push_char_to_str(&res, &size, s[i]);
			i++;
		}
	}
	if (strcmp(s, res))
		printf("expanded '%s' to '%s'\n", s, res);
	assert(!s[i]);
	return (res);
}

t_cmd get_cmd_from_tokens(t_msh *msh, t_token *token, int token_count)
{
	t_cmd cmd;

	cmd.command = 0;
	cmd.args = calloc(1, sizeof(char *));
	cmd.valid = 0;
	cmd.in = -1;
	cmd.out = -1;
	int arg_count = 0;
	int i = 0;
	char *in_file = 0, *out_file = 0;
	while (i < token_count)
	{
		if (token->type == TOKEN_WORD)
		{
			char *s = expand(msh, token->word);
			if (!cmd.command)
				cmd.command = s;
			char **new = calloc(arg_count + 2, sizeof(char *));
			memcpy(new, cmd.args, sizeof(char *) * arg_count);
			new[arg_count++] = s;
			cmd.args = new;
		}
		else if (token->type == '<')
		{
			token = token->next;
			char *filename = expand(msh, token->word);	
			i++;
			assert(i < token_count && token->type == TOKEN_WORD);
			int in = open(filename, O_RDONLY);
			if (in < 0)
			{
				perror(filename);
				return cmd;
			}
			in_file = filename;
			if (cmd.in != -1)
				close(cmd.in);
			cmd.in = in;
		}
		else if (token->type == '>')
		{
			token = token->next;
			char *filename = expand(msh, token->word);	
			i++;
			assert(i < token_count && token->type == TOKEN_WORD);
			int out = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (out < 0)
			{
				perror(filename);
				return cmd;
			}
			out_file = filename;
			if (cmd.out != -1)
				close(cmd.out);
			cmd.out = out;
		}
		else
			assert(0);
		i++;
		token = token->next;
	}
	char *name = cmd.command;
	cmd.command = get_fullpath(msh, cmd.command);
	if (cmd.command)
		cmd.valid = 1;
	printf("\033[0;32mparsed command: [name: \"%s\", fullpath: \"%s\", in: \"%s\", out: \"%s\", args: [",
			name, cmd.command, in_file, out_file);
	for (int i = 0; cmd.args[i]; i++)
	{
		printf("\"%s\"", cmd.args[i]);
		if (cmd.args[i + 1])
			printf(", ");
	}
	printf("]\033[0m\n");
	return cmd;	
}

t_exec_result	execute(t_msh *msh, t_node *node, int in, int out)
{
	t_exec_result res;

	res.pid = -1;
	res.status = 1;
	if (node->type == TOKEN_WORD)
	{
		t_cmd cmd = get_cmd_from_tokens(msh, node->args, node->arg_count);
		if (!cmd.valid)
			return res; // ????? fix status

		int pid = fork();
		if (pid < 0)
		{
			perror("fork");
			return res;
		}
		else if (!pid)
		{
			int in_fd = in;
			int out_fd = out;
			if (cmd.in != -1)
				in_fd = cmd.in;
			if (cmd.out != -1)
				out_fd = cmd.out;
			for (t_pipe *pipe = msh->first_pipe; pipe; pipe = pipe->next)
			{
				if (pipe->in != in_fd && pipe->in != out_fd)
					close(pipe->in);
				if (pipe->out != in_fd && pipe->out != out_fd)
					close(pipe->out);
			}
			// what if out_fd is stdin? maybe not possible
			if (in_fd)
				dup2(in_fd, 0);
			if (out_fd != 1)
				dup2(out_fd, 1);
			execve(cmd.command, cmd.args, msh->env);
			perror("execve");
			exit(1);
		}
		else
		{
			res.pid = pid;
			t_process *p = calloc(sizeof(*p), 1);
			p->id = pid;
			if (!msh->first_process)
				msh->first_process = msh->last_process = p;
			else
				msh->last_process = msh->last_process->next = p;
		}
		close(cmd.in);
		close(cmd.out);
	}
	else if (node->type == TOKEN_AND || node->type == TOKEN_OR)
	{
		t_exec_result r = execute(msh, node->left, in, out);
		int status = r.status;
		if (r.pid >= 0)
		{
			waitpid(r.pid, &status, 0);
			if (msh->first_process)
			{
				if (msh->first_process->id == r.pid)
					msh->first_process = msh->first_process->next;
				else
				{
					t_process *p = msh->first_process;
					while (p->next)
					{
						if (p->next->id == r.pid)
						{
							p->next = p->next->next;
							break ;
						}
						p = p->next;
					}
				}
			}
		}
		status = WEXITSTATUS(status); // ???
		if ((node->type == TOKEN_AND && !status) ||
			(node->type == TOKEN_OR && status))
			r = execute(msh, node->right, in, out);	
		return r;
	}
	else if (node->type == '|')
	{
		t_pipe *p = calloc(1, sizeof(*p));
		int fd[2];
		pipe(fd);
		p->in = fd[0];
		p->out = fd[1];
		t_pipe *prev_last = msh->last_pipe;
		if (!msh->first_pipe)
			msh->first_pipe = msh->last_pipe = p;
		else
			msh->last_pipe = msh->last_pipe->next = p;
		execute(msh, node->left, in, p->out);
		res = execute(msh, node->right, p->in, out);
		close(fd[0]);
		close(fd[1]);

			
		msh->last_pipe = prev_last;
		if (!prev_last)
			msh->first_pipe = 0;
	}
	else
		assert(0);
	return res;
}

int main(int argc, char **argv, char **envp)
{
	char	*line;
	t_msh msh;



	while (1)
	{
		line = readline("msh$ ");
		if (!line) // what?
			break ;
		if (*line)
		{
			ft_memset(&msh, 0, sizeof(msh));
#if 1
			for (int i = 0; envp[i] ; i++)
			{
				char *eq = ft_strchr(envp[i], '=');
				assert(eq); // ?????????????????
				int len = strlen(envp[i]);
				char *s1 = calloc(1, eq - envp[i] + 1);
				memcpy(s1, envp[i], eq - envp[i]);

				//a=b
				//^
				char *s2 = calloc(1, envp[i] + len - eq);
				memcpy(s2, eq + 1, envp[i] + len - eq - 1);

				add_var(&msh, s1, s2);
			}
#endif
			msh.env = envp;
			tokenize(&msh, line);

			for (t_token *token = msh.first_token; token->type; token = token->next)
			{
				printf("\"%s\" ", token->word);
			}
			printf("\n");
			if (msh.failed)
				continue ;
			parse(&msh);
			if (msh.failed)
				continue ;
			execute(&msh, msh.root, 0, 1);
			int status = 0;
			for (t_process *p = msh.first_process; p; p = p->next)
			{
				waitpid(p->id, &status, 0);
			}
			printf("exited with status %d\n", WEXITSTATUS(status));
		}
	}
}
