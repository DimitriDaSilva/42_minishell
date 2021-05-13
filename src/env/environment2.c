/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environment2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gleal <gleal@student.42lisboa.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/03 17:21:45 by dda-silv          #+#    #+#             */
/*   Updated: 2021/05/13 19:43:41 by gleal            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "environment.h"

/*
** Gets the value stored in the environment variable
linked list given a variable name
** @param:	- [char *] name of environment variable to check
**			- [d] param_value
** @return:	[char *] string duplicate of environment variable value
** Line-by-line comments:
** @14	comment
*/

char	*ft_getenv(char *key)
{
	int		i;
	t_list	*envp;
	char	*curr_envp;
	char	*value;

	envp = g_msh.dup_envp;
	while (envp)
	{
		i = 0;
		curr_envp = (char *)envp->data;
		while (key[i] && curr_envp[i] && (key[i] == curr_envp[i]))
			i++;
		if (!key[i] && (curr_envp[i] == '='
				|| curr_envp[i] == '\0'))
		{
			value = ft_strdup(&(curr_envp[i + 1]));
			if (!value)
				quit_program(EXIT_FAILURE);
			return (value);
		}
		envp = envp->next;
	}
	return (0);
}

/*
** Creates a copy of the envp string array so that it can be manipulated
** throughout our program
** @param:	- [t_list **] stored in linked list for easier exporting and
**			unsetting (more modular memory allocation)
**			- [char *] environment vars array of strings 
**			(third argument in main function)
** Line-by-line comments:
** @5	some operating systems don't have the third main argument (array of
** 		environment variables). In this case the exercise would have needed
** 		to be solved using the environ extern global variable
**		(see man environ(7))
*/

void	duplicate_env(t_list **dup_envp, char **envp)
{
	t_list	*next_env;
	int		i;
	char	*tmp;

	if (!envp || !envp[0])
		quit_program(EXIT_FAILURE);
	i = 0;
	while (envp[i] != NULL)
	{
		tmp = ft_strdup(envp[i]);
		if (!tmp)
			quit_program(EXIT_FAILURE);
		next_env = ft_lstnew(tmp);
		if (!next_env)
			quit_program(EXIT_FAILURE);
		ft_lstadd_back(dup_envp, next_env);
		i++;
	}
}

/*
** In case we want to unnassign an environment variable that
has been exported (key must be the unnassigned environment variable)
e.g. USER=gleal receives USER to unnassign
** @param:	- [char *] environment variable name (unnassigned)
** Line-by-line comments:
** @12-16	skip if env var is already unnassigned
*/

void	remove_env_value(char	*key)
{
	int		i;
	t_list	*envp;
	char	*curr_envp;

	envp = g_msh.dup_envp;
	while (envp)
	{
		i = 0;
		curr_envp = (char *)envp->data;
		while (key[i] && curr_envp[i] && (key[i] == curr_envp[i]))
			i++;
		if (!key[i] && curr_envp[i] == '=')
		{
			replace_string(key, &curr_envp);
			envp->data = curr_envp;
		}
		envp = envp->next;
	}
	return ;
}

void	replace_status_env(char **str, int	last_status)
{
	int		replace_spot;
	char	*status_string;
	char	*final;

	replace_spot = ft_strnstr_iterator(*str, "$?", ft_strlen(*str));
	while (replace_spot != -1)
	{
		status_string = ft_itoa(last_status);
		if (status_string == 0)
			return (quit_program(EXIT_FAILURE));
		final = replace_midstring(*str, "$?", status_string, replace_spot);
		free(status_string);
		status_string = 0;
		free(*str);
		*str = final;
		replace_spot = ft_strnstr_iterator(*str, "$?", ft_strlen(*str));
	}
}

void	replace_underscore_env(char **str)
{
	int		replace_spot;
	char	*final;
	char	*last_cmd_str;
	char	*to_print;

	last_cmd_str = ft_getenv("_");
	if (has_absolute_path(last_cmd_str) && is_in_path_env(last_cmd_str))
		to_print = ft_strrchr(last_cmd_str, '/');
	replace_spot = ft_strnstr_iterator(*str, "$_", ft_strlen(*str));
	while (replace_spot != -1)
	{
		final = replace_midstring(*str, "$_", to_print, replace_spot);
		free(*str);
		*str = final;
		replace_spot = ft_strnstr_iterator(*str, "$_", ft_strlen(*str));
	}
	free (last_cmd_str);
}
