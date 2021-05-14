/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dda-silv <dda-silv@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 09:01:52 by dda-silv          #+#    #+#             */
/*   Updated: 2021/05/14 10:31:47 by dda-silv         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_ECHO_H
# define FT_ECHO_H

# include "main.h"

int	ft_echo(t_list *tokens);
int	has_repeated_char_in_str(char repeat_chr,
		char *str,
		unsigned int start);

#endif
