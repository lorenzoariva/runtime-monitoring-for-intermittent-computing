/*
 * monitor.c
 *
 *  Created on: 1 ago 2022
 *      Author: lorenzo
 */

#include "monitor.h"
#include <stdio.h>

__nv monitor_t monitor_fram;
monitor_t* monitor = &monitor_fram;

// VARIABLES
char* error_string[MAXERROR] = {
    "[ERROR_PATH] Not following the correct path!",
    "[ERROR_REP_T] Too many repetitions of the same task!",
    "[ERROR_REP_M] Too many repetition of the monitor for the same task!"
};

// STATE MACHINES ---------------------------------------------------------

state_machine_m monitorStateMachine[] = {
    { MONITOR_STOPPED, NULL, stMonitorStopped },
    { MONITOR_READY, stMonitorReady, NULL },
    { MONITOR_STARTED, stMonitorStarted, NULL },
    { MONITOR_FINISHED, stMonitorFinished, NULL },
    { MONITOR_ERROR, stMonitorError, NULL }
};



state_machine_f functionStateMachine[] = {
    { CHECK_REP_R, checkRepetition },
    { CHECK_REP_S, checkRepetition },
    { INIT_COUNT, init_count },
    { COUNT_TASK, incrementCountTask },
    { UPDATE_LTASK, updateLastTask },
    { CHECK_PROGRESS, checkCorrectProgression },
    { COUNT_PROGRESS, incrementIndexProgression },
    { MONITOR_BACKUP, backupMonitor },
    { GET_TIME, updateTime }
};
// -----------------------------------------------------------------------

void stMonitorError(){
    fprintf(stdout, "%s\n", error_string[monitor->monitor_error_type]);
    fflush(stdout);
    exit(11);
}

boolean checkMonitorThresholdRep(){
    return (monitor->monitor_rep < monitor->repeat_threshold);
}
//to be called at each boot
void reboot_monitor(){

    if(monitor->state == MONITOR_ERROR){
        monitor->control_state = CONTROL_INTERRUPTED;
        (*monitorStateMachine[monitor->state].func_v)();
    } else if(monitor->state == MONITOR_STARTED || monitor->state == MONITOR_FINISHED || monitor->state == MONITOR_READY ){
        monitor->control_state = CONTROL_INTERRUPTED;
        monitor->monitor_rep = (monitor->monitor_rep)+1;
        if(!checkMonitorThresholdRep()){
            monitor->monitor_error_type = ERROR_REP_M;
            monitor->state = MONITOR_ERROR;
            (*monitorStateMachine[monitor->state].func_v)();
        } else {
            monitor->monitor_is_rep = TRUE;
            monitor_entry(monitor->task_bck, monitor->progress);
        }
    }
}

// to be called only at first boot
void boot_init_monitor(int num_st, int num_tr, void** graph, int repeat_threshold, long int time_threshold){
    monitor->state = MONITOR_STOPPED;
    monitor->num_tr = num_tr;
    monitor->graph = (void**) malloc (num_tr * sizeof(void*));
    monitor->transactions = (long int*) malloc (num_tr * sizeof(long int));
    for(int i=0; i<num_tr; i++){
        *(monitor->transactions + i) = 0;
        *(monitor->graph + i) = *(graph + i);
    }
    monitor->index = 0;
    monitor->_index = 0;
    monitor->index_bck = 0;
    monitor->repeat_threshold = repeat_threshold;
    monitor->time_threshold = time_threshold;
    monitor->execution_time = 0;
    monitor->last_task_s = NULL;
    monitor->_last_task_s = NULL;
    monitor->last_task_e = NULL;
    monitor->_last_task_e = NULL;
    monitor->task_bck = NULL;
    monitor->count_rep = 0;
    monitor->_count_rep = 0;
    monitor->progress = TASKSTOPPED;
    monitor->monitor_rep = 0;
    monitor->monitor_is_rep = FALSE;
    monitor->function_state = FUNCTION_ENDED;
    monitor->monitor_error_type = NO_ERROR;
    monitor->control_state = CONTROL_STOPPED;
}


void init_count(){
    monitor->count_rep = 0;
    monitor->_count_rep = 0;
    monitor->function_state = FUNCTION_ENDED;
}

boolean checkThresholdRep(){
    return (monitor->_count_rep < monitor->repeat_threshold);
}

void incrementCountTask(){
    monitor->_count_rep = monitor->count_rep + 1;
    if(!checkThresholdRep()){
        monitor->monitor_error_type = ERROR_REP_T;
        monitor->state = MONITOR_ERROR;
        (*monitorStateMachine[monitor->state].func_v)();
    } else {
        monitor->function_state = FUNCTION_ENDED;
    }
}

void incrementIndexProgression(){
    if(monitor->progress == TASKENDING){
        monitor->_index = monitor->index + 1;
    }
    monitor->function_state = FUNCTION_ENDED;
}

