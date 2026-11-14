/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: radakrou <radakrou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/10 19:09:56 by radakrou          #+#    #+#             */
/*   Updated: 2026/11/10 19:22:56 by radakrou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int dongles_in_use(t_simulation *sim, int left, int right, int count)
{
    if (sim->dongles[left].in_use == 1)
        return (1);
    if (count > 1 && sim->dongles[right].in_use == 1)
        return (1);
    return (0);
}

long long get_ready_time(t_simulation *sim, int left, int right, int count)
{
    long long ready_at;
    
    ready_at = sim->dongles[left].available_at;
    if (count > 1 && sim->dongles[right].available_at > ready_at)
        ready_at = sim->dongles[right].available_at;
    return (ready_at);
}

int	wait_for_cooldown(t_simulation *sim, long long ready_at)
{
	struct timespec	timeout;
	long long		remaining;

	remaining = ready_at - get_current_time();
	if (remaining <= 0)
		return (0);
	clock_gettime(CLOCK_REALTIME, &timeout);
	timeout.tv_sec += remaining / 1000;
	timeout.tv_nsec += (remaining % 1000) * 1000000;
	if (timeout.tv_nsec >= 1000000000)
	{
		timeout.tv_sec++;
		timeout.tv_nsec -= 1000000000;
	}
	return (pthread_cond_timedwait(&sim->scheduler.condition,
			&sim->scheduler.mutex, &timeout));
}

int	grant_dongles(t_simulation *sim, int coder_id)
{
    int         left;
    int         right;
    int         count;
    t_request   request;

    coder->last_compile_start = get_current_time();
    count = sim->config.number_of_coders;
    left = coder_id - 1;
    right = coder_id % count;
    if(heap_pop(sim, &request) != 0)
        return (0);
    sim->dongles[left].in_use = 1;
    if(count > 1)
        sim->dongles[right].in_use =1;
    pthread_mutex_lock(&sim->dongles[left].mutex);
    if(count > 1)
        pthread_mutex_lock(&sim->dongles[right].mutex);
    return (1);
}

void    *monitor_routine(void *arg)
{
    t_simulation *sim;
    int           i;
    int           dead_coder;
    long long     now;

    sim = (t_simulation *)arg;
    while (1)
    {
        dead_coder = 0;
        pthread_mutex_lock(&sim->scheduler.mutex)
        if(sim->stop)
        {
            pthread_mutex_unlock(&sim->scheduler.mutex);
            break;
        }
        now = get_current_time();
        i = 0;
        while (i < sim->config.number_of_coders)
        {
            if(sim->coders[i].state == FINISHED
                && now - sim->coders[i].last_compile_start
                >= sim->config.time_to_burnout)
                {
                    dead_coder = sim->coders[i].id;
                    sim->stop = 1;
                    break;
                }
                i++;
        }
        if (dead_coder != 0)
            pthread_cond_broadcast(&sim->scheduler.condition);
        pthread_mutex_unlock(&sim->scheduler.mutex);
        if (dead_coder != 0)
        {
            printf("Coder %d burned out\n", dead_coder);
            break ;
        }
        usleep(1000);
    }
    return (NULL);
}