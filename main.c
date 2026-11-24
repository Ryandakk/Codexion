/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: radakrou <radakrou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 18:15:34 by radakrou          #+#    #+#             */
/*   Updated: 2026/11/13 19:30:53 by radakrou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdio.h>

int	main(int argc, char **argv)
{
	int				j;
	int				number;
	t_simulation	sim;
	t_config		config;

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
	init_config(&config, argv);
	if (config.number_of_coders == 0)
	{
		printf("Error: no coders provided\n");
		return (1);
	}
	if (init_simulation(&sim, &config) != 0)
	{
		printf("Error : simulation could not be initialized\n");
		return (1);
	}
	if (run_coders(&sim) != 0)
	{
		cleanup_simulation(&sim);
		return (1);
	}
	cleanup_simulation(&sim);
	return (0);
}
