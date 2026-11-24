/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: radakrou <radakrou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/24 18:30:25 by radakrou          #+#    #+#             */
/*   Updated: 2026/11/24 19:14:08 by radakrou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	lock_broadcast(t_simulation *sim)
{
	pthread_mutex_lock(&sim->scheduler.mutex);
	sim->stop = 1;
	pthread_cond_broadcast(&sim->scheduler.condition);
	pthread_mutex_unlock(&sim->scheduler.mutex);
}

int	start_coders(t_simulation *sim)
{
	int	i;
	int	created;

	i = 0;
	created = 0;
	sim->stop = 0;
	sim->start_time = get_current_time();
	while (i < sim->config.number_of_coders)
	{
		sim->coders[i].last_compile_start = sim->start_time;
		i++;
	}
	if (pthread_create(&sim->monitor_thread, NULL, monitor_routine, sim) != 0)
		return (-1);
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL, coder_routine,
				&sim->coders[i]) != 0)
			break ;
		created++;
		i++;
	}
	return (created);
}

int	lonely_coder(t_simulation *sim, int coder_id)
{
	if (sim->config.number_of_coders != 1)
		return (0);
	print_status(sim, coder_id, "has taken a dongle");
	while (!sim_stopped(sim))
		usleep(500);
	return (1);
}

int	find_burned_out(t_simulation *sim, long long now)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (sim->coders[i].state != FINISHED
			&& now - sim->coders[i].last_compile_start
			>= sim->config.time_to_burnout)
		{
			sim->stop = 1;
			return (sim->coders[i].id);
		}
		i++;
	}
	return (0);
}
