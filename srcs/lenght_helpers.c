/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lenght_helpers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jericard <jericard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/26 12:51:43 by jericard          #+#    #+#             */
/*   Updated: 2026/06/26 12:51:44 by jericard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	len_matriz(char **argv)
{
	int	lenght;

	lenght = 0;
	while (argv[lenght])
		lenght++;
	return (lenght);
}

int	len_str(char *argv)
{
	int	lenght;

	lenght = 0;
	while (argv[lenght])
		lenght++;
	return (lenght);
}
