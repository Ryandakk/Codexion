/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: radakrou <radakrou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/24 17:55:00 by radakrou          #+#    #+#             */
/*   Updated: 2026/11/24 18:48:16 by radakrou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

int	check_args(int argc, char **argv)
{
	int	j;
	int	number;

	if (argc != 9)
	{
		printf("Error: Wrong number of arguments\n");
		return (1);
	}
	j = 1;
	while (j <= 7)
	{
		if (!is_valid_number(argv[j]))
		{
			printf("Error: Invalid number\n");
			return (1);
		}
		number = ft_atoi(argv[j]);
		if (number == -1)
		{
			printf("Error: Number overflow\n");
			return (1);
		}
		j++;
	}
	if (!is_valid_scheduler(argv[8]))
	{
		printf("Error: Invalid scheduler name\n");
		return (1);
	}
	return (0);
}
