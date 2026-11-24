/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   schedule.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: radakrou <radakrou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/06 18:59:56 by radakrou          #+#    #+#             */
/*   Updated: 2026/11/24 18:05:42 by radakrou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	has_higher_priority(t_simulation *sim, t_request a, t_request b)
{
	if (strcmp(sim->config.scheduler, "fifo") == 0)
		return (a.order < b.order);
	if (a.deadline != b.deadline)
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
		if (has_higher_priority(sim, sim->scheduler.heap[index],
				sim->scheduler.heap[parent]))
		{
			temp = sim->scheduler.heap[index];
			sim->scheduler.heap[index] = sim->scheduler.heap[parent];
			sim->scheduler.heap[parent] = temp;
			index = parent;
		}
		else
			break ;
	}
}

int	heap_insert(t_simulation *sim, t_request request)
{
	int	index;

	if (sim->scheduler.size >= sim->scheduler.capacity)
		return (1);
	index = sim->scheduler.size;
	sim->scheduler.heap[index] = request;
	sim->scheduler.size++;
	heapify_up(sim, index);
	return (0);
}

int	heap_pop(t_simulation *sim, t_request *request, int coder_id)
{
	int	i;

	i = 0;
	while (i < sim->scheduler.size
		&& sim->scheduler.heap[i].coder_id != coder_id)
		i++;
	if (i == sim->scheduler.size)
		return (1);
	*request = sim->scheduler.heap[i];
	sim->scheduler.size--;
	if (i != sim->scheduler.size)
	{
		sim->scheduler.heap[i] = sim->scheduler.heap[sim->scheduler.size];
		heapify_down(sim, i);
		heapify_up(sim, i);
	}
	return (0);
}

void	heapify_down(t_simulation *sim, int index)
{
	int			left;
	int			right;
	int			highest;
	t_request	temp;

	while (1)
	{
		left = (2 * index) + 1;
		right = (2 * index) + 2;
		highest = index;
		if (left < sim->scheduler.size && has_higher_priority(sim,
				sim->scheduler.heap[left], sim->scheduler.heap[highest]))
			highest = left;
		if (right < sim->scheduler.size && has_higher_priority(sim,
				sim->scheduler.heap[right], sim->scheduler.heap[highest]))
			highest = right;
		if (highest == index)
			break ;
		temp = sim->scheduler.heap[index];
		sim->scheduler.heap[index] = sim->scheduler.heap[highest];
		sim->scheduler.heap[highest] = temp;
		index = highest;
	}
}
