/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dda-silv <dda-silv@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/21 10:37:25 by dda-silv          #+#    #+#             */
/*   Updated: 2021/05/16 18:33:35 by dda-silv         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

/*
** Extracts the Abstract Syntax Tree (AST) out of the input entered in cmd line.
** In this case, the AST is a list that has a cmd_table per node and each
** cmd_table has a list a simple command per node. Each simple command has a 
** list of tokens and list of redirections
** @param:	- [const char *] the unchanged line entered in stdin
** @return:	[t_ast *] struct with a list of cmd_tables
** Line-by-line comments:
** @10		We pass down the reference of the curr_pos to keep track of the
** 			parsing executed by every subsequent function
** @14		In exec_ast(), we'll want to execute the cmd_table in entering order
**			so we need to add to the back each new cmd_table
*/

t_ast	*get_ast(const char *input)
{
	t_ast	*ast;
	t_list	*cmd_table;
	int		curr_pos;

	ast = ft_calloc(1, sizeof(t_ast));
	if (!ast)
		quit_program(EXIT_FAILURE);
	curr_pos = 0;
	while (input[curr_pos])
	{
		skip_spaces(input, &curr_pos);
		cmd_table = ft_lstnew((void *)get_cmd_table(input, &curr_pos));
		if (!cmd_table)
			quit_program(EXIT_FAILURE);
		ft_lstadd_back(&ast->cmd_tables, cmd_table);
	}
	return (ast);
}

/*
** Gets a command table, which is a series of simple commands to execute
** @param:	- [const char *] the unchanged line entered in stdin
**			- [int *] the current parsing position within the input  
** @return:	[t_cmd_table *] struct with list of simple cmds and the delimiter
**							that seperates this cmd_table from the next
** Line-by-line comments:
** @14-15	If the delimiter is '|', it means we just got a simple command
**			and we are about to get a new one. So we increment curr_pos and we
**			continue parsing
**			All error handling is done before by is_input_valid() so we don't
**			need to be on the lookout for bad syntax
** @16-21	If the delimiter is ';', the current cmd_table is finished
*/

t_cmd_table	*get_cmd_table(const char *input, int *curr_pos)
{
	t_cmd_table	*cmd_table;
	t_list		*cmd;

	cmd_table = ft_calloc(1, sizeof(t_cmd_table));
	if (!cmd_table)
		quit_program(EXIT_FAILURE);
	while (input[*curr_pos] && !is_cmd_table_delimiter(&input[*curr_pos]))
	{
		skip_spaces(input, curr_pos);
		cmd = ft_lstnew((void *)get_cmd(input, curr_pos));
		if (!cmd)
			quit_program(EXIT_FAILURE);
		ft_lstadd_back(&cmd_table->cmds, cmd);
		if (input[*curr_pos] == '|' && input[*curr_pos + 1] != '|')
			(*curr_pos)++;
	}
	cmd_table->delimiter = get_cmd_table_delimiter(input, curr_pos);
	return (cmd_table);
}

/*
** Gets a simple command, which is a series of tokens or redirections. Tokens
** and redirection aren't necessarily sorted. This is a valid simple command:
** > redir1 program_name token1 >> redir2 token2 token3 < redir3
** @param:	- [const char *] the unchanged line entered in stdin
**			- [int *] the current parsing position within the input  
** @return:	[t_cmd *] struct with a list of tokens and a list of redirection
** 					  in order of entering
** Line-by-line comments:
** @7-8		Parse until simple command is finished (i.e. '\0', ';' or '|')
** @10		If not '>' or '<', then it's a token
** @17		If '>' or '<', then it's a redirection
*/

t_cmd	*get_cmd(const char *input, int *curr_pos)
{
	t_cmd	*cmd;
	t_list	*new_node;

	cmd = ft_calloc(1, sizeof(t_cmd));
	if (!cmd)
		quit_program(EXIT_FAILURE);
	while (input[*curr_pos] && !is_cmd_delimiter(input[*curr_pos]))
	{
		if (input[*curr_pos] != '>' && input[*curr_pos] != '<')
		{
			new_node = ft_lstnew((void *)get_token(input, curr_pos));
			if (!new_node)
				quit_program(EXIT_FAILURE);
			ft_lstadd_back(&cmd->tokens, new_node);
		}
		else if (input[*curr_pos] == '>' || input[*curr_pos] == '<')
		{
			new_node = ft_lstnew((void *)get_redir(input, curr_pos));
			if (!new_node)
				quit_program(EXIT_FAILURE);
			ft_lstadd_back(&cmd->redirs, new_node);
		}
		skip_spaces(input, curr_pos);
	}
	return (cmd);
}

/*
** Gets a single redirection
** @param:	- [const char *] the unchanged line entered in stdin
**			- [int *] the current parsing position within the input  
** @return:	[t_redir *] struct with a string with the direction and the type
** Line-by-line comments:
** @6-14	Gets the type
** @16		Gets the file. The file_name is stored inside a t_token because
** 			we need to know the delimiter around the redirection
*/

t_redir	*get_redir(const char *input, int *curr_pos)
{
	t_redir	*redir;

	redir = ft_calloc(1, sizeof(t_redir));
	if (!redir)
		quit_program(EXIT_FAILURE);
	if (input[*curr_pos] == '<')
		*redir->type = input[(*curr_pos)++];
	else if (!ft_strncmp(&input[*curr_pos], ">>", 2))
	{
		ft_strncpy(redir->type, (char *)&input[*curr_pos], 2);
		*curr_pos += 2;
	}
	else if (input[*curr_pos] == '>')
		*redir->type = input[(*curr_pos)++];
	skip_spaces(input, curr_pos);
	redir->direction = get_token(input, curr_pos);
	return (redir);
}

/*
** Gets a token, which represents one argument in the cmd_line. It can
** either be a word or a string of words if quotes are used.
** @param:	- [const char *] the unchanged line entered in stdin
**			- [int *] the current parsing position within the input  
** @return:	[t_token *] a struct with a str and a char delimiter
** Line-by-line comments:
** @7		The delimiter can be a space, single or double quotes.
**			get_token_delimiter() increments curr_pos if a quotes is found so that
**			the token starts after the quotes
** @8		We need to save the beginning of the token so that we can extract
**			the string once we have found the end of the token
** @11-12	The token is a string of words because a quote has been found
**			by get_token_delimiter(). So we parse while we haven't found it's matching
**			quote (either single or double)
** @13-14	The token is a word and will be finished when we find a delimiter
**			(' ', ';', '|', '<' or '>')
** @20-21	We need to do one last increment of curr_pos if we were dealing with
**			quotes, otherwise next time get_token() will be called it will
**			process the closing quotes of this token as the opening ones of the
**			next token
** 22-23	If the delimiter is a space, a valid input would be:
**			hello"wor'ld"'te"st'
**			This input needs to represent 1 single token and be interpreted as:
**			hellowor'ldte"st
*/

char	*get_token(const char *input, int *curr_pos)
{
	char	*token;
	int		saved_pos;

	saved_pos = *curr_pos;
	while (input[*curr_pos] && !is_token_delimiter(input[*curr_pos]))
	{
		if (input[*curr_pos] == '"' || input[*curr_pos] == '\'')
			skip_quotes(input, curr_pos);
		if (input[*curr_pos])
			(*curr_pos)++;
	}
	token = ft_substr(input, saved_pos, *curr_pos - saved_pos);
	if (!token)
		quit_program(EXIT_FAILURE);
	return (token);
}
