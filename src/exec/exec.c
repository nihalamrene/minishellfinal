#include "../../includes/parse.h"

void	_process(char **cmd, int fd_in, int fd_out, t_envi *envi)
{
	if (dup2(fd_in, STDIN_FILENO) == -1)
	{
		ft_putnbr_fd(fd_in, 2);
		perror("\nerror dup");
		return ;
	}
	if (dup2(fd_out, STDOUT_FILENO) == -1)
	{
		perror("error dup2\n");
		return ;
	}
	exe_cmd(cmd, envi);
}

int	exe_first_cmd(t_cmd *cmd, t_envi *envi, char **files)
{
	pid_t	pid;
	int		fd_in;
	int		fd_out;
	int		fd_pipe[2];

	if (pipe(fd_pipe))
	{
		perror("minishell: pipe");
		return (-1);
	}
	pid = fork();
	if (pid == -1)
	{
		perror("minishell: fork");
		close(fd_pipe[0]);
		close(fd_pipe[1]);
		return (-1);
	}
	if (pid == 0)
	{
		close(fd_pipe[0]);
		fd_in = STDIN_FILENO;
		fd_out = fd_pipe[1];
		if (!cmd->next)
			fd_out = STDOUT_FILENO;
		handel_red(cmd, &fd_in, &fd_out, files);
		if ((cmd->cmd))
			_process(cmd->cmd, fd_in, fd_out, envi);
	}
		if (cmd && !cmd->next)
			return (close(fd_pipe[1]), close(fd_pipe[0]), -1);
		return (close(fd_pipe[1]), fd_pipe[0]);
}

int	cmd_med(t_cmd *cmd, t_envi *envi, int fd_in, char **files)
{
    int     fd_out;
    int     fd_pipe[2];
    pid_t   pid;

	if (pipe(fd_pipe))
	{
		perror("pipe err\n");
		return (-1);
	}
	pid = fork();
	if (pid == -1)
	{
		perror("minishell: fork");
		close(fd_in);
		close(fd_pipe[0]);
		close(fd_pipe[1]);
		return (-1);
	}
	if (pid == 0)
	{
		close(fd_pipe[0]);
		fd_out = fd_pipe[1];
		handel_red(cmd, &fd_in, &fd_out, files);
		_process(cmd->cmd, fd_in, fd_out, envi);
	}
	return (close(fd_pipe[1]), close(fd_in), fd_pipe[0]);
}

void	last_cmd(t_cmd *cmd, t_envi *envi, int fd_in, char **files)
{
	pid_t	pid;
	int		fd_out;

	pid = fork();
	if (pid == -1)
	{
		perror("minishell: fork");
		close(fd_in);
		return ;
	}
	if (pid == 0)
	{
		fd_out = STDOUT_FILENO;
		handel_red(cmd, &fd_in, &fd_out, files);
		_process(cmd->cmd, fd_in, fd_out, envi);
	}
	close(fd_in);
}

int	count_cmd(t_cmd *cmd)
{
	int	i;

	i = 0;
	while (cmd)
	{
		i++;
		cmd = cmd->next;
	}
	return (i);
}

void	_exec(t_cmd *cmd, t_envi **envi)
{
	int		fd;
	int		nbr_cmd;
	int		fd_out;
	char	**heredoc_files;

	nbr_cmd = count_cmd(cmd);
	heredoc_files = creat_heredoc(cmd, *envi);
	if (cmd && !(cmd->err))
	{
		fd = STDIN_FILENO;
		fd_out = STDOUT_FILENO;
		if (cmd->cmd && nbr_cmd == 1 && is_build(*(cmd->cmd)) )
		{
			build_in_exe(cmd, envi, heredoc_files);
			cmd = cmd->next;
			return ;
		}
		fd = exe_first_cmd(cmd, *envi, heredoc_files);
		if (cmd)
			cmd = cmd->next;
		while (cmd && cmd->next && fd != -1)
		{
			fd = cmd_med(cmd, *envi, fd, heredoc_files);
			cmd = cmd->next;
		}
		if (cmd && fd != -1)
			last_cmd(cmd, *envi, fd, heredoc_files);
		while (nbr_cmd--)
		{
			int	status;

			wait(&status);
			if (WIFEXITED(status))
				set_exit_status(WEXITSTATUS(status), envi);
			else if (WEXITSTATUS(status))
				set_exit_status(WEXITSTATUS(status), envi);
		}
	}
	nbr_cmd = -1;
	while (heredoc_files && heredoc_files[++nbr_cmd])
	{
		unlink(heredoc_files[nbr_cmd]);
		free(heredoc_files[nbr_cmd]);
	}
	free(heredoc_files);
}
