#include "codexion.h"

static void	helper_freedom(t_data *data, int flag)
{
	int	i;

	i = -1;
	pthread_mutex_lock(&data->general_mutex);
	data->stop = 1;
	pthread_cond_broadcast(&data->general_cond);
	pthread_mutex_unlock(&data->general_mutex);
	while (++i <= (flag - 1))
		pthread_join(data->workers[i].thread, NULL);
	freedom(data, 1);
}

static void	starting_routines(t_data *data, t_monitor *monitor)
{
	int	j;
	int	k;

	j = -1;
	k = -1;
	while (++j < data->n_workers)
		if ((pthread_create(&data->workers[j].thread, NULL,
					worker_routine, &data->workers[j])) != 0)
			helper_freedom(data, j);
	if ((pthread_create(&monitor->thread, NULL,
				monitor_routine, monitor)) != 0)
		helper_freedom(data, data->n_workers);
	while (++k < data->n_workers)
		if ((pthread_join(data->workers[k].thread, NULL)) != 0)
			freedom(data, 1);
	if ((pthread_join(monitor->thread, NULL)) != 0)
		freedom(data, 1);
}

int	main(int argc, char **argv)
{
	int			i;
	t_data		data;
	t_monitor	monitor;

	if (argc != 9 || verifier(argv))
		return (1);
	i = -1;
	data.start_simulation = get_time_ms();
	assigning(&data, argv);
	creating_table(&data, -1, -1);
	monitor.data = &data;
	monitor.finished = 0;
	monitor.workers = data.workers;
	while (++i < data.n_workers)
		data.workers[i].monitor = &monitor;
	starting_routines(&data, &monitor);
	freedom(&data, 0);
	return (0);
}

