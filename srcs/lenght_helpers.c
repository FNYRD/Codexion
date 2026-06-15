#include "codexion.h"

int	len_matriz(char **argv)
{
	int	lenght;

	lenght = 0;
	while (argv[lenght])
		lenght++;
	return (lenght);
}

int	len_str(char *argv)
{
	int	lenght;

	lenght = 0;
	while (argv[lenght])
		lenght++;
	return (lenght);
}
