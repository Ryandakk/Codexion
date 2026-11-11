/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: radakrou <radakrou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/11 17:03:47 by radakrou          #+#    #+#             */
/*   Updated: 2026/11/11 18:50:06 by radakrou         ###   ########.fr       */
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
    return (NULL);
}

int compile_phase(t_coder *coder)
{
    t_simulation    *sim;

    sim = coder->sim;
    if(submit_request(sim, coder->id) != 0)
        return (1);
    if(wait_for_turn(sim, coder->id) == 0)
        return (0);
    coder->last_compile_start = get_current_time();
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