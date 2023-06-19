#include "../_headers/utils.h"

bool	is_redirection(t_type type)
{
	return (type == redir_input || type == redir_output || type == append_);
}

