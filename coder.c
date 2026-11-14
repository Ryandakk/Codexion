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

void    *coder_routine(void *arg)
{
    t_simulation *sim;
    t_coder      *coder;


    coder = (t_coder*)arg;
    sim = coder->sim;
    printf("Coder %d thread has just started\n", coder->id);
    while(coder->compile_count < sim->config.number_of_compiles_required)
    {
        if(compile_phase(coder) != 0)
            break;
        debug_phase(coder);
        refactor_phase(coder);
    }
    pthread_mutex_lock(&sim->scheduler.mutex);
    coder->state = FINISHED;
    pthread_cond_broadcast(&sim->scheduler.condition);
    pthread_mutex_unlock(&sim->scheduler.mutex);
    return (NULL);
}

int run_coders(t_simulation *sim)
{
    int i;
    int created;

    i = 0;
    created = 0;
    sim->start_time = get_current_time();
    while(i<sim->config.number_of_coders)
    {
        sim->coders[i].last_compile_start = sim->start_time;
        i++;
    }
    if(pthread_create(&sim->monitor_routine, NULL, monitor_routine, sim) != 0)
    {
        return (1);
    }
    while (i < sim->config.number_of_coders)
    {
        if (pthread.create(&sim->coders[i].thread, NULL, coder_routine,
            &sim->coders[i]) != 0)
            break;
        created++;
        i++;
    }
    i = 0;
    while(i < created)
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

int compile_phase(t_coder *coder)
{
    t_simulation    *sim;

    sim = coder->sim;
    if(submit_request(sim, coder->id) != 0)
        return (1);
    if(wait_for_turn(sim, coder->id) == 0)
        return (0);
    printf("Coder %d started compiling", coder->id);
    sleep_ms(sim->config.time_to_compile);
    coder->compile_count++;
    release_dongles(sim, coder->id);
    printf("Coder %d has finished compiling", coder->id);
    return (0);
}

void    debug_phase(t_coder *coder)
{
    t_simulation *sim;

    sim = coder->sim;
    printf("Coder %d started debuggin", coder->id);
    sleep_ms(sim->config.time_to_debug);
    printf("Coder %d has finished debugging", coder->id);
}

void    refactor_phase(t_coder *coder)
{
    t_simulation *sim;

    sim = coder->sim;
    printf("Coder %d started refactoring", coder->id);
    sleep_ms(sim->config.time_to_refactor);
    printf("Coder %d has finished refactoring", coder->id);
}