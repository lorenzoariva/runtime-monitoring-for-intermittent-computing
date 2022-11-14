#ifndef MONITOR_MONITOR_H_
#define MONITOR_MONITOR_H_

#include "../tiny-ink/ink.h"

/* monitor repetition threshold is strict definied into the monitor */
#define MONITOR_REP_THRESHOLD 2
#define MAXERROR 4

typedef enum { FALSE = 0, TRUE = 1 } boolean;

/* used to identify where in thread.c code is the monitor call made. */
typedef enum { TASKENDING, TASKSTARTING, TASKENDED, TASKSTOPPED} progress_t;
/* different errors possible and recognised by the monitor */
typedef enum { ERROR_PATH, ERROR_REP_M, ERROR_REP_T, ERROR_TIME_T, NO_ERROR } error_type;

/* Used by the error functions state machine */
typedef enum {
    NEW_ERROR,
    SKIP,
    RESTART,
    RESTART_BCK,
    ERROR_BCK,
    EXIT,
    DECISION_STOPPED
} error_decision;

/* Used to understand if is a normal monitor call or made by the reboot of the monitor or if the monitor is not used */
typedef enum {
    CONTROL_ENTRY,
    CONTROL_INTERRUPTED,
    CONTROL_STOPPED
} state_c;

/* Used by the monitor state machine */
typedef enum {
    MONITOR_STOPPED,
    MONITOR_STARTED,
    MONITOR_FINISHED,
    MONITOR_ERROR
} state_m;

/* Used by the functions state machine */
typedef enum {
    CHECK_REP,
    INIT_COUNT,
    COUNT_TASK,
    CHECK_PROGRESS,
    UPDATE_LTASK,
    COUNT_PROGRESS,
    MONITOR_BACKUP,
    MONITOR_TIME,
    FUNCTION_ENDED
} state_f;

// STRUCTS ------------------------------------------------------------------------
/**
 * @brief Used by monitor state machine:
 * 1. Monitor state.
 * 2. Function with no parameters called based on the monitor state (not used only in case of MONITOR_STOPPED state).
 * 3. Function with a void pointer, a progress_t value, a thread_t pointer called based on the monitor state (used only in case of MONITOR_STOPPED state).
 */
typedef struct
{
    state_m state;
    void (*func_v)(void);
    void (*func_p)(void*, progress_t, thread_t*);
} state_machine_m;

/**
 * @brief Used by functions state machine and error functions state machine.
 * 1. Function state.
 * 2. Function with no parameters called based on the function state.
 */
typedef struct
{
    state_f state;
    void (*func)(void);
} state_machine_f;

/**
 * @brief Used by error state machine.
 * 1. Error type.
 * 2. Decision to take based on the error type.
 * 3. Next index for SKIP and RESTART decisions, error number for EXIT decision.
 */
typedef struct
{
    error_type error;
    error_decision decision;
    int nextI;
} state_machine_e;

/**
 * @brief keeps decision in case of error, next index/error number, time, repeat and restart threshold for a certain task (pointer).
 * 1. pointer to the task.
 * 2. decision to take in case of error with the specified task (1).
 * 3. next index/error number.
 * 4. time threshold for the specified task (1).
 * 5. repeat threshold for the specified task (1).
 * 6. restart threshold for the specified task (1).
 */
typedef struct
{
    void* pointer;
    error_decision decision;
    int nextI;
    long int time_threshold;
    int repeat_threshold;
    int restart_threshold;
} state_machine_decision;

/**
 * @brief Used to call the function related to a specific decision.
 * 1. Decision value.
 * 2. Function with no parameters called based on the decision value.
 */
typedef struct
{
    error_decision decision;
    void (*func)(void);
} decisionToFunc;

/**
 * @brief Main structure of the monitor.
 * These are all the variables needed by the monitor and stored in non-volatile memory memory.
 */
typedef struct {       
    volatile state_m state;                 /* current/last monitor state */
    int num_tr;                             /* number of transitions in the graph */
    void** graph;                           /* graph of the application */
    long int *transactions;                 /* structure containing the esecution time of the tasks */
    int index;                              /* index to the current point in the graph */
    int _index;                             /* backup of index */
    long int time_threshold;                /* time threshold for the application */
    long int execution_time;                /* execution time of the application */
    void *last_task_s;                      /* last new task encountered by the monitor in thread.c in TASK_READY state */
    void *_last_task_s;                     /* backup of last_task_s */
    void *last_task_e;                      /* last new task encountered by the monitor in thread.c in TASK_FINISHED state */
    void *_last_task_e;                     /* backup of last_task_e */
    void *current_task;                     /* current task at the monitor_entry function call moment */
    volatile progress_t progress;           /* thread.c state at the monitor_entry function call moment */
    int count_rep;                          /* count of the current task repetitions */
    int _count_rep;                         /* backup of count_rep */
    int monitor_rep;                        /* count of the monitor repetitions */
    volatile boolean monitor_is_rep;        /* TRUE: monitor repeat itself */
    volatile state_f function_state;        /* current/last function state during the monitor execution */
    volatile error_type monitor_error_type; /* type of error in case of error */
    volatile state_c control_state;         /* where the control of the monitor started */
    int num_task;                           /* number of different tasks in the graph */
    state_machine_decision* decision;       /* structure with errors decisions and thresholds for each different task in the graph */
    int* restart;                           /* keep the restarts count for each different task in the graph */
    int* _restart;                          /* backup of restart */
    int taskIndex;                          /* index (number) of the current task */
    thread_t *current_thread;               /* current thread at monitor_entry call moment */
    error_decision decision_state;          /* current/last error function state during the MONITOR_ERROR state execution */
    int index_tmp;                          /* temporary index used during MONITOR_ERROR state execution*/
}monitor_t;


// FUNCTIONS --------------------------------------------------------------

int take_nextI();
void errorStBCK();
void errorRestartStBCK();
void errorRestartSt();
void errorSkipSt();
void errorExitSt();
void findCurrI();
void errorStoppedSt();
long int getTime();
void stMonitorError();
boolean checkMonitorThresholdRep();
void reboot_monitor();
void boot_init_monitor(int num_tr, void** graph, long int time_threshold, int num_task, state_machine_decision* decision);
void init_count();
boolean checkThresholdRep();
void incrementCountTask();
void incrementIndexProgression();
void checkCorrectProgression();
void updateLastTask();
void checkRepetition();
boolean checkTime();
void timeFunctionSt();
void backupMonitor();
void stMonitorFinished();
void stMonitorStarted();
void stMonitorStopped(void *task, progress_t progress, thread_t *thread);
void monitor_entry(void *task, progress_t progress, thread_t *thread);

#endif /* MONITOR_MONITOR_H_ */
