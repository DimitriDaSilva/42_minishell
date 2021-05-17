/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   write_term.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gleal <gleal@student.42lisboa.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/21 10:38:39 by dda-silv          #+#    #+#             */
/*   Updated: 2021/05/17 03:05:52 by gleal            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WRITE_TERM_H
# define WRITE_TERM_H

# include "main.h"

void	write_prompt(void);
void	write_msh_error(char *err_message);
void	write_exec_error(char *func_name, char *err_message);
void	write_msh_exec_error(char *func_name, char *err_message);
void	write_msh_exec_arg_error(char *exec_name, char *arg, char *err_message);
void	write_msh_exec_arg_error_nocolon(char *exec_name,
			char *arg, char *err_message);

#endif
