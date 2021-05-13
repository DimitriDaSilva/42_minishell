/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_absolute_path.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gleal <gleal@student.42lisboa.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/05 16:47:12 by dda-silv          #+#    #+#             */
/*   Updated: 2021/05/13 18:33:28 by gleal            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_ABSOLUTE_PATH_H
# define GET_ABSOLUTE_PATH_H

# include "main.h"

int		has_relative_path(char *first_token);
int		has_absolute_path(char *first_token);
char	*get_absolute_path(char *program_name);
char	**get_path_env_split(void);
char	*get_correct_path(char *program_name, char **path_env_split);
void	add_slash(char ***path_env_split);

#endif
