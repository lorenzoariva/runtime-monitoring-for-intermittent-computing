/*
 * monitor.h
 *
 *  Created on: 1 ago 2022
 *      Author: lorenzo
 */

#ifndef MONITOR_MONITOR_H_
#define MONITOR_MONITOR_H_

#include "../tiny-ink/ink.h"

#define MONITOR_REP_THRESHOLD 5

#define TASKSTARTED 1
#define TASKENDED 0
#define MAXERROR 3

typedef enum { FALSE = 0, TRUE = 1 } boolean;
typedef enum { TASKENDING = 0, TASKSTARTING = 1, TASKSTOPPED = 10} progress_t;
typedef enum { ERROR_PATH, ERROR_REP_T, ERROR_REP_M, NO_ERROR } error_type;

// STRUCTS ------------------------------------------------------------------------
typedef enum {
    CONTROL_ENTRY,
    CONTROL_INTERRUPTED,
    CONTROL_STOPPED
} state_c;

typedef enum {
    MONITOR_STOPPED,
    MONITOR_READY,
    MONITOR_STARTED,
    MONITOR_FINISHED,
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
    GET_TIME,
    FUNCTION_ENDED
} state_f;

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
    void *task_bck;
    volatile progress_t progress;
    int count_rep;
    int _count_rep;
    int monitor_rep;
    volatile boolean monitor_is_rep;
    volatile state_f function_state;
    volatile error_type monitor_error_type;
    volatile state_c control_state;
}monitor_t;

typedef struct
{
    state_m state;
    void (*func_v)(void);
    void (*func_p)(void*, progress_t);
} state_machine_m;

typedef struct
{
    state_f state;
    void (*func)(void);
} state_machine_f;

// FUNCTIONS --------------------------------------------------------------
//tmp getTime function
long int getTime();

void stMonitorError();
boolean checkMonitorThresholdRep();
void reboot_monitor();
void boot_init_monitor(int num_st, int num_tr, void** graph, int repeat_threshold, long int time_threshold);
void init_count();
boolean checkThresholdRep();
void incrementCountTask();
void incrementIndexProgression();
void checkCorrectProgression();
void updateLastTask();
void checkRepetition();
void updateTime();
void backupMonitor();
void stMonitorFinished();
void stMonitorStarted();
void stMonitorReady();
void stMonitorStopped(void *task, progress_t progress);
void monitor_entry(void *task, progress_t progress);

#endif /* MONITOR_MONITOR_H_ */
