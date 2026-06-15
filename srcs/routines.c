#include "codexion.h"

static int	worker_waiting_routine(t_worker *worker, t_data *data)
{
	if (data->stop)
	{
		pthread_mutex_unlock(&data->general_mutex);
		return (1);
	}
	steering(data, worker, -1);
	while (continue_while(data, worker))
		pthread_cond_wait(&data->general_cond, &data->general_mutex);
	if (data->stop)
	{
		pthread_mutex_unlock(&data->general_mutex);
		return (1);
	}
	return (0);
}

static void	worker_ending_routine(t_worker *worker, t_data *data)
{
	printf_log(data, worker, (get_time_ms() - data->start_simulation), 2);
	pthread_mutex_lock(&data->general_mutex);
	worker->last_compile = get_time_ms();
	pthread_mutex_unlock(&data->general_mutex);
	usleep(data->compile * 1000);
	pthread_mutex_lock(&data->general_mutex);
	worker->used += 1;
	if (worker->used == data->uses_per_worker)
		worker->monitor->finished += 1;
	worker->lef_dongle->available = 1;
	worker->right_dongle->available = 1;
	worker->ready = 0;
	waiting_end(data, -1, 0);
	pthread_cond_broadcast(&data->general_cond);
	pthread_mutex_unlock(&data->general_mutex);
	printf_log(data, worker, (get_time_ms() - data->start_simulation), 3);
	usleep(data->debug * 1000);
	printf_log(data, worker, (get_time_ms() - data->start_simulation), 4);
	usleep(data->refactoring * 1000);
}

void	*worker_routine(void *worker_raw)
{
	t_worker	*worker;
	t_data		*data;

	worker = (t_worker *)worker_raw;
	data = worker->data;
	while (worker->used < data->uses_per_worker)
	{
		pthread_mutex_lock(&data->general_mutex);
		if (worker_waiting_routine(worker, data))
			break ;
		worker->lef_dongle->available = 0;
		printf_log(data, worker, (get_time_ms() - data->start_simulation), 1);
		worker->right_dongle->available = 0;
		printf_log(data, worker, (get_time_ms() - data->start_simulation), 1);
		heap_pop(data);
		pthread_mutex_unlock(&data->general_mutex);
		worker_ending_routine(worker, data);
	}
	return (NULL);
}

static int	monitor_action(t_worker *workers, t_data *data, int i)
{
	long	start;

	start = data->start_simulation;
	pthread_mutex_lock(&data->general_mutex);
	if (workers[i].monitor->finished == data->n_workers)
	{
		data->stop = 1;
		pthread_cond_broadcast(&data->general_cond);
		pthread_mutex_unlock(&data->general_mutex);
		return (1);
	}
	else if ((get_time_ms() - workers[i].last_compile) >= data->bornout)
	{
		data->stop = 1;
		pthread_cond_broadcast(&data->general_cond);
		pthread_mutex_unlock(&data->general_mutex);
		printf_log(data, &workers[i], (get_time_ms() - start), 5);
		return (1);
	}
	pthread_mutex_unlock(&data->general_mutex);
	return (0);
}

void	*monitor_routine(void *monitor_raw)
{
	t_monitor	*monitor;
	t_data		*data;
	t_worker	*workers;
	int			condition;
	int			i;

	condition = 1;
	monitor = (t_monitor *)monitor_raw;
	data = (t_data *)monitor->data;
	workers = (t_worker *)monitor->workers;
	while (condition)
	{
		i = -1;
		while (++i < data->n_workers && condition)
		{
			if (monitor_action(workers, data, i))
				condition = 0;
			else
				usleep(500);
		}
	}
	return (NULL);
}
