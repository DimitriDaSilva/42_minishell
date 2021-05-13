/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gleal <gleal@student.42lisboa.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/05 19:10:23 by dda-silv          #+#    #+#             */
/*   Updated: 2021/05/13 02:33:59 by gleal            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "signals.h"

/*
** When user presses ctrl-c and ctrl-d a new line is added
** as opposed to the default signal interrupt behaviour
** @param:	- [int] signal identifier
*/

void	catch_sigint(int signum)
{
	(void)signum;
	ft_putstr("\n");
}

/*
** prints out quit message and kills children processes
(if we has default_signal sleep 5 | sleep 5; sleep 5
ctrl-\ would kill minishell)
** @param:	- [int] signal identifier
** Line-by-line comments:
** @3	sends
*/

void	catch_sigquit(int signum)
{
	(void)signum;
	printf("Quit: 3\n");
	kill(0, SIGCHLD);
}