void checkCorrectProgression(){
    if(monitor->task_bck == *((monitor->graph)+monitor->index)){
        monitor->function_state = COUNT_PROGRESS;
    } else {
        monitor->monitor_error_type = ERROR_PATH;
        monitor->state = MONITOR_ERROR;
        (*monitorStateMachine[monitor->state].func_v)();
    }
}

void updateLastTask(){
    if(monitor->progress == TASKSTARTING){
        monitor->_last_task_s = monitor->task_bck;
    } else if (monitor->progress == TASKENDING){
        monitor->_last_task_e = monitor->task_bck;
    }
    monitor->function_state = CHECK_PROGRESS;
}

void checkRepetition(){
    boolean res = FALSE;
    if(monitor->progress == TASKSTARTING){
        res = (monitor->last_task_s == monitor->task_bck);
    } else if (monitor->progress == TASKENDING){
        res = (monitor->last_task_e == monitor->task_bck);
    }
    if(res){
        if(monitor->state == MONITOR_READY){
            monitor->function_state = FUNCTION_ENDED;
        } else if (monitor->state == MONITOR_STARTED){
            monitor->function_state = COUNT_TASK;
        }
    } else {
        if(monitor->state == MONITOR_READY){
            monitor->function_state = INIT_COUNT;
        } else if (monitor->state == MONITOR_STARTED){
            monitor->function_state = UPDATE_LTASK;
        }
    }
}

// tmp getTime() function
long int getTime(){
    return (1000 * (monitor->index_bck +1));
}

void updateTime(){
    if(*(monitor->transactions + monitor->index_bck) == 0){
        if(monitor->index_bck == 0){
            *(monitor->transactions + monitor->index_bck) = getTime();
        } else {
            *(monitor->transactions + monitor->index_bck) = getTime()- *(monitor->transactions + (monitor->index_bck -1));
        }
    }
    if(monitor->_index == monitor->num_tr && monitor->execution_time == 0){
        monitor->execution_time = getTime();
    }
    if(monitor->execution_time != 0){
        fprintf(stdout, "[SUCCESS][%ld] End reached!\n", monitor->execution_time);
    }
    monitor->function_state = FUNCTION_ENDED;
}

void backupMonitor(){
    monitor->count_rep = monitor->_count_rep;
    monitor->index = monitor->_index;

    if(monitor->progress == TASKSTARTING){
        monitor->last_task_s = monitor->_last_task_s;
        monitor->function_state = FUNCTION_ENDED;
    } else if (monitor->progress == TASKENDING){
        monitor->last_task_e = monitor->_last_task_e;
        monitor->function_state = GET_TIME;
    }
    
}

void stMonitorFinished(){
    while(monitor->function_state != FUNCTION_ENDED){
        (*functionStateMachine[monitor->function_state].func)();
    }
    monitor->state = MONITOR_STOPPED;
}

void stMonitorStarted(){
    while(monitor->function_state != FUNCTION_ENDED && monitor->function_state != MONITOR_BACKUP){
        (*functionStateMachine[monitor->function_state].func)();
    }
    monitor->function_state = MONITOR_BACKUP;
    monitor->state = MONITOR_FINISHED;
}

void stMonitorReady(){
    while(monitor->function_state != FUNCTION_ENDED && monitor->function_state != CHECK_REP_S){
        (*functionStateMachine[monitor->function_state].func)();
    }
    monitor->function_state = CHECK_REP_S;
    monitor->state = MONITOR_STARTED;
}


void stMonitorStopped(void *task, progress_t progress){
    monitor->monitor_rep = 0;
    monitor->task_bck = task;
    monitor->index_bck = monitor->index;
    monitor->progress = progress;
    
    monitor->function_state = CHECK_REP_R;
    monitor->state = MONITOR_READY;
}

// when you are here you have 2 possibilities:
// 1. MONITOR_STOPPED 2. MONITOR_READY
void monitor_entry(void *task, progress_t progress){
    if(!monitor->monitor_is_rep){
        monitor->control_state = CONTROL_ENTRY;
        //If I'm here I'm sure to be MONITOR_STOPPED and monitor_is_rep == FALSE
        (*monitorStateMachine[monitor->state].func_p)(task, progress);
        
        while(monitor->state != MONITOR_STOPPED){
            (*monitorStateMachine[monitor->state].func_v)();
        }
    } else if(monitor->state != MONITOR_STOPPED){
        //monitor not STOPPED and is_rep == TRUE
        while(monitor->state != MONITOR_STOPPED){
            (*monitorStateMachine[monitor->state].func_v)();
        }
    } else {
        //monitor STOPPED and is_rep == TRUE
        monitor->monitor_is_rep = FALSE;
    }

    monitor->control_state = CONTROL_STOPPED;
}
