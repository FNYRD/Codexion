/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wake_up.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jericard <jericard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/26 12:57:48 by jericard          #+#    #+#             */
/*   Updated: 2026/06/26 13:04:27 by jericard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	wake_up(t_data *data)
{
	int	i;
	
	i = -1;
	pthread_mutex_lock(&data->general_mutex);
	while (++i < data->n_workers)
	{
		if (!data->dongles[i].available
			&& (get_time_ms() - data->dongles[i].release_time)
			>= data->cooldown)
		{
			data->dongles[i].available = 1;
			waiting_end(data, -1, 0);
			pthread_cond_broadcast(&data->general_cond);
		}
	}
	pthread_mutex_unlock(&data->general_mutex);
}
