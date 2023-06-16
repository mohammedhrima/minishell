#ifndef REMOVE_H
# define REMOVE_H

# include "minishell.h"

char	*type_to_string(t_type type);
void	ft_putchar(int fd, int c);
void	ft_putstr(int fd, char *str);
void	ft_putnbr(int fd, long num);
void	print_space(int fd, int len);
int		ft_isnum(int c);
void	ft_printf(int file_descriptor, char *fmt, ...);

#endif