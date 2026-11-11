/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   schedule.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: radakrou <radakrou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/06 18:59:56 by radakrou          #+#    #+#             */
/*   Updated: 2026/11/10 19:42:07 by radakrou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int has_higher_priority(t_simulation *sim, t_request a, t_request b)
{
    if(strcmp(sim->config.scheduler, "fifo") == 0)
        return (a.order < b.order);
    if(a.deadline != b.deadline)
        return (a.deadline < b.deadline);
    return (a.order < b.order);
}

void	heapify_up(t_simulation *sim, int index)
{
	int			parent;
	t_request	temp;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (has_higher_priority(sim, sim->scheduler.heap[index], sim->scheduler.heap[parent]))
		{
			temp = sim->scheduler.heap[index];
			sim->scheduler.heap[index] = sim->scheduler.heap[parent];
			sim->scheduler.heap[parent] = temp;
			index = parent;
		}
		else
			break;
	}
}

int	heap_insert(t_simulation *sim, t_request request)
{
	int index;

	if(sim->scheduler.size >= sim->scheduler.capacity)
		return (1);

	index = sim->scheduler.size;
	sim->scheduler.heap[index] = request;
	sim->scheduler.size++;
	heapify_up(sim, index);
	return (0);
}

int	heap_pop(t_simulation *sim, t_request *request)
{
	t_scheduler *scheduler;

	scheduler = &sim->scheduler;
	if (scheduler->size == 0)
		return (1);

	*request = scheduler->heap[0];
	scheduler->size--;
	if (scheduler->size > 0)
	{
		scheduler->heap[0] = scheduler->heap[scheduler->size];
		heapify_down(sim, 0);
	}
	return (0);
}

void	heapify_down(t_simulation	*sim, int index)
{
	int			left;
	int			right;
	int			highest;
	t_request	temp;

	while(1)
	{
		left = (2 * index) + 1;
		right = (2 * index) + 2;
		highest = index;

		if (left < sim->scheduler.size
				&& has_higher_priority(sim, sim->scheduler.heap[left],
					sim->scheduler.heap[highest]))
			highest = left;
		if (right < sim->scheduler.size
				&& has_higher_priority(sim, sim->scheduler.heap[right],
					sim->scheduler.heap[highest]))
			highest = right;
		if (highest == index)
			break;
		temp = sim->scheduler.heap[index];
		sim->scheduler.heap[index] = sim->scheduler.heap[highest];
		sim->scheduler.heap[highest] = temp;
		index = highest;
	}
}

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

	if(heap_insert(sim, request) != 0)
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
	if(sim->dongles[left].in_use == 1
		|| now < sim->dongles[left].available_at)
			return (0);
	if (count == 1)
		return (1);
	if(sim->dongles[right].in_use == 1
		|| now < sim->dongles[right].available_at)
		return (0);
	return (1);
}

int	aquire_dongles(t_simulation *sim, int coder_id)
{
	int 		left;
    int 		right;
    int 		count;
	t_request	request;

	count = sim->config.number_of_coders;
	left = coder_id - 1;
	right = coder_id % count;

	pthread_mutex_lock(&sim->scheduler.mutex);
	if (sim->scheduler.size == 0
		|| sim->scheduler.heap[0].coder_id != coder_id)
	{
		pthread_mutex_unlock(&sim->scheduler.mutex);
		return (0);
	}
	if (!are_dongles_available(sim, coder_id))
	{
		pthread_mutex_unlock(&sim->scheduler.mutex);
		return (0);
	}
	heap_pop(sim, &request);
	sim->dongles[left].in_use = 1;
	if(count > 1)
		sim->dongles[right].in_use = 1;
	pthread_mutex_lock(&sim->dongles[left].mutex);
	if (count > 1)
    	pthread_mutex_lock(&sim->dongles[right].mutex);
    pthread_mutex_unlock(&sim->scheduler.mutex);
    return (1);
}

int	release_dongles(t_simulation *sim, int coder_id)
{
	int			left;
	int 		right;
	int 		count;
	long long	now;

	count = sim->config.number_of_coders;
	left = coder_id - 1;
	right = coder_id % count;
	now = get_current_time();

	pthread_mutex_lock(&sim->scheduler.mutex);
	pthread_mutex_unlock(&sim->dongles[left].mutex);
	sim->dongles[left].last_release_time = now;
	sim->dongles[left].available_at = now + sim->config.dongle_cooldown;
	sim->dongles[left].in_use = 0;

	if (count > 1)
	{
		pthread_mutex_lock(&sim->dongles[right].mutex);
		sim->dongles[right].last_release_time = now;
		sim->dongles[right].available_at = now 
		+ sim->config.dongle_cooldown;
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
	while(1)
	{
		if(sim->scheduler.size > 0
			&& sim->scheduler.heap[0].coder_id == coder_id
			&& are_dongles_available(sim, coder_id))
		{
			result = grant_dongles(sim, coder_id);
			pthread_mutex_unlock(&sim->scheduler.mutex);
			return (result);
		}
		if(sim->scheduler.size > 0
			&& sim->scheduler.heap[0].coder_id == coder_id
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