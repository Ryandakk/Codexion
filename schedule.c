/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   schedule.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: radakrou <radakrou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/06 18:59:56 by radakrou          #+#    #+#             */
/*   Updated: 2026/11/06 19:51:19 by radakrou         ###   ########.fr       */
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

	while (index > 1)
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