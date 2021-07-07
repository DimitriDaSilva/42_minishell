/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dda-silv <dda-silv@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/07 21:35:14 by dda-silv          #+#    #+#             */
/*   Updated: 2021/07/08 00:56:29 by dda-silv         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

/*
** A custom shell that takes some basic functionality of bash and history
** management (pressing up / down will parse through commands previously
** entered).
** @param:	- [int] argument count
**			- [char **] argument values
**			- [char **] environment pointers of the shell where the program is
**						running
** Line-by-line comments:
** @1		The command history is a double linked list with a string as data
**			so here input represents a new node of that command history
** @6		The main has an infinite loop so that the program ends only with
**			signals or exit.
** @8		To implement history management we need to have more control on the
**			input of the user. The canonical mode (mode set by default) means
**			that the characters will be read only when the user presses enter.
**			We need to read one input at a time to be able to catch the arrows
**			and other input like the Home key or Ctrl + X.
**			It also means that the keys are "echoed" to the terminal and we
**			don't want to see for instance the ANSI value of the up arrow
** @10		get_input returns a string with the input of the user and takes
**			care of parsing through the history of previous command if the
**			user presses an up or down arrow
** @17		We have to set canonical back on in case the program executed
**			by the user require an input
*/

t_msh	g_msh;

int	main(int argc, char **argv, char **envp)
{
	t_dlist	*input;

	(void)argc;
	(void)argv;
	init_minishell(&g_msh, envp);
	while (1)
	{
		turn_off_canonical_mode(&g_msh.termcaps);
		write_prompt();
		input = ft_dlstnew((void *)get_input(g_msh.input_history,
					&g_msh.termcaps));
		if (!input)
			quit_program(EXIT_FAILURE);
		ft_dlstadd_front(&g_msh.input_history, input);
		if (!is_input_valid((const char *)input->data))
			continue ;
		turn_on_canonical_mode(&g_msh.termcaps);
		g_msh.ast = get_ast((const char *)input->data);
		exec_ast(g_msh.ast);
		free_ast(g_msh.ast);
		g_msh.ast = 0;
	}
	return (0);
}
