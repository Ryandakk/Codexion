/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   schedule2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: radakrou <radakrou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/24 18:05:49 by radakrou          #+#    #+#             */
/*   Updated: 2026/11/24 18:06:52 by radakrou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	submit_request(t_simulation *sim, int coder_id)
{
	t_coder		*coder;
	t_request	request;

	coder = &sim->coders[coder_id - 1];
	pthread_mutex_lock(&sim->scheduler.mutex);
	request.coder_id = coder_id;
	request.deadline = coder->last_compile_start + sim->config.time_to_burnout;
	request.order = sim->scheduler.next_order;
	sim->scheduler.next_order++;
	if (heap_insert(sim, request) != 0)
	{
		pthread_mutex_unlock(&sim->scheduler.mutex);
		return (1);
	}
	pthread_cond_broadcast(&sim->scheduler.condition);
	pthread_mutex_unlock(&sim->scheduler.mutex);
	return (0);
}

int	are_dongles_available(t_simulation *sim, int coder_id)
{
	int			left;
	int			right;
	int			count;
	long long	now;

	count = sim->config.number_of_coders;
	left = coder_id - 1;
	right = coder_id % count;
	now = get_current_time();
	if (sim->dongles[left].in_use == 1 || now < sim->dongles[left].available_at)
		return (0);
	if (count == 1)
		return (1);
	if (sim->dongles[right].in_use == 1
		|| now < sim->dongles[right].available_at)
		return (0);
	return (1);
}

int	release_dongles(t_simulation *sim, int coder_id)
{
	int			left;
	int			right;
	int			count;
	long long	now;

	count = sim->config.number_of_coders;
	left = coder_id - 1;
	right = coder_id % count;
	pthread_mutex_unlock(&sim->dongles[left].mutex);
	if (count > 1)
		pthread_mutex_unlock(&sim->dongles[right].mutex);
	now = get_current_time();
	pthread_mutex_lock(&sim->scheduler.mutex);
	sim->dongles[left].last_release_time = now;
	sim->dongles[left].available_at = now + sim->config.dongle_cooldown;
	sim->dongles[left].in_use = 0;
	if (count > 1)
	{
		sim->dongles[right].last_release_time = now;
		sim->dongles[right].available_at = now + sim->config.dongle_cooldown;
		sim->dongles[right].in_use = 0;
	}
	pthread_cond_broadcast(&sim->scheduler.condition);
	pthread_mutex_unlock(&sim->scheduler.mutex);
	return (0);
}

int	wait_for_turn(t_simulation *sim, int coder_id)
{
	int			left;
	int			right;
	int			count;
	int			result;
	long long	ready_at;

	count = sim->config.number_of_coders;
	left = coder_id - 1;
	right = coder_id % count;
	pthread_mutex_lock(&sim->scheduler.mutex);
	while (1)
	{
		if (sim->stop)
		{
			pthread_mutex_unlock(&sim->scheduler.mutex);
			return (0);
		}
		if (sim->scheduler.size > 0 && is_my_turn(sim, coder_id)
			&& are_dongles_available(sim, coder_id))
		{
			result = grant_dongles(sim, coder_id);
			pthread_mutex_unlock(&sim->scheduler.mutex);
			return (result);
		}
		if (sim->scheduler.size > 0 && is_my_turn(sim, coder_id)
			&& !dongles_in_use(sim, left, right, count))
		{
			ready_at = get_ready_time(sim, left, right, count);
			result = wait_for_cooldown(sim, ready_at);
		}
		else
			result = pthread_cond_wait(&sim->scheduler.condition,
					&sim->scheduler.mutex);
		if (result != 0 && result != ETIMEDOUT)
		{
			pthread_mutex_unlock(&sim->scheduler.mutex);
			return (0);
		}
	}
}

int	shares_dongle(t_simulation *sim, int a, int b)
{
	int	left_a;
	int	right_a;
	int	left_b;
	int	right_b;
	int	count;

	count = sim->config.number_of_coders;
	left_a = a - 1;
	right_a = a % count;
	left_b = b - 1;
	right_b = b % count;
	if (left_a == left_b || left_a == right_b || right_a == left_b
		|| right_a == right_b)
		return (1);
	return (0);
}
