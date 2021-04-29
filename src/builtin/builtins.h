/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gleal <gleal@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/25 20:17:08 by gleal             #+#    #+#             */
/*   Updated: 2021/04/27 19:34:28 by gleal            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTINS_H
# define BUILTINS_H

# include "main.h"

int		has_repeated_char_in_str(char repeat_chr, char *str, unsigned int start);
int		is_env_var(char *potential_var, t_list *env);
int		change_dir_home(char	*pwd, t_list *env);
int		update_directories(char *new_dir, char *old_dir, t_list *env);
int		update_environment_var(char *var, char *new_value, t_list *env);
int		replace_env_value(char **env_ptr, char	*var_name, char *new_value);
int		change_to_old_dir(char	*pwd, t_list *env);

#endif
