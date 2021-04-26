/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dds <dda-silv@student.42lisboa.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/21 17:33:17 by dda-silv          #+#    #+#             */
/*   Updated: 2021/04/26 14:28:49 by gleal            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	main(int argc, char **argv, char **envp)
{
	t_dlist	*cmd_history;
	t_dlist	*ast;
	t_list	*env;

	(void)argc;
	(void)argv;
	env = dup_env(envp);
	while (1)
	{
		write_prompt();
		ast = ft_dlstnew((void *)get_ast());
		if (!ast)
			exit(EXIT_FAILURE);
		ft_dlstadd_front(&cmd_history, ast);
		// print_ast((t_ast *)cmd_history->data);
		if (((t_ast *)cmd_history->data)->cmd_tables)
			execute_ast((t_ast *)cmd_history->data, &env);
	}
	return (0);
}
