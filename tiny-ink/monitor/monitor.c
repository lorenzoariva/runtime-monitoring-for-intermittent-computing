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
    "[ERROR_REP_M] Too many repetition of the monitor for the same task!",
    "[ERROR_REP_T] Too many repetitions of the same task!",
    "[ERROR_TIME_T] Task limit execution time exceeded!"
};

// STATE MACHINES ---------------------------------------------------------

state_machine_m monitorStateMachine[] = {
    { MONITOR_STOPPED, NULL, stMonitorStopped },
    { MONITOR_STARTED, stMonitorStarted, NULL },
    { MONITOR_FINISHED, stMonitorFinished, NULL },
    { MONITOR_TIME, stMonitorTime, NULL},
    { MONITOR_ERROR, stMonitorError, NULL }
};



state_machine_f functionStateMachine[] = {
    { CHECK_REP, checkRepetition },
    { INIT_COUNT, init_count },
    { COUNT_TASK, incrementCountTask },
    { UPDATE_LTASK, updateLastTask },
    { CHECK_PROGRESS, checkCorrectProgression },
    { COUNT_PROGRESS, incrementIndexProgression },
    { MONITOR_BACKUP, backupMonitor }
};

//monitor v4 ----------------------------
decisionToFunc decision_func[] = {
    { NEW_ERROR, errorStoppedSt },
    { SKIP, errorSkipSt },
    { RESTART, errorRestartSt },
    { RESTART_BCK, errorRestartStBCK },
    { ERROR_BCK, errorStBCK },
    { EXIT, errorExitSt }
};


state_machine_e errorStateMachine[] = {
    { ERROR_PATH, EXIT, 6 },
    { ERROR_REP_M, EXIT, 5 }
};
// -----------------------------------------------------------------------
int take_nextI(){
    int error_nextI = 0;
    if (monitor->monitor_error_type == ERROR_REP_T || monitor->monitor_error_type == ERROR_TIME_T) {
        error_nextI = (*(monitor->decision + monitor->taskIndex)).nextI;
    } else {
        error_nextI = errorStateMachine[monitor->monitor_error_type].nextI;
    }

    return error_nextI;
}

void errorStBCK(){

    error_decision dec_tmp;

    if(monitor->monitor_error_type == ERROR_REP_T || monitor->monitor_error_type == ERROR_TIME_T){
        dec_tmp = (monitor->decision + monitor->taskIndex)->decision;
    } else {
        dec_tmp = errorStateMachine[monitor->monitor_error_type].decision;
    }

    if(dec_tmp == SKIP){
        *(monitor->transactions + monitor->index_tmp) = -1;
    } else {
        *(monitor->transactions + monitor->index_tmp) = 0;
    }

    monitor->execution_time = 0;
    monitor->last_task_s = NULL;
    monitor->_last_task_s = NULL;
    monitor->last_task_e = NULL;
    monitor->_last_task_e = NULL;
    monitor->current_task = NULL;
    monitor->count_rep = 0;
    monitor->_count_rep = 0;
    monitor->monitor_rep = 0;

    monitor->index = monitor->_index;
    (monitor->current_thread)->next = *(monitor->graph + monitor->index);
    (monitor->current_thread)->_next = *(monitor->graph + monitor->index);
    monitor->decision_state = DECISION_STOPPED;
}

void errorRestartStBCK(){
    *(monitor->restart + monitor->taskIndex) = *(monitor->_restart + monitor->taskIndex);
    monitor->decision_state = ERROR_BCK;
}

void errorRestartSt(){
    int error_nextI = take_nextI();
    
    void* error_nextT = *(monitor->graph + error_nextI);

    if(*(monitor->restart + monitor->taskIndex) < monitor->repeat_threshold){
        //fprintf(stdout, "[RESTART][%d]%p ==> [%d]%p\n", monitor->index_tmp, monitor->current_task, error_nextI, error_nextT);
        //fflush(stdout);
        monitor->_index = error_nextI;
        *(monitor->_restart + monitor->taskIndex) = *(monitor->restart + monitor->taskIndex) + 1;
        monitor->decision_state = RESTART_BCK;
    } else {
        //fprintf(stdout, "[RESTART_FAIL][%d]%p ==> [%d]%p\n", monitor->index_tmp, monitor->current_task, error_nextI, error_nextT);
        //fflush(stdout);
        exit(20);
    }

}

