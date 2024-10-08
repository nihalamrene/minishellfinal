#include "../../includes/parse.h"

void	set_oper(t_token *token, t_redi **redir, int type)
{
	if (token)
		ft_lstadd_back_redi(redir,
			ft_lstnew_redi(ft_strdup(token->content), type));
	if (token && type == heredoc && token->type == SINGLE_EXP)
		ft_lstlast_redi(*redir)->must_exp = 1;
}

int	check_redir(t_token *token, t_envi **env)
{
	int	len;

	len = ft_strlen(token->content) - 1;
	if (token->expand)
	{
		if (ft_strchr_2(token->content, ' ') && token->content[len] != ' ')
		{
			ft_putendl_fd_2("ambigous redirection", NULL, 2);
			set_exit_status(1, env);
			return(1);
		}
		if (!*(token->content))
		{
			ft_putendl_fd_2("ambigous redirection", NULL, 2);
			set_exit_status(1, env);
			return(1);
		}
	}
	return (0);
}

void	is_operator(t_token *token, t_cmd *cmd, t_envi **env)
{
	if (token->next)
	{
		if (check_redir(token->next, env))
		{
			cmd->err = 1;
			return ;
		}
	}

	if (!ft_strcmp(token->content, "<<"))
		set_oper(token->next, &cmd->in, heredoc);
	else if (*(token->content) == '<')
		set_oper(token->next, &cmd->in, in);
	else if (!ft_strcmp(token->content, ">>"))
		set_oper(token->next, &cmd->out, append);
	else if (*(token->content) == '>')
		set_oper(token->next, &cmd->out, out);
}

void	rub_operator(t_cmd *cmd, t_token *token, t_token **tok, t_envi **env)
{
	t_token	*tmp;
	t_token	*tmp1;

	tmp = NULL;
	while (token && token->type != PIPE)
	{
		if (token->type == OPERATOR)
		{
			is_operator(token, ft_lstlast_cmd(cmd), env);
			if (token && token->next)
			{
				tmp1 = token->next->next;
				ft_remove(token, &token->next, tok);
				ft_remove(tmp, &token, tok);
				token = tmp1;
			}
		}
		else
		{
			tmp = token;
			token = token->next;
		}
	}
}

void	set_cmd(t_cmd *cmd)
{
	cmd->cmd = NULL;
	cmd->in = NULL;
	cmd->out = NULL;
	cmd->pipe = 0;
}