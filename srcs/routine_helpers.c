/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine_helpers.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jericard <jericard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/26 12:51:45 by jericard          #+#    #+#             */
/*   Updated: 2026/06/26 12:55:21 by jericard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	waiting_push(t_data *data, t_worker *new)
{
	if (new->ready || ((data->waiting_size + 1) == data->n_workers))
		return ;
	data->waiting_size++;
	data->waiting[data->waiting_size] = new;
}

void	steering(t_data *data, t_worker *new, int i)
{
	int	ready;
	int	flag;

	flag = 0;
	ready = ((data->n_workers > 1)
			&& (new->lef_dongle->available && new->right_dongle->available));
	if (ready)
	{
		new->ready = 1;
		while (++i <= data->heap_size)
			if (data->heap[i]->id == new->id)
				flag = 1;
		if (!flag)
			heap_ready(data, new);
	}
	else
	{
		while (++i <= data->waiting_size)
			if (data->waiting[i]->id == new->id)
				flag = 1;
		if (!flag)
			waiting_push(data, new);
	}
}

void	waiting_end(t_data	*data, int i, int j)
{
	t_worker	**waiting;
	int			ready;

	waiting = data->waiting;
	if (data->waiting_size == -1)
		return ;
	while (++i <= data->waiting_size)
	{
		ready = (((waiting[i]->lef_dongle
						&& waiting[i]->lef_dongle->available)
					&& (waiting[i]->right_dongle
						&& waiting[i]->right_dongle->available)));
		if (ready)
		{
			waiting[i]->ready = 1;
			heap_ready(data, waiting[i]);
			j = i;
			if (i < data->waiting_size)
				while (++j <= data->waiting_size)
					waiting[j - 1] = waiting[j];
			i--;
			data->waiting_size--;
		}
	}
}

void	printf_log(t_data *data, t_worker *worker, long time, int flag)
{
	pthread_mutex_lock(&data->print_mutex);
	if (!data->bornout_flag)
	{
		if (flag == 1)
			printf("%ld %d has taken a dongle\n", time, worker->id);
		if (flag == 2)
			printf("%ld %d is compiling\n", time, worker->id);
		if (flag == 3)
			printf("%ld %d is debugging\n", time, worker->id);
		if (flag == 4)
			printf("%ld %d is refactoring\n", time, worker->id);
		if (flag == 5)
		{
			printf("%ld %d burned out\n", time, worker->id);
			data->bornout_flag = 1;
		}
	}
	pthread_mutex_unlock(&data->print_mutex);
}

int	continue_while(t_data *data, t_worker *worker)
{
	if (data->stop)
		return (0);
	if (!worker->lef_dongle || !worker->right_dongle)
		return (1);
	if (!worker->lef_dongle->available)
		return (1);
	if (!worker->right_dongle->available)
		return (1);
	if (data->heap_size < 0)
		return (1);
	if (data->heap[0]->id != worker->id)
		return (1);
	return (0);
}