void errorSkipSt(){
    int error_nextI = take_nextI();

    void* error_nextT = *(monitor->graph + error_nextI);

    //fprintf(stdout, "[SKIP][%d]%p ==> [%d]%p\n", monitor->index_tmp, monitor->current_task, error_nextI, error_nextT);
    //fflush(stdout);

    monitor->_index = error_nextI;
    monitor->decision_state = ERROR_BCK;
}
void errorExitSt()
{
    int err = take_nextI();

    fprintf(stdout, "[EXIT][%d]%p ==> %d\n", monitor->index_tmp, monitor->current_task, err);
    fflush(stdout);

    exit(err);

}


void findCurrI(){
    boolean find = FALSE;
    int i=0;
    while(!find && (i<monitor->num_task)){
        if((monitor->decision + i)->pointer == monitor->current_task){
            find = TRUE;
            monitor->taskIndex = i;
        } else {
            i++;
        }
    }
}


void errorStoppedSt(){
    
    if(monitor->monitor_error_type == ERROR_PATH || monitor->monitor_error_type == ERROR_REP_M ){
        monitor->decision_state = errorStateMachine[monitor->monitor_error_type].decision;
    } else if(monitor->monitor_error_type == ERROR_REP_T){
        findCurrI();
        monitor->decision_state = (monitor->decision + monitor->taskIndex)->decision;
    } else if(monitor->monitor_error_type == ERROR_TIME_T){
        monitor->decision_state = (monitor->decision + monitor->taskIndex)->decision;
    } else {
        //printf("[INVALID_ERROR] The error type is invalid!\n");
        //fflush(stdout);
        exit(20);
    }

    monitor->index_tmp = monitor->index;
    if(monitor->progress == TASKENDING || monitor->progress == TASKENDED){
        monitor->index_tmp = monitor->index_tmp - 1;
    }

}

//------------------------------ monitor v4

void stMonitorError(){
    //printf("%s\n", error_string[monitor->monitor_error_type]);
    //fflush(stdout);
    
    
    while(monitor->decision_state != DECISION_STOPPED){
        (*decision_func[monitor->decision_state].func)();
    }
    
    (monitor->current_thread)->state = TASK_READY;
    monitor->function_state = FUNCTION_ENDED;
    monitor->state = MONITOR_STOPPED;
}

boolean checkMonitorThresholdRep(){
    return (monitor->monitor_rep < MONITOR_REP_THRESHOLD);
}
//to be called at each boot
void reboot_monitor(){

    if(monitor->state == MONITOR_STARTED || monitor->state == MONITOR_FINISHED || monitor->state == MONITOR_TIME || monitor->state == MONITOR_ERROR){
        monitor->control_state = CONTROL_INTERRUPTED;
        monitor->monitor_rep = (monitor->monitor_rep)+1;
        if(!checkMonitorThresholdRep()){
            monitor->monitor_error_type = ERROR_REP_M;
            monitor->decision_state = NEW_ERROR;
            monitor->state = MONITOR_ERROR;
        } else {
            monitor->monitor_is_rep = TRUE;
        }
        monitor_entry(monitor->current_task, monitor->progress, monitor->current_thread);
    }
}

// to be called only at first boot
void boot_init_monitor(int num_st, int num_tr, void** graph, int repeat_threshold, long int time_threshold, int num_task, state_machine_decision* decision, int restart_threshold){
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
    monitor->repeat_threshold = repeat_threshold;
    monitor->time_threshold = time_threshold;
    monitor->execution_time = 0;
    monitor->last_task_s = NULL;
    monitor->_last_task_s = NULL;
    monitor->last_task_e = NULL;
    monitor->_last_task_e = NULL;
    monitor->current_task = NULL;
    monitor->count_rep = 0;
    monitor->_count_rep = 0;
    monitor->progress = TASKSTOPPED;
    monitor->monitor_rep = 0;
    monitor->monitor_is_rep = FALSE;
    monitor->function_state = FUNCTION_ENDED;
    monitor->monitor_error_type = NO_ERROR;
    monitor->control_state = CONTROL_STOPPED;

    //monitor v4
    monitor->num_task = num_task;
    monitor->decision = (state_machine_decision*) malloc (num_task * sizeof(state_machine_decision));
    monitor->restart_threshold = restart_threshold;
    monitor->restart = (int*) malloc (num_task * sizeof(int));
    monitor->_restart = (int*) malloc (num_task * sizeof(int));
    for(int i = 0; i<num_task; i++){
        *(monitor->decision + i) = *(decision + i);
        *(monitor->restart + i) = 0;
        *(monitor->_restart + i) = 0;
    }
    monitor->taskIndex = 0;
    monitor->current_thread = NULL;
    monitor->decision_state = DECISION_STOPPED;
    monitor->index_tmp = 0;
}


void init_count(){
    monitor->count_rep = 0;
    monitor->_count_rep = 0;
    monitor->function_state = UPDATE_LTASK;
}

