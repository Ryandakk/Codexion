/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: radakrou <radakrou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/24 18:04:29 by radakrou          #+#    #+#             */
/*   Updated: 2026/11/24 19:00:07 by radakrou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	cleanup_simulation(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->initialized_dongles)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		i++;
	}
	destroy_mutexes(sim);
	free(sim->scheduler.heap);
	free(sim->coders);
	free(sim->dongles);
	sim->scheduler.heap = NULL;
	sim->coders = NULL;
	sim->dongles = NULL;
	sim->initialized_dongles = 0;
	sim->scheduler_mutex_initialized = 0;
	sim->initialized_log_mutexes = 0;
	sim->initialized_stop_mutexes = 0;
	sim->initialized_conditions = 0;
}

void	destroy_mutexes(t_simulation *sim)
{
	if (sim->initialized_conditions > 0)
		pthread_cond_destroy(&sim->scheduler.condition);
	if (sim->scheduler_mutex_initialized == 1)
		pthread_mutex_destroy(&sim->scheduler.mutex);
	if (sim->initialized_log_mutexes > 0)
		pthread_mutex_destroy(&sim->log_mutex);
	if (sim->initialized_stop_mutexes > 0)
		pthread_mutex_destroy(&sim->stop_mutex);
}

void	init_config(t_config *config, char **argv)
{
	config->number_of_coders = ft_atoi(argv[1]);
	config->time_to_burnout = ft_atoi(argv[2]);
	config->time_to_compile = ft_atoi(argv[3]);
	config->time_to_debug = ft_atoi(argv[4]);
	config->time_to_refactor = ft_atoi(argv[5]);
	config->number_of_compiles_required = ft_atoi(argv[6]);
	config->dongle_cooldown = ft_atoi(argv[7]);
	config->scheduler = argv[8];
}
