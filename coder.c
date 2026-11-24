/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: radakrou <radakrou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/11 17:03:47 by radakrou          #+#    #+#             */
/*   Updated: 2026/11/13 19:30:38 by radakrou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*coder_routine(void *arg)
{
	t_simulation	*sim;
	t_coder			*coder;

	coder = (t_coder *)arg;
	sim = coder->sim;
	while (coder->compile_count < sim->config.number_of_compiles_required)
	{
		if (compile_phase(coder) != 0)
			break ;
		debug_phase(coder);
		if (sim_stopped(sim))
			break ;
		refactor_phase(coder);
		if (sim_stopped(sim))
			break ;
	}
	pthread_mutex_lock(&sim->scheduler.mutex);
	coder->state = FINISHED;
	pthread_cond_broadcast(&sim->scheduler.condition);
	pthread_mutex_unlock(&sim->scheduler.mutex);
	return (NULL);
}

int	run_coders(t_simulation *sim)
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
		return (1);
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL, coder_routine,
				&sim->coders[i]) != 0)
			break ;
		created++;
		i++;
	}
	if (created != sim->config.number_of_coders)
	{
		pthread_mutex_lock(&sim->scheduler.mutex);
		sim->stop = 1;
		pthread_cond_broadcast(&sim->scheduler.condition);
		pthread_mutex_unlock(&sim->scheduler.mutex);
	}
	i = 0;
	while (i < created)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	pthread_mutex_lock(&sim->scheduler.mutex);
	sim->stop = 1;
	pthread_cond_broadcast(&sim->scheduler.condition);
	pthread_mutex_unlock(&sim->scheduler.mutex);
	pthread_join(sim->monitor_thread, NULL);
	if (created != sim->config.number_of_coders)
		return (1);
	return (0);
}

int	compile_phase(t_coder *coder)
{
	t_simulation	*sim;

	sim = coder->sim;
	if (sim->config.number_of_coders == 1)
	{
		print_status(sim, coder->id, "has taken a dongle");
		while (!sim_stopped(sim))
			usleep(500);
		return (1);
	}
	if (submit_request(sim, coder->id) != 0)
		return (1);
	if (wait_for_turn(sim, coder->id) == 0)
		return (1);
	print_status(sim, coder->id, "has taken a dongle");
	print_status(sim, coder->id, "has taken a dongle");
	print_status(sim, coder->id, "is compiling");
	if (!sleep_interrupt(sim, sim->config.time_to_compile))
	{
		release_dongles(sim, coder->id);
		return (1);
	}
	coder->compile_count++;
	release_dongles(sim, coder->id);
	pthread_mutex_lock(&sim->scheduler.mutex);
	coder->state = DEBUGGING;
	pthread_mutex_unlock(&sim->scheduler.mutex);
	return (0);
}

void	debug_phase(t_coder *coder)
{
	t_simulation	*sim;

	sim = coder->sim;
	print_status(sim, coder->id, "is debugging");
	if (!sleep_interrupt(sim, sim->config.time_to_debug))
		return ;
	pthread_mutex_lock(&sim->scheduler.mutex);
	coder->state = REFACTORING;
	pthread_mutex_unlock(&sim->scheduler.mutex);
}

void	refactor_phase(t_coder *coder)
{
	t_simulation	*sim;

	sim = coder->sim;
	print_status(sim, coder->id, "is refactoring");
	if (!sleep_interrupt(sim, sim->config.time_to_refactor))
		return ;
}

int	sim_stopped(t_simulation *sim)
{
	int	stopped;

	pthread_mutex_lock(&sim->scheduler.mutex);
	stopped = sim->stop;
	pthread_mutex_unlock(&sim->scheduler.mutex);
	return (stopped);
}
