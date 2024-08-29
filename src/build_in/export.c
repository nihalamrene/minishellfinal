#include "../../includes/parse.h"

int	get_idx(char *str, int c)
{
	int	i;

	i = 0;
	if (!str)
		return (-1);
	while (str[i])
	{
		if (str[i] == c)
			return (i);
		i++;
	}
	return (-1);
}

void	export_node(t_envi **envi, char *key, char *value, int append)
{
	t_envi	*ptr;

	ptr = *envi;
	while (ptr)
	{
		if (!ft_strcmp(ptr->key, key))
			break ;
		ptr = ptr->next;
	}
	if (!ptr)
		return (ft_lstadd_back_env(envi, ft_lstnew_env(key, value, 0)));
	if (append == 1)
		ptr->value = ft_strjoin(ptr->value, value);
	else if (value)
	{
		free(ptr->value);
		ptr->value = value;
	}
}

int	_export(char *line, t_envi **envi)
{
	char	*key;
	char	*value;
	int		append;
	int		equal;
	int		plus;

	equal = get_idx(line, '=');
	plus = get_idx(line, '+');
	append = 0;
	if (plus + 1 == equal)
	{
		append = 1;
		key = ft_substr(line, 0, plus);
	}
	else
		key = ft_substr(line, 0, equal);
	if (!check_var(key, "export", envi))
		return (1);
	if (equal != -1)
		value = ft_substr(line + equal + 1, 0, ft_strlen(line) - equal);
	else
		value = NULL;
	export_node(envi, key, value, append);
	return (0);
}

void	swap_node(t_envi **envi, t_envi *node)
{
	t_envi	*p_node;
	t_envi	*pp_node;

	pp_node = *envi;
	if (pp_node && node && pp_node->next
		&& !ft_strcmp(pp_node->next->key, node->key))
	{
		*envi = (*envi)->next;
		pp_node->next = (*envi)->next;
		(*envi)->next = pp_node;
		return ;
	}
	while (pp_node)
	{
		if (pp_node->next && pp_node->next->next
			&& !ft_strcmp(pp_node->next->next->key, node->key))
		{
			p_node = pp_node->next;
			p_node->next = node->next;
			node->next = p_node;
			pp_node->next = node;
			break ;
		}
		pp_node = pp_node->next;
	}
}

t_envi	*cpy_env(t_envi *envi)
{
	t_envi	*cpy;

	cpy = NULL;
	while (envi)
	{
		if (envi->value)
			ft_lstadd_back_env(&cpy, ft_lstnew_env(ft_strdup(envi->key), ft_strdup(envi->value), 0));
		else
			ft_lstadd_back_env(&cpy, ft_lstnew_env(ft_strdup(envi->key), NULL, 0));
		envi = envi->next;
	}
	return (cpy);
}
void	print_export(t_envi *cpy)
{
	while (cpy)
	{
		if (ft_strcmp(cpy->key, "?") && cpy->value)
			printf("declare -x %s=\"%s\"\n", cpy->key, cpy->value);
		else if (ft_strcmp(cpy->key, "?"))
			printf("declare -x %s\n", cpy->key);
		cpy = cpy->next;
	}
}

void	sort_export(t_envi *envi)
{
	t_envi	*cpy;
	t_envi	*ptr;
	int		sort_over;

	cpy = cpy_env(envi);
	while (1)
	{
		sort_over = 1;
		ptr = cpy;
		while (ptr)
		{
			if (ptr->next && ft_strcmp(ptr->key, ptr->next->key) > 0)
			{
				swap_node(&cpy, ptr->next);
				sort_over = 0;
			}
			ptr = ptr->next;
		}
		if (sort_over)
			break ;
	}
	print_export(cpy);
}
int	export(char **av, t_envi **envi)
{
	int	exit_stat;
	int	i;

	i = 0;
	exit_stat = 0;
	if (!av[i])
		sort_export(*envi);
	else
		while (av[i])
			if (_export(av[i++], envi))
				exit_stat = 1;
	return (exit_stat);
}