boolean checkThresholdRep(){
    return (monitor->_count_rep < monitor->repeat_threshold);
}

void incrementCountTask(){
    monitor->_count_rep = monitor->count_rep + 1;
    if(!checkThresholdRep()){
        monitor->monitor_error_type = ERROR_REP_T;
        monitor->decision_state = NEW_ERROR;
        monitor->state = MONITOR_ERROR;
    }
    monitor->function_state = FUNCTION_ENDED;
}

void incrementIndexProgression(){
    if(monitor->progress == TASKENDING){
        monitor->_index = monitor->index + 1;
    }
    monitor->function_state = FUNCTION_ENDED;
}

void checkCorrectProgression(){
    if(monitor->current_task == *((monitor->graph)+monitor->index)){
        monitor->function_state = COUNT_PROGRESS;
    } else {
        monitor->monitor_error_type = ERROR_PATH;
        monitor->decision_state = NEW_ERROR;
        monitor->state = MONITOR_ERROR;
        monitor->function_state = FUNCTION_ENDED;
    }
}

void updateLastTask(){
    if(monitor->progress == TASKSTARTING){
        monitor->_last_task_s = monitor->current_task;
    } else if (monitor->progress == TASKENDING){
        monitor->_last_task_e = monitor->current_task;
    }
    monitor->function_state = CHECK_PROGRESS;
}

void checkRepetition(){
    boolean res = FALSE;
    if(monitor->progress == TASKSTARTING){
        res = (monitor->last_task_s == monitor->current_task);
    } else if (monitor->progress == TASKENDING){
        res = (monitor->last_task_e == monitor->current_task);
    }
    if(res){
        monitor->function_state = COUNT_TASK;
    } else {
        monitor->function_state = INIT_COUNT;
    }
}

boolean checkTime(){
    findCurrI();
    return (*(monitor->transactions + (monitor->index-1)) > (monitor->decision + monitor->taskIndex)->time);
}

// tmp getTime() function
long int getTime(){
    return (100 * (monitor->index +1));
}

//void updateTime(){
void stMonitorTime(){
    int index_tmp = monitor->index -1;
    if(*(monitor->transactions + index_tmp) == 0){
        if(index_tmp == 0){
            *(monitor->transactions + index_tmp) = getTime();
        } else {
            *(monitor->transactions + index_tmp) = getTime()- *(monitor->transactions + (index_tmp -1));
        }
    }
    if(checkTime()){
        monitor->monitor_error_type = ERROR_TIME_T;
        monitor->decision_state = NEW_ERROR;
        monitor->state = MONITOR_ERROR;
    } else {
        if(monitor->index == monitor->num_tr && monitor->execution_time == 0){
            monitor->execution_time = getTime();
        }
        if(monitor->execution_time != 0){
            fprintf(stdout, "[SUCCESS][%ld] End reached!\n", monitor->execution_time);
            if(monitor->execution_time > monitor->time_threshold){
                fprintf(stdout, "[TIME_EXCEEDED] [%ld]>[%ld] The application take to much time!\n", monitor->execution_time, monitor->time_threshold);
            }
            fflush(stdout);
        }
        monitor->state = MONITOR_STOPPED;
    }
}

void backupMonitor(){
    if(monitor->progress == TASKSTARTING){
        monitor->last_task_s = monitor->_last_task_s;
    } else if (monitor->progress == TASKENDING){
        monitor->last_task_e = monitor->_last_task_e;
    }
    monitor->count_rep = monitor->_count_rep;
    monitor->index = monitor->_index;
    monitor->function_state = FUNCTION_ENDED;
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

    if(monitor->state != MONITOR_ERROR){
        monitor->function_state = MONITOR_BACKUP;
        monitor->state = MONITOR_FINISHED;
    }
}


void stMonitorStopped(void *task, progress_t progress, thread_t *thread){
    monitor->monitor_rep = 0;
    monitor->current_task = task;
    monitor->progress = progress;
    monitor->current_thread = thread;

    if(progress == TASKENDED){
        monitor->state = MONITOR_TIME;
    } else {
        monitor->function_state = CHECK_REP;
        monitor->state = MONITOR_STARTED;
    }
}

// when you are here you have 2 possibilities:
// 1. MONITOR_STOPPED 2. MONITOR_READY
void monitor_entry(void *task, progress_t progress, thread_t *thread){
    if(!monitor->monitor_is_rep){
        monitor->control_state = CONTROL_ENTRY;
        //If I'm here I'm sure to be MONITOR_STOPPED and monitor_is_rep == FALSE
        (*monitorStateMachine[monitor->state].func_p)(task, progress, thread);
        
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
