/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-oirg <zel-oirg@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/25 22:09:38 by zel-oirg          #+#    #+#             */
/*   Updated: 2024/08/28 01:05:21 by zel-oirg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/parse.h"

void    set_exit_status(int status, t_envi **envi)
{
    t_envi  *env_exit;

    env_exit = find_node(*envi, "?");
    env_exit->value = ft_itoa(status);
}

int nbr_arg(char **av)
{
    int i;

    if (!av && !*av)
        return (0);
    i = 0;
    while (av[i])
        i++;
    return (i);
}

static int check_arg(char *str, t_envi **envi)
{
    int i;

    i = 0;
    while (str[i])
    {
        if (!ft_isdigit(str[i]))
        {
            set_exit_status(255, envi);
            // g_exit_status = 255;//set $? to 255
            return (ft_putstr_fd("minishell: ", 2),ft_putstr_fd(str, 2),ft_putstr_fd(": numeric argument required\n", 2)
                ,  exit(255), 0);//bash: exit: ds: numeric argument required
        }
        i++;
    }
    return (1);
}

int    _exit_(char **av, t_envi **envi)
{
    int n;

    ft_putstr_fd("exit\n", 1);
    if (!*av)
    {
        set_exit_status(0, envi);
        exit(0);
        return (0);
    }
    else if (nbr_arg(av) >= 2)
    {
        ft_putstr_fd("minishell: exit: too many arguments\n", 2);
        set_exit_status(1, envi);
        exit(1);
        return (1);
    }
    else
    {
        if (check_arg(*av, envi))
        {
            n = ft_atoi(*av);
            set_exit_status(n, envi);
            // g_exit_status = n;//set the $? to 
            return (exit(n), n);
        }
    }
    return (0);
}
