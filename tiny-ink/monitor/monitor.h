/*
 * monitor.h
 *
 *  Created on: 1 ago 2022
 *      Author: lorenzo
 */

#ifndef MONITOR_MONITOR_H_
#define MONITOR_MONITOR_H_

#include "../tiny-ink/ink.h"

#define MONITOR_REP_THRESHOLD 3
#define MAXERROR 4

typedef enum { FALSE = 0, TRUE = 1 } boolean;
typedef enum { TASKENDING, TASKSTARTING, TASKSTOPPED, TASKENDED} progress_t;
typedef enum { ERROR_PATH, ERROR_REP_M, ERROR_REP_T, ERROR_TIME_T, NO_ERROR } error_type;

typedef enum {
    NEW_ERROR,
    SKIP,
    RESTART,
    RESTART_BCK,
    ERROR_BCK,
    EXIT,
    DECISION_STOPPED
} error_decision;

typedef enum {
    CONTROL_ENTRY,
    CONTROL_INTERRUPTED,
    CONTROL_STOPPED
} state_c;

typedef enum {
    MONITOR_STOPPED,
    MONITOR_STARTED,
    MONITOR_FINISHED,
    MONITOR_TIME,
    MONITOR_ERROR
} state_m;

typedef enum {
    CHECK_REP,
    INIT_COUNT,
    COUNT_TASK,
    UPDATE_LTASK,
    CHECK_PROGRESS,
    COUNT_PROGRESS,
    MONITOR_BACKUP,
    FUNCTION_ENDED
} state_f;

// STRUCTS ------------------------------------------------------------------------
typedef struct
{
    state_m state;
    void (*func_v)(void);
    void (*func_p)(void*, progress_t, thread_t*);
} state_machine_m;

typedef struct
{
    state_f state;
    void (*func)(void);
} state_machine_f;

//monitor v4
typedef struct
{
    error_type error;
    error_decision decision;
    int nextI;
} state_machine_e;

//monitor v4
typedef struct
{
    void* pointer;
    error_decision decision;
    int nextI;
    long int time;
} state_machine_decision;

typedef struct
{
    error_decision decision;
    void (*func)(void);
} decisionToFunc;


typedef struct {       
    volatile state_m state;
    int num_tr;
    void** graph;
    long int *transactions;
    int index;
    int _index;
    int repeat_threshold;
    long int time_threshold;
    long int execution_time;
    void *last_task_s;
    void *_last_task_s;
    void *last_task_e;
    void *_last_task_e;
    void *current_task;
    volatile progress_t progress;
    int count_rep;
    int _count_rep;
    int monitor_rep;
    volatile boolean monitor_is_rep;
    volatile state_f function_state;
    volatile error_type monitor_error_type;
    volatile state_c control_state;

    //monitor v4
    int num_task;
    state_machine_decision* decision;
    int* restart;
    int* _restart;
    int restart_threshold;
    int taskIndex;
    thread_t *current_thread;
    error_decision decision_state; 
    int index_tmp;
}monitor_t;


// FUNCTIONS --------------------------------------------------------------

//monitor v4
int take_nextI();
void errorStBCK();
void errorRestartStBCK();
void errorRestartSt();
void errorSkipSt();
void errorExitSt();
void findCurrI();
void errorStoppedSt();

//tmp getTime function
long int getTime();

void stMonitorError();
boolean checkMonitorThresholdRep();
void reboot_monitor();
void boot_init_monitor(int num_st, int num_tr, void** graph, int repeat_threshold, long int time_threshold, int num_task, state_machine_decision* decision, int restart_threshold);
void init_count();
boolean checkThresholdRep();
void incrementCountTask();
void incrementIndexProgression();
void checkCorrectProgression();
void updateLastTask();
void checkRepetition();
boolean checkTime();
void stMonitorTime();
void backupMonitor();
void stMonitorFinished();
void stMonitorStarted();
void stMonitorStopped(void *task, progress_t progress, thread_t *thread);
void monitor_entry(void *task, progress_t progress, thread_t *thread);

#endif /* MONITOR_MONITOR_H_ */
