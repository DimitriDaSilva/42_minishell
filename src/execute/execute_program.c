/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_program.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dda-silv <dda-silv@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/03 22:10:06 by dda-silv          #+#    #+#             */
/*   Updated: 2021/05/03 23:41:39 by dda-silv         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "execute.h"

int	execute_program(char **tokens, t_list *redirs, char **envp)
{
	pid_t	pid;
	int		status;
	char	*abs_path;

	if (has_path(tokens[0]))
		abs_path = tokens[0];
	else
		abs_path = get_abs_path(tokens[0]);
	pid = fork();	
	if (pid == 0)
		execve(abs_path, tokens, envp);
	if (pid > 0)
	{
		pid = wait(&status);
		// if (WIFEXITED(status))
		// 	printf("The process ended with exit(%d).\n", WEXITSTATUS(status));
		// if (WIFSIGNALED(status))
		// 	printf("The process ended with kill -%d.\n", WTERMSIG(status));
	}
	if (pid < 0)
		perror("In fork():");
	free(tokens);
	free(envp);
	// free(abs_path);
	return (1);
	(void)redirs;
}

int	has_path(char *first_token)
{
	int	check;

	if (*first_token == '.' || *first_token == '/')
		check = 1;
	else
		check = 0;
	return (check);
}

char *get_abs_path(char *program_name)
{
	char		**path_envs;
	int			i;
	char		*path_name;
	struct stat	statbuf;

	path_envs = ft_split(ft_getenv("PATH"), ":");
	if (!path_envs)
		ft_exit(EXIT_FAILURE);
	add_slash(&path_envs);
	i = 0;
	while (path_envs[i])
	{
		path_name = ft_strjoin(path_envs[i], program_name);
		if (!path_name)
			ft_exit(EXIT_FAILURE);
		if (stat(path_name, &statbuf) == EXIT_SUCCESS)
			break ;
		free(path_name);
		i++;
	}
	if (!path_envs[i])
		path_name = program_name;
	free_arr((void **)path_envs);
	return (path_name);
}

void	add_slash(char ***path_envs)
{
	int		i;
	char	*tmp;
	int		len;
	char	*path;

	i = 0;
	while ((*path_envs)[i])
	{
		path = (*path_envs)[i];
		len = ft_strlen(path);
		if (path[len - 1] != '/')
		{
			tmp = ft_strjoin(path, "/");
			free(path);
			(*path_envs)[i] = tmp;
		}
		i++;
	}
}

// int	execute_program(char **tokens, t_list *redirs, char **envp)
// {
// 	pid_t	pid;
// 	int		status;

// 	pid = fork();	
// 	if (pid == 0)
// 	{
// 		printf("\033[0;34m📌 Here in %s line %d\n\033[0m", __FILE__, __LINE__);
// 		execve(tokens[0], tokens, envp);
// 	}
// 	if (pid > 0)
// 	{
// 		printf("\033[0;34m📌 Here in %s line %d\n\033[0m", __FILE__, __LINE__);
// 		pid = wait(&status);
// 		if (WIFEXITED(status))
// 			printf("The process ended with exit(%d).\n", WEXITSTATUS(status));
// 		 if (WIFSIGNALED(status))
// 			printf("The process ended with kill -%d.\n", WTERMSIG(status));
// 	}
// 	if (pid < 0)
// 		perror("In fork():");
// 	// free_arr((void **)tokens);
// 	// free_arr((void **)envp);
// 	return (0);
// 	(void)redirs;
// }
