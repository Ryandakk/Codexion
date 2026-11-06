/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: radakrou <radakrou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 16:56:11 by radakrou          #+#    #+#             */
/*   Updated: 2026/11/06 18:40:31 by radakrou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int init_simulation(t_simulation *sim, t_config *config)
{
    sim->config = *config;
    sim->coders = NULL;
    sim->dongles = NULL;
    sim->start_time = 0;
    sim->stop = 0;
    sim->scheduler.heap = NULL;
    sim->initialized_dongles = 0;
    sim->scheduler_mutex_initialized = 0;
    sim->initialized_conditions = 0;
    sim->initialized_log_mutexes = 0;
    sim->initialized_stop_mutexes = 0;
    if(init_coders(sim) != 0)
    {
        cleanup_simulation(sim);
        return (1);
    }
        
    if(init_dongles(sim) != 0)
    {
        cleanup_simulation(sim);
        return (1);
    }
        
    if(init_scheduler(sim) != 0)
    {
     cleanup_simulation(sim);
     return (1);   
    }
    if(init_mutexes(sim) != 0)
    {
        cleanup_simulation(sim);
        return (1);
    }
    return (0);
}

int init_coders(t_simulation *sim)
{
    int i;
    
    sim->coders = malloc(sizeof(t_coder) * sim->config.number_of_coders);
    if (!sim->coders)
        return(1);
    i = 0;
    while(i < sim->config.number_of_coders)
    {
        sim->coders[i].id = i + 1;
        sim->coders[i].compile_count = 0;
        sim->coders[i].state = 0;
        sim->coders[i].last_compile_start = 0;
        i++;
    }
    return(0);
}

int init_dongles(t_simulation *sim)
{
    int i;
    sim->dongles = malloc(sizeof(t_dongle) * (sim->config.number_of_coders));
    if(!sim->dongles)
        return(1);
    i = 0;
    while(i < sim->config.number_of_coders)
    {
        sim->dongles[i].id = i + 1;
        sim->dongles[i].last_release_time = 0;
        sim->dongles[i].available_at = 0;
        if(pthread_mutex_init(&sim->dongles[i].mutex, NULL) != 0)
            return (1);
        i++;
        sim->initialized_dongles++;
    }
    return(0);
}

int init_scheduler(t_simulation *sim)
{
    t_scheduler *scheduler;

    scheduler = &sim->scheduler;
    scheduler->capacity = sim->config.number_of_coders;
    scheduler->size = 0;
    scheduler->next_order = 0;
    scheduler->heap = malloc(sizeof(t_request) * scheduler->capacity);
    if (!scheduler->heap)
        return (1);
    if (pthread_mutex_init(&scheduler->mutex, NULL) != 0)
            return (1);
    sim->scheduler_mutex_initialized = 1;
    if(pthread_cond_init(&scheduler->condition, NULL) != 0)
        return (1);
    sim->initialized_conditions++;
    return (0);
}

int init_mutexes(t_simulation *sim)
{
    if(pthread_mutex_init(&sim->log_mutex, NULL) != 0)
        return (1);
    sim->initialized_log_mutexes++;
    if(pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
        return (1);
    sim->initialized_stop_mutexes++;
    return (0);
}

void cleanup_simulation(t_simulation *sim)
{
    int i;

    i = 0;

    while(i < sim->initialized_dongles)
    {
        pthread_mutex_destroy(&sim->dongles[i].mutex);
        i++;
    }
    if(sim->initialized_conditions > 0)
        pthread_cond_destroy(&sim->scheduler.condition);
    if(sim->scheduler_mutex_initialized == 1)
        pthread_mutex_destroy(&sim->scheduler.mutex);
    if(sim->initialized_log_mutexes > 0)
        pthread_mutex_destroy(&sim->log_mutex);
    if(sim->initialized_stop_mutexes > 0)
        pthread_mutex_destroy(&sim->stop_mutex);
    free(sim->scheduler.heap);
    free(sim->coders);
    free(sim->dongles);
    sim->scheduler.heap = NULL;
    sim->coders = NULL;
    sim->dongles = NULL;
    sim->initialized_dongles = 0;
    sim->scheduler_mutex_initialized = 0;
    sim->initialized_log_mutexes = 0;
    sim->initialized_stop_mutexes = 0;
    sim->initialized_conditions = 0;
    
}