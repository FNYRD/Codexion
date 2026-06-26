/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jericard <jericard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/26 12:51:56 by jericard          #+#    #+#             */
/*   Updated: 2026/06/26 13:02:39 by jericard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <unistd.h>
# include <sys/time.h>

# define INT_MAX_STR "2147483647"
# define LONG_MAX_STR "9223372036854775807"

typedef struct s_worker		t_worker;
typedef struct s_dongle		t_dongle;
typedef struct s_monitor	t_monitor;

typedef struct s_data
{
	int				stop;
	int				n_workers;
	int				heap_size;
	int				scheduler;
	int				waiting_size;
	int				bornout_flag;
	int				uses_per_worker;
	long			debug;
	long			compile;
	long			bornout;
	long			cooldown;
	long			refactoring;
	long			start_simulation;
	t_worker		**heap;
	t_worker		**waiting;
	t_worker		*workers;
	t_dongle		*dongles;
	pthread_mutex_t	print_mutex;
	pthread_cond_t	general_cond;
	pthread_mutex_t	general_mutex;
}	t_data;

typedef struct s_worker
{
	int			id;
	int			used;
	int			ready;
	long		to_bornout;
	long		last_compile;
	t_data		*data;
	pthread_t	thread;
	t_monitor	*monitor;
	t_dongle	*lef_dongle;
	t_dongle	*right_dongle;
}	t_worker;

typedef struct s_monitor
{
	int			finished;
	t_data		*data;
	pthread_t	thread;
	t_worker	*workers;
}	t_monitor;

typedef struct s_dongle
{
	int				id;
	int				available;
	long			release_time;
}	t_dongle;

int		len_str(char *argv);
int		verifier(char **argv);
int		len_matriz(char **argv);
int		ft_atoi(const char *nptr);
int		ft_strcmp(char *s1, char *s2);
int		continue_while(t_data *data, t_worker *worker);
int		heap_relatives(int index, int flag, t_data data);
int		swap_indexft(t_data *data, int right_index, int left_index);

long	get_time_ms(void);
long	ft_atol(const char *nptr);

void	wake_up(t_data *data);
void	heap_pop(t_data *data);
void	shift_up(t_data *data);
void	dongling(t_data *data, int i);
void	*worker_routine(void *worker_raw);
void	*monitor_routine(void *monitor_raw);
void	assigning(t_data *data, char **argv);
void	heap_ready(t_data *data, t_worker *new);
void	waiting_end(t_data	*data, int i, int j);
void	freedom(t_data *data, int code);
void	creating_table(t_data *data, int i, int j);
void	steering(t_data *data, t_worker *new, int i);
void	shift_down(t_data *data, int right_index, int left_index);
void	printf_log(t_data *data, t_worker *worker, long time, int flag);

#endif
