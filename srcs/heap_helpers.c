/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_helpers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jericard <jericard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/26 12:51:36 by jericard          #+#    #+#             */
/*   Updated: 2026/06/26 12:51:37 by jericard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	shift_up(t_data *data)
{
	int			index;
	long		last;
	long		dad;
	t_worker	**heap;
	t_worker	*swap;

	index = data->heap_size;
	heap = data->heap;
	last = heap[index]->last_compile;
	dad = heap[heap_relatives(index, 3, *data)]->last_compile;
	while (last < dad)
	{
		swap = heap[index];
		heap[index] = heap[heap_relatives(index, 3, *data)];
		heap[heap_relatives(index, 3, *data)] = swap;
		index = heap_relatives(index, 3, *data);
		if (index == 0)
			break ;
		last = heap[index]->last_compile;
		dad = heap[heap_relatives(index, 3, *data)]->last_compile;
	}
}

int	swap_indexft(t_data *data, int right_index, int left_index)
{
	t_worker	**heap;
	int			swap_index;

	heap = data->heap;
	if ((left_index + right_index) == -2)
		return (0);
	if (right_index > -1)
	{
		if (heap[right_index]->last_compile < heap[left_index]->last_compile)
			swap_index = right_index;
		else
			swap_index = left_index;
	}
	else
		swap_index = left_index;
	return (swap_index);
}

void	shift_down(t_data *data, int right_index, int left_index)
{
	int			swap;
	int			dad;
	t_worker	*swap_worker;

	while (1)
	{
		swap = swap_indexft(data, right_index, left_index);
		if (!swap)
			return ;
		dad = heap_relatives(swap, 3, *data);
		if (data->heap[swap]->last_compile < data->heap[dad]->last_compile)
		{
			swap_worker = data->heap[dad];
			data->heap[dad] = data->heap[swap];
			data->heap[swap] = swap_worker;
		}
		else
			return ;
		right_index = heap_relatives(swap, 2, *data);
		left_index = heap_relatives(swap, 1, *data);
	}
}
