
#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <time.h>
# include <errno.h>

# define COMPILING 1
# define DEBUGGING 2
# define REFACTORING 3
# define FINISHED 4

typedef struct s_config
{
	int		number_of_coders;
	int		time_to_burnout;
	int		time_to_compile;
	int		time_to_debug;
	int		time_to_refactor;
	int		number_of_compiles_required;
	int		dongle_cooldown;
	char	*scheduler;
}	t_config;


typedef struct s_coder
{
	int		    id;
	int			compile_count;
	int			state;
	pthread_t	thread;
    struct  s_simulation *sim;
	long long	last_compile_start;
}	t_coder;

typedef struct s_dongle
{
    int             id;
    int             in_use;
    pthread_mutex_t mutex;
    long long       last_release_time;
    long long       available_at;
}   t_dongle;

typedef struct s_request
{
    int         coder_id;
    long long   deadline;
    long long   order;
}   t_request;

typedef struct s_scheduler
{
    t_request       *heap;
    int             size;
    int             capacity;
    long long       next_order;
    pthread_mutex_t mutex;
    pthread_cond_t  condition;
}   t_scheduler;

typedef struct s_simulation
{
    t_config    config;
    t_coder    *coders;
    t_dongle   *dongles;
    t_scheduler scheduler;
    pthread_t   monitor_thread;
    pthread_mutex_t   log_mutex;
    pthread_mutex_t   stop_mutex;
    long long   start_time;
    int         stop;
    int initialized_dongles;
    int scheduler_mutex_initialized;
    int initialized_conditions;
    int initialized_log_mutexes;
    int initialized_stop_mutexes;
}   t_simulation;

int         run_coder(t_simulation *sim);
void        refactor_phase(t_coder *coder);
void        debug_phase(t_coder *coder);
int         compile_phase(t_coder *coder);
int	        wait_for_turn(t_simulation *sim, int coder_id);
void        sleep_ms(int duration);
long long   get_ready_time(t_simulation *sim, int left, int right, int count);
void        *coder_routine(void *arg);
int         dongles_in_use(t_simulation *sim, int left, int right, int count);
int	        wait_for_cooldown(t_simulation *sim, long long ready_at);
int	        grant_dongles(t_simulation *sim, int coder_id);
int	        are_dongles_available(t_simulation *sim, int coder_id);
int	        aquire_dongles(t_simulation *sim, int coder_id);
int	        release_dongles(t_simulation *sim, int coder_id);
long long   get_current_time(void);
int         submit_request(t_simulation *sim, int coder_id);
void        heapify_down(t_simulation *sim, int index);
int         heap_pop(t_simulation *sim, t_request *request);
int 	    heap_insert(t_simulation *sim, t_request request);
void	    heapify_up(t_simulation *sim, int index);
int         has_higher_priority(t_simulation *sim, t_request a, t_request b);
int         init_mutexes(t_simulation *sim);
int         init_scheduler(t_simulation *sim);
int         init_dongles(t_simulation *sim);
int         init_simulation(t_simulation *sim, t_config *config);
int         init_coders(t_simulation *sim);
int		    is_valid_number(char *str);
int		    ft_atoi(char *str);
int		    is_valid_scheduler(char *str);
void        cleanup_simulation(t_simulation *sim);
void	    init_config(t_config *config, char **argv);

#endif
