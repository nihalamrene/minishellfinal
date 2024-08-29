#include "../../includes/parse.h"

 void	error_cmd(char *cmd, char *err_msg, int n)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd, 2);
	ft_putstr_fd(err_msg, 2);
	exit(n);
}

static void	sp_free(char **env)
{
	int	i;

	i = -1;
	while (env && env[++i])
		free(env[i]);
}

char    **get_env(t_envi *envi)
{
    char    **env;

    while (envi)
    {
        if (ft_strcmp(envi->key, "PATH") == 0)
            break ;
        envi = envi->next;
    }
    env = NULL;
    if (envi)
        env = ft_split(envi->value, ':');
    return (env);
}
char    *get_path_cmd(char *cmd, char **env)
{
    char	*path;
    int		i;

	if (!cmd)
		return (NULL);
	if (!access(cmd, F_OK))
        return (sp_free(env), free(env), ft_strdup(cmd));
	if (!env || ft_strchr(cmd, '/'))
	{
		error_cmd(cmd, ": No such file or directory\n", 127);
	}
	i = -1;
    while (env && env[++i])
	{
		path = ft_strjoin(ft_strjoin(env[i], "/"), cmd);
		if (!access(path, F_OK))
		{
			sp_free(env + i + 1);
			free(env);
			return (path);
		}
		free(path);
	}
	free(env);
	return (NULL);
}

int	count_env(t_envi *envi)
{
	int	i;

	i = 0;
	while (envi)
	{
		if (envi->value)
			i++;
		envi = envi->next;
	}
	return (i);
}

char	**execve_env(t_envi *envi)
{
	char	**env;
	int		i;
	int		pl_holder;

	i = count_env(envi);
	env = malloc((i + 1) * sizeof (char *));
	if (!env)
		return (NULL);
	i = 0;
	while (envi)
	{
		if (envi->key && !ft_strcmp(envi->key, "SHLVL") && envi->value)
		{
			pl_holder = ft_atoi(envi->value);
			free(envi->value);
			if (pl_holder < 0)
				envi->value = ft_strdup("0");
			else if (pl_holder >= 999)
				envi->value = ft_strdup("");
			else
				envi->value = ft_itoa(pl_holder + 1);
		}
		if (envi->value)
			env[i++] = ft_strjoin(ft_strjoin(ft_strdup(envi->key), "="), envi->value);
		envi = envi->next;
	}
	env[i] = NULL;
	return (env);
}

void	fct1(char *path, char **cmd)
{
    struct stat statbuf;

	if (stat(path, &statbuf) == 0)
	{
	    if (S_ISDIR(statbuf.st_mode)) 
	        error_cmd(path, ": is a directory\n", 126);
	    else 
		{
	        if (access(path, X_OK) == -1)
			{
	            if (errno == EACCES)
	                error_cmd(path, ": Permission denied\n", 126);
			}
	        else
			{
	            int fd = open(path, O_RDONLY);
	            if (fd == -1)
	                error_cmd(path, ": No such file or directory\n", 1);
	            else
				{
	                close(fd);
	                error_cmd(*cmd, ": command not found\n", 127);
	            }
	        }
	    }
	}
	else
	    error_cmd(path, ": No such file or directory\n", 127);
}

int	size_ptr(char **ptr)
{
	int	i;

	i = 0;
	while (ptr[i])
		i++;
	return (i);
}

void	exe_cmd(char **cmd, t_envi *envi)
{
    char    **env;
	int		exit_code;
    char    *path;

	if (!cmd || !*cmd)
		return ;
	if (is_build(*cmd))
	{
        exit_code =	build_in(cmd, &envi);
        exit(exit_code);
    }
	env = get_env(envi);
    path = get_path_cmd(*cmd, env);
    if (!path)
        error_cmd(*cmd, ": command not found\n", 127);
    env = execve_env(envi);
    execve(path, cmd, env);
	// if (errno == ENOENT)  // Command not found
    // {
    //     fprintf(stderr, "%s: command not found\n", *cmd);
    //     exit(127);
    // }
    // else if (errno == EACCES)  // Permission denied
    // {
    //     fprintf(stderr, "%s: permission denied\n", *cmd);
    //     exit(126);
    // }
    // else  // Other execve errors
    // {
    //     perror("execve");
    //     exit(1);
    // }
	fct1(path, cmd);
    free(path);
}
