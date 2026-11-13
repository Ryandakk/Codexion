
#include "codexion.h"
#include <limits.h>
#include <string.h>

int	is_valid_number(char *str)
{
	int	i;

	i = 0;
	if (str[0] == '\0')
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

int	ft_atoi(char *str)
{
	int	i;
	int	result;
	int	digit;

	i = 0;
	result = 0;
	while (str[i])
	{
		digit = str[i] - '0';
		if (result > (INT_MAX - digit) / 10)
			return (-1);
		result = result * 10 + digit;
		i++;
	}
	return (result);
}

int	is_valid_scheduler(char *str)
{
	if (strcmp(str, "fifo") == 0)
		return (1);
	if (strcmp(str, "edf") == 0)
		return (1);
	return (0);
}
