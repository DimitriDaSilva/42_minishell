/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dda-silv <dda-silv@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/13 16:48:16 by dda-silv          #+#    #+#             */
/*   Updated: 2021/07/08 00:56:53 by dda-silv         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_H
# define MAIN_H

# include <string.h>
# include <fcntl.h>
# include <curses.h>
# include <termcap.h>
# include <errno.h>
# include <signal.h>
# include <dirent.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/ioctl.h>
# include <termios.h>
# include "../libft/src/libft.h"

# include "structs.h"
# include "constants.h"

# include "builtin_utils.h"
# include "ft_cd.h"
# include "ft_exit.h"
# include "ft_echo.h"
# include "ft_env.h"
# include "ft_pwd.h"
# include "ft_export.h"
# include "ft_unset.h"

# include "environment.h"
# include "environment_utils.h"

# include "execute.h"
# include "execute_utils.h"
# include "fork_exec_wait.h"
# include "redirections_pipes.h"
# include "get_absolute_path.h"

# include "parse.h"
# include "parse_utils.h"
# include "get_input.h"
# include "input_validation.h"

# include "quit_program.h"
# include "free_memory.h"

# include "termcaps.h"

# include "init_minishell.h"
# include "signals.h"
# include "write_err_message.h"
# include "write_prompt.h"
# include "utils.h"

extern t_msh	g_msh;

#endif
