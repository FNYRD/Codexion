#include "codexion.h"

static void	reserving(t_data *data)
{
	data->workers = malloc(sizeof(t_worker) * data->n_workers);
	data->dongles = malloc(sizeof(t_dongle) * data->n_workers);
	data->heap = malloc(sizeof(t_worker *) * data->n_workers);
	data->waiting = malloc(sizeof(t_worker *) * data->n_workers);
	if (!data->workers || !data->dongles || !data->heap || !data->waiting)
	{
		if (data->workers)
			free(data->workers);
		if (data->dongles)
			free(data->dongles);
		if (data->heap)
			free(data->heap);
		if (data->waiting)
			free(data->waiting);
		exit (1);
	}
}

void	destroy(t_data *data, int i, int flag)
{
	if (flag)
		pthread_mutex_destroy(&data->dongles[i].mutex);
	i--;
	while (i >= 0)
	{
		pthread_mutex_destroy(&data->dongles[i].mutex);
		pthread_cond_destroy(&data->dongles[i].cond);
		i--;
	}
	freedom(data, 1, 1);
}

static void	mutexes(t_data *data)
{
	if (pthread_mutex_init(&data->print_mutex, NULL) != 0)
		exit (1);
	if (pthread_mutex_init(&data->general_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&data->print_mutex);
		exit (1);
	}
	if (pthread_cond_init(&data->general_cond, NULL) != 0)
	{
		pthread_mutex_destroy(&data->print_mutex);
		pthread_mutex_destroy(&data->general_mutex);
		exit (1);
	}
}

void	assigning(t_data *data, char **argv)
{
	data->n_workers = ft_atoi(argv[1]);
	data->stop = 0;
	data->bornout = ft_atol(argv[2]);
	data->compile = ft_atol(argv[3]);
	data->debug = ft_atol(argv[4]);
	data->refactoring = ft_atol(argv[5]);
	data->uses_per_worker = ft_atoi(argv[6]);
	data->cooldown = ft_atol(argv[7]);
	data->heap_size = -1;
	data->waiting_size = -1;
	data->bornout_flag = 0;
	mutexes(data);
	if (!ft_strcmp(argv[8], "fifo"))
		data->scheduler = 0;
	if (!ft_strcmp(argv[8], "edf"))
		data->scheduler = 1;
	reserving(data);
}

void	creating_table(t_data *data, int i, int j)
{
	dongling(data, i);
	while (++j < data->n_workers)
	{
		data->workers[j].id = j + 1;
		data->workers[j].lef_dongle = &data->dongles[j];
		if (data->n_workers == 1)
			data->workers[j].right_dongle = NULL;
		else if (j == data->n_workers - 1)
			data->workers[j].right_dongle = &data->dongles[0];
		else
			data->workers[j].right_dongle = &data->dongles[j + 1];
		data->workers[j].ready = 0;
		data->workers[j].data = data;
		data->workers[j].used = 0;
		data->workers[j].last_compile = get_time_ms();
	}
}
