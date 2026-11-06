/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: radakrou <radakrou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 18:15:34 by radakrou          #+#    #+#             */
/*   Updated: 2026/11/04 18:15:52 by radakrou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "codexion.h"

int	main(int argc, char **argv)
{
	int			j;
	int			number;
	t_config	config;

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
	printf("Coders: %d\n", config.number_of_coders);
	printf("Burnout: %d\n", config.time_to_burnout);
	printf("Compile: %d\n", config.time_to_compile);
	printf("Debug: %d\n", config.time_to_debug);
	printf("Refactor: %d\n", config.time_to_refactor);
	printf("Required compiles: %d\n",
		config.number_of_compiles_required);
	printf("Cooldown: %d\n", config.dongle_cooldown);
	printf("Scheduler: %s\n", config.scheduler);
	printf("Arguments are valid\n");
	return (0);
}
