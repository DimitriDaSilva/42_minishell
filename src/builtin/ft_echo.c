/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gleal <gleal@student.42lisboa.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/25 20:16:20 by gleal             #+#    #+#             */
/*   Updated: 2021/05/01 22:53:56 by gleal            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

/*
** ft_echo (prints args to standard output separated by spaces)
** @param:	- [t_list *] arguments/tokens
** @return:	[int] exit_status (still needs to be implemented)
** Line-by-line comments:
** @12	there can be multiple flag arguments, but as soon as we get a non
flag argument then flag time is over.
*/

int	ft_echo(t_list *tokens)
{
	int		lb;
	int		flag_time;
	char	*arg;
	char	delim;

	flag_time = 1;
	lb = 1;
	while (tokens)
	{
		arg = ((t_token *)tokens->data)->str;
		delim = ((t_token *)tokens->data)->delimiter;
		if (flag_time && arg[0] == '-' && has_repeated_char_in_str('n', arg, 1))
			lb = 0;
		else
		{
			if (!flag_time)
				ft_putchar_fd(' ', 1);
			flag_time = 0;
			ft_putstr_fd(arg, 1);
		}
		tokens = tokens->next;
	}
	if (lb)
		ft_putchar_fd('\n', 1);
	return (0);
}

/*
** Function to look for repeated characters
** @param:	- [char] char to be checked
**			- [char *] string to be parsed
**			- [unsigned int] iterator start
** @return:	[int] true or false
*/

int	has_repeated_char_in_str(char repeat_chr, char *str, unsigned int start)
{
	unsigned int	i;

	i = start;
	while (str[i])
	{
		if (str[i] != repeat_chr)
			return (0);
		i++;
	}
	return (1);
}
