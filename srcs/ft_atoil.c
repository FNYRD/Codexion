#include "codexion.h"

int	ft_atoi(const char *nptr)
{
	int	i;
	int	ret;
	int	signal;

	i = 0;
	ret = 0;
	signal = 1;
	while (nptr[i] == 32 || (nptr[i] >= '\t' && nptr[i] <= '\r'))
		i++;
	if (nptr[i] == '+' || nptr[i] == '-')
	{
		if (nptr[i] == '-')
			signal *= -1;
		i++;
	}
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		ret = ret * 10 + (nptr[i] - '0');
		i++;
	}
	return (ret * signal);
}

long	ft_atol(const char *nptr)
{
	long	i;
	long	ret;
	long	signal;

	i = 0;
	ret = 0;
	signal = 1;
	while (nptr[i] == 32 || (nptr[i] >= '\t' && nptr[i] <= '\r'))
		i++;
	if (nptr[i] == '+' || nptr[i] == '-')
	{
		if (nptr[i] == '-')
			signal *= -1;
		i++;
	}
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		ret = ret * 10 + (nptr[i] - '0');
		i++;
	}
	return (ret * signal);
}
