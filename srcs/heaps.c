/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heaps.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jericard <jericard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/26 12:51:38 by jericard          #+#    #+#             */
/*   Updated: 2026/06/26 12:51:39 by jericard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	heap_relatives(int index, int flag, t_data data)
{
	if (flag == 1)
		if (((index * 2) + 1) <= data.heap_size)
			return ((index * 2) + 1);
	if (flag == 2)
		if (((index * 2) + 2) <= data.heap_size)
			return ((index * 2) + 2);
	if (index > 0)
		if (flag == 3)
			if (((index - 1) / 2) >= 0)
				return ((index - 1) / 2);
	return (-1);
}

void	heap_ready(t_data *data, t_worker *new)
{
	t_worker	**heap;
	int			index;

	if ((data->heap_size + 1) == data->n_workers)
		return ;
	heap = data->heap;
	data->heap_size++;
	heap[data->heap_size] = new;
	index = data->heap_size;
	if (index > 0 && data->scheduler)
		shift_up(data);
}

void	heap_pop(t_data *data)
{
	t_worker	**heap;
	int			right_index;
	int			left_index;
	int			i;

	i = 0;
	if (data->heap_size == -1)
		return ;
	heap = data->heap;
	if (!data->scheduler)
	{
		while (++i <= data->heap_size)
			heap[i - 1] = heap[i];
		data->heap_size--;
		return ;
	}
	heap[0] = heap[data->heap_size];
	data->heap_size--;
	right_index = heap_relatives(0, 2, *data);
	left_index = heap_relatives(0, 1, *data);
	shift_down(data, right_index, left_index);
}
