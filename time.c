/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: radakrou <radakrou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 18:58:45 by radakrou          #+#    #+#             */
/*   Updated: 2026/11/11 17:23:40 by radakrou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long get_current_time(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return ((long long)tv.tv_sec * 1000
        + (long long)tv.tv_usec / 1000);
}

void    sleep_ms(int duration)
{
    usleep(duration * 1000);
}

int sleep_interrupt(t_simulation *sim, int duration)
{
    long long start;

    start = get_current_time();

    while(get_current_time - start < duration)
    {
        pthread_mutex_lock(&sim->scheduler.mutex);
        if(sim->stop)
        {
            pthread_mutex_unlock(&sim->scheduler.mutex);
            return (0);
        }
        pthread_mutex_unlock(&sim->scheduler.mutex);
        usleep(1000);
    }
    return (1);
}