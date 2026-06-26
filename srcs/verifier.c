/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   verifier.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jericard <jericard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/26 12:51:50 by jericard          #+#    #+#             */
/*   Updated: 2026/06/26 12:51:51 by jericard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_num(char *data)
{
	int	i;

	i = 0;
	while (data[i])
	{
		if (data[i] < '0' || data[i] > '9')
			return (1);
		i++;
	}
	return (0);
}

static int	is_max_int(char *number)
{
	int	i;
	int	len;
	int	len_number;

	i = -1;
	len = len_str(INT_MAX_STR);
	len_number = len_str(number);
	if (len_number < len)
		return (0);
	if (len_number > len)
		return (1);
	while (++i < len)
	{
		if (number[i] > INT_MAX_STR[i])
			return (1);
		else
			if (number[i] < INT_MAX_STR[i])
				return (0);
	}
	return (0);
}

static int	is_max_long(char *number)
{
	int	i;
	int	len;
	int	len_number;

	i = -1;
	len = len_str(LONG_MAX_STR);
	len_number = len_str(number);
	if (len_number < len)
		return (0);
	if (len_number > len)
		return (1);
	while (++i < len)
	{
		if (number[i] > LONG_MAX_STR[i])
			return (1);
		else
			if (number[i] < LONG_MAX_STR[i])
				return (0);
	}
	return (0);
}

int	verifier(char **argv)
{
	int	limit;
	int	i;
	int	j;

	i = 0;
	limit = len_matriz(argv) - 1;
	if (ft_strcmp(argv[limit], "fifo") && ft_strcmp(argv[limit], "edf"))
		return (1);
	while (++i < limit)
	{
		if ((i == 6 || i == 1) && is_max_int(argv[i]))
			return (1);
		if ((i != 6 && i != 1) && is_max_long(argv[i]))
			return (1);
		if (argv[i][0] == '\0')
			return (1);
		j = -1;
		while (++j < len_str(argv[i]))
			if (is_num(&argv[i][j]) || argv[i][0] < '1')
				return (1);
	}
	return (0);
}
