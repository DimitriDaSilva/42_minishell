/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gleal <gleal@student.42lisboa.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/21 14:42:15 by dda-silv          #+#    #+#             */
/*   Updated: 2021/05/03 15:49:05 by gleal            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "execute.h"

/*
** This function executes all the command tables entered by the user.
** At the moment it simply executes each command independently of the
** exit status of the other command tables.
** @param:	- [t_ast *] struct that contains all the command and typed
**						information;
**			- [t_list **] Pointer to linked list with all the
**							environment variables;
** @return:	[int] exit status of the function;
** Line-by-line comments:
** @1-3		Inside the ast struct there are linked lists in which
**			t_list->data corresponds to a t_cmd_table struct, which 
**			contains all the command table information (commands, delimiters
**			and return value).
*/

int	execute_ast(t_ast **ast, t_list **env)
{
	t_list		*cmd_table;
	static int	status;

	cmd_table = (*ast)->cmd_tables;
	while (cmd_table)
	{
		status = execute_cmd_table((t_cmd_table *)cmd_table->data, env, status);
		cmd_table = cmd_table->next;
	}
	return (status);
}

/*
** Redirections still need to be implemented. For each command table
** this function will execute each command consecutively and redirect
** the input and output to the following commands if the redirection
** signs are present. (ask why pipes are not redirections).
** @param:	- [t_cmd_table] Current command table being executed(series
** of interconnected commands)
**			- [t_list **] Pointer to linked list with all the
**							environment variables;
** @return:	[int] exit status of the command table;
** Line-by-line comments:
** @7-10	if there are sevaral commands in a command table
**			then they need to be connected somehow (otherwise they
**			would be categorized as a single command with 
**			multiple arguments)
*/

int	execute_cmd_table(t_cmd_table *cmd_table, t_list **env, int	last_status)
{
	t_list	*cmds;
	int		pipe;
	int		status;

	status = last_status;
	cmds = cmd_table->cmds;
	while (cmds)
	{
		if (cmds->next == NULL)
			pipe = 0;
		else
			pipe = 1;
		status = execute_cmd((t_cmd *)cmds->data, env, pipe, status);
		cmds = cmds->next;
	}
	return (status);
}

/*
** Executes the command based on the first token/word;
** @param:	- [t_cmd *] current command
**			- [t_list **] Pointer to linked list with all the
**							environment variables;
**			- [int] Idenfifies if the output will be redirected or if
**			we should execute the command.
** @return:	[int] exit status of command;
** Line-by-line comments:
** @7		replaces environment variables by their values;
** @12		executes recreated functions (echo, cd, pwd, export, unset, env, exit);
*/

int	execute_cmd(t_cmd *cmd, t_list **env, int pipe, int last_status)
{
	t_list	*tokens;
	char	*first;

	tokens = cmd->tokens;
	if (tokens == 0)
		return (0);
	env_vars(tokens, last_status);
	first = ((t_token *)tokens->data)->str;
	if (ft_strcmp(first, "exit") == 0 && pipe == 0)
		ft_exit(0);
	if (is_builtin(first))
		return (execute_builtin(tokens, env));
	else
		return (1);
}
