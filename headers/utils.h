/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 02:09:40 by mhrima            #+#    #+#             */
/*   Updated: 2023/06/16 22:30:04 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

# include "minishell.h"

int		ft_exit(int code);
int		get_last_exit_code(void);
int		ft_isspace(int c);
void	ft_memset(void *pointer, int c, size_t len);
void	*ft_memcpy(void *destination, void *source, size_t len);
void	add_to_addresses(void *pointer);
void	*ft_calloc(size_t count, size_t size);
void	*ft_realloc(void *pointer, size_t oldsize, size_t newsize);
int		ft_strlen(char *string);
void	ft_strncpy(char *destination, char *source, int len);
char	*ft_strchr(char *string, int c);
int		ft_strncmp(char *left, char *right, size_t len);
int		ft_strcmp(char *left, char *right);
char	**split(char *string, char *spliter);
char	**split_by_two(char *string, char c);
char	*strjoin(char *string1, char *string2, char *string3);
char	*ft_itoa(int num);
int		ft_atoi(char *str);
char	*ft_strdup(char *string);
int     create_child(void);
int     *open_pipe(void);
void close_fds(void);
#endif