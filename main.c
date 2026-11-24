/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: radakrou <radakrou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 18:15:34 by radakrou          #+#    #+#             */
/*   Updated: 2026/11/24 19:04:08 by radakrou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdio.h>

int	main(int argc, char **argv)
{
	t_simulation	sim;
	t_config		config;

	if (check_args(argc, argv) != 0)
		return (1);
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

void	print_status(t_simulation *sim, int id, char *message)
{
	long long	timestamp;

	pthread_mutex_lock(&sim->log_mutex);
	if (!sim_stopped(sim))
	{
		timestamp = get_current_time() - sim->start_time;
		printf("%lld %d %s\n", timestamp, id, message);
	}
	pthread_mutex_unlock(&sim->log_mutex);
}

void	print_burnout(t_simulation *sim, int id, long long now)
{
	long long	timestamp;

	timestamp = now - sim->start_time;
	pthread_mutex_lock(&sim->log_mutex);
	printf("%lld %d %s\n", timestamp, id, "burned out");
	pthread_mutex_unlock(&sim->log_mutex);
}

int	is_my_turn(t_simulation *sim, int coder_id)
{
	int			i;
	int			count;
	int			other;
	t_request	my_request;

	count = sim->config.number_of_coders;
	if (!find_request(sim, coder_id, &my_request))
		return (0);
	i = 0;
	while (i < sim->scheduler.size)
	{
		other = sim->scheduler.heap[i].coder_id;
		if (sim->scheduler.heap[i].coder_id != coder_id
			&& has_higher_priority(sim, sim->scheduler.heap[i], my_request)
			&& !dongles_in_use(sim, other - 1, other % count, count)
			&& shares_dongle(sim, sim->scheduler.heap[i].coder_id, coder_id))
			return (0);
		i++;
	}
	return (1);
}

int	find_request(t_simulation *sim, int coder_id, t_request *my_request)
{
	int	i;
	int	found;

	i = 0;
	found = 0;
	while (i < sim->scheduler.size)
	{
		if (sim->scheduler.heap[i].coder_id == coder_id)
		{
			*my_request = sim->scheduler.heap[i];
			found = 1;
		}
		i++;
	}
	return (found);
}
