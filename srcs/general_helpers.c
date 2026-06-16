#include "codexion.h"

void	freedom(t_data *data, int code)
{
	int	i;

	i = 0;
	while (i < data->n_workers)
	{
		data->workers[i].data = NULL;
		data->workers[i].lef_dongle = NULL;
		data->workers[i].right_dongle = NULL;
		i++;
	}
	pthread_mutex_destroy(&data->general_mutex);
	pthread_cond_destroy(&data->general_cond);
	pthread_mutex_destroy(&data->print_mutex);
	free(data->dongles);
	free(data->heap);
	free(data->waiting);
	free(data->workers);
	exit (code);
}

int	ft_strcmp(char *s1, char *s2)
{
	int	i;

	i = 0;
	while (s1[i] && s2[i])
	{
		if (s1[i] != s2[i])
			return (1);
		i++;
	}
	if (!s1[i] && !s2[i])
		return (0);
	return (1);
}

void	dongling(t_data *data, int i)
{
	while (++i < data->n_workers)
	{
		data->dongles[i].id = i;
		data->dongles[i].release_time = 0;
		data->dongles[i].available = 1;
	}
}
