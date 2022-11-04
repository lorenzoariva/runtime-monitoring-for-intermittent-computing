#include "monitor.h"
#include <stdio.h>

/* monitor structure definied in non-volatile memory */
__nv monitor_t monitor_fram;
monitor_t* monitor = &monitor_fram;

// VARIABLES
/**
 * @brief main errors phrases.
 */
char* error_string[MAXERROR] = {
    "[ERROR_PATH] Not following the correct path!",
    "[ERROR_REP_M] Too many repetition of the monitor for the same task!",
    "[ERROR_REP_T] Too many repetitions of the same task!",
    "[ERROR_TIME_T] Task limit execution time exceeded!"
};

// STATE MACHINES ---------------------------------------------------------
/**
 * @brief state machine of the monitor with the main monitor states.
 * See monitor.h for the struct definition.
 */
state_machine_m monitorStateMachine[] = {
    { MONITOR_STOPPED, NULL, stMonitorStopped },
    { MONITOR_STARTED, stMonitorStarted, NULL },
    { MONITOR_FINISHED, stMonitorFinished, NULL },
    { MONITOR_ERROR, stMonitorError, NULL }
};


/**
 * @brief functions state machine for the fuctions executed during the normal execution of the monitor.
 * See monitor.h for the struct definition.
 */
state_machine_f functionStateMachine[] = {
    { CHECK_REP, checkRepetition },
    { INIT_COUNT, init_count },
    { COUNT_TASK, incrementCountTask },
    { UPDATE_LTASK, updateLastTask },
    { CHECK_PROGRESS, checkCorrectProgression },
    { COUNT_PROGRESS, incrementIndexProgression },
    { MONITOR_BACKUP, backupMonitor },
    { MONITOR_TIME, timeFunctionSt }
};

/**
 * @brief error functions state machine for the functions executed in case of errors.
 * See monitor.h for the struct definition.
 */
decisionToFunc decision_func[] = {
    { NEW_ERROR, errorStoppedSt },
    { SKIP, errorSkipSt },
    { RESTART, errorRestartSt },
    { RESTART_BCK, errorRestartStBCK },
    { ERROR_BCK, errorStBCK },
    { EXIT, errorExitSt }
};

/**
 * @brief state machine for the errors managed only by the monitor.
 * See monitor.h for the struct definition.
 */
state_machine_e errorStateMachine[] = {
    { ERROR_PATH, EXIT, 6 },
    { ERROR_REP_M, EXIT, 5 }
};
// -----------------------------------------------------------------------
/**
 * @brief return the value of the next index saved in the decision or errorStateMachine structs in case of errors.
 * The index refers to the next index for the SKIP and RESTART decisions or to the error number in case of EXIT decision.
 * 
 * @return int value of the next index.
 */
int take_nextI(){
    int error_nextI = 0;
    if (monitor->monitor_error_type == ERROR_REP_T || monitor->monitor_error_type == ERROR_TIME_T) {
        error_nextI = (*(monitor->decision + monitor->taskIndex)).nextI;
    } else {
        error_nextI = errorStateMachine[monitor->monitor_error_type].nextI;
    }

    return error_nextI;
}

/**
 * @brief finishes the backup of variables.
 * Initialize the value of certain variables in the monitor struct to avoid unintended future errors.
 * If the decision was SKIP the transaction value for the current task is set to -1 because the current task never ended.
 * In the other cases the value is set to 0.
 */
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

/**
 * @brief backup the restart struct (maintain the continuity in case of power failure) 
 * in the monitor that contains the count of restarts for every different task in the graph.
 * Finishes the backup of variables in the ERROR_BCK.
 */
void errorRestartStBCK(){
    *(monitor->restart + monitor->taskIndex) = *(monitor->_restart + monitor->taskIndex);
    monitor->decision_state = ERROR_BCK;
}

/**
 * @brief RESTART error function state.
 * Called if the decision for the error is RESTART.
 * Check if the restart threshold for the current task is respected.
 * If the threshold is respected:
 *  - prints the decision, the current index, the current task, the next index and the next task.
 *  - updates the index in the monitor and the count of restarts from the current task.
 *  - starts the backup of variables in the RESTART_BCK.
 * If the thredhold is not respected is printed that the restart fails and an exit call is made.
 */
void errorRestartSt(){
    int error_nextI = take_nextI();
    
    void* error_nextT = *(monitor->graph + error_nextI);

    if(*(monitor->restart + monitor->taskIndex) < ((monitor->decision + monitor->taskIndex)->restart_threshold)){
        fprintf(stdout, "[RESTART][%d]%p ==> [%d]%p\n", monitor->index_tmp, monitor->current_task, error_nextI, error_nextT);
        fflush(stdout);
        monitor->_index = error_nextI;
        *(monitor->_restart + monitor->taskIndex) = *(monitor->restart + monitor->taskIndex) + 1;
        monitor->decision_state = RESTART_BCK;
    } else {
        fprintf(stdout, "[RESTART_FAIL][%d]%p ==> [%d]%p\n", monitor->index_tmp, monitor->current_task, error_nextI, error_nextT);
        fflush(stdout);
        exit(20);
    }

}

/**
 * @brief SKIP error function state.
 * Called if the decision for the error is SKIP.
 * Prints the decision, the current index, the current task, the next index and the next task.
 * Updates the index in the monitor.
 * Finishes the backup of the variables in ERROR_BCK error funtion state.
 */
void errorSkipSt(){
    int error_nextI = take_nextI();

    void* error_nextT = *(monitor->graph + error_nextI);

    fprintf(stdout, "[SKIP][%d]%p ==> [%d]%p\n", monitor->index_tmp, monitor->current_task, error_nextI, error_nextT);
    fflush(stdout);

    monitor->_index = error_nextI;
    monitor->decision_state = ERROR_BCK;
}

/**
 * @brief EXIT error function state.
 * Called if the decision for the error is EXIT.
 * Print the decision, the current index, the current task and the error number.
 * Then make an exit call with the error number.
 */
void errorExitSt()
{
    int err = take_nextI();

    fprintf(stdout, "[EXIT][%d]%p ==> %d\n", monitor->index_tmp, monitor->current_task, err);
    fflush(stdout);

    exit(err);

}

/**
 * @brief saves in the monitor the index (number of the task) in the decision struct of the current task.
 * If the current task cannot be found in the decision struct (not allowed), the application will be stop with an exit call.
 */
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
    if(!find){
        //printf("[INVALID_TASK] The task was not found in the decision struct!\n");
        //fflush(stdout);
        exit(30);
    }
}

/**
 * @brief first state to be called at each new error.
 * Based on the error it decide the function state to call.
 * If there are ERROR_PATH or ERROR_REP_M errors, the decision will be taken from the errorStateMachine struct (strict of the monitor).
 * If there are ERROR_REP_T or ERROR_TIME_T errors, the decision will be taken from the decision struct, saved into the monitor, but decided by the user.
 * If there are other kind of errors (not allowed), the application will be stop with an exit call.
 * For the next operations a temporary value of the index is saved based on the progress variable value.
 */
void errorStoppedSt(){
    
    if(monitor->monitor_error_type == ERROR_PATH || monitor->monitor_error_type == ERROR_REP_M ){
        monitor->decision_state = errorStateMachine[monitor->monitor_error_type].decision;
    } else if(monitor->monitor_error_type == ERROR_REP_T || monitor->monitor_error_type == ERROR_TIME_T){
        monitor->decision_state = (monitor->decision + monitor->taskIndex)->decision;
    } else {
        fprintf(stdout, "[INVALID_ERROR] The error type is invalid!\n");
        fflush(stdout);
        exit(20);
    }

    monitor->index_tmp = monitor->index;
    if(monitor->progress == TASKENDING || monitor->progress == TASKENDED){
        monitor->index_tmp = monitor->index_tmp - 1;
    }

}

/**
 * @brief main monitor error state.
 * Will call the different function state based on the error.
 * In the end stops changes the state of the current thread and stops the monitor.
 */
void stMonitorError(){
    fprintf(stdout, "%s\n", error_string[monitor->monitor_error_type]);
    fflush(stdout);
    
    
    while(monitor->decision_state != DECISION_STOPPED){
        (*decision_func[monitor->decision_state].func)();
    }
    
    (monitor->current_thread)->state = TASK_READY;
    monitor->function_state = FUNCTION_ENDED;
    monitor->state = MONITOR_STOPPED;
}

/**
 * @brief check if the monitor repetitions are lower than the monitor repetitions threshold.
 * 
 * @return boolean TRUE if the current repetitions are lower than the threshold.
 */
boolean checkMonitorThresholdRep(){
    return (monitor->monitor_rep < MONITOR_REP_THRESHOLD);
}

/**
 * @brief called at every reboot of the application.
 * Check if the monitor was interrupted during its execution.
 * In case of interruption of the monitor the monitor repetitions count is incremented,
 * monitor_is_rep variable becomes TRUE to avoid repetitions of the same opertaions during monitor calls.
 * If the repetitions of the monitor are lowet than the threshold monitor_entry function is called to finish the remaining operations before all.
 * In case of too many repetitions of the monitor the monitor state is changed in MONITOR_ERROR so it'll be called in the monitor_entry function.
 */
void reboot_monitor(){

    if(monitor->state == MONITOR_STARTED || monitor->state == MONITOR_FINISHED || monitor->state == MONITOR_ERROR){
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

/**
 * @brief Initial boot of the monitor. Called only during the first boot of the application.
 * Here all the monitor variables are initialised with starting values.
 * For the variables that can be choosed by the user, their values are copied in the corresponding variables of the monitor.
 * 
 * @param num_tr number of the transitions of the graph.
 * @param graph path of the application.
 * @param time_threshold application execution time threshold
 * @param num_task number of different tasks (they can be repeated in the graph).
 * @param decision  is a struct containing for each different task in the graph (see monitor.h for the struct definition):
 *                  - in case of error, what should be done ([SKIP] skipping to a different task, [RESTART] restart the application from a previous point, [EXIT] exit the application with a certain error number).
 *                  - index to the skip/restart point or as the error exit number.
 *                  - execution time threshold.
 *                  - repetition of the same task threshold.
 *                  - call for a restart from the same task threshold.
 */
void boot_init_monitor(int num_tr, void** graph, long int time_threshold, int num_task, state_machine_decision* decision){
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
    monitor->num_task = num_task;
    monitor->decision = (state_machine_decision*) malloc (num_task * sizeof(state_machine_decision));
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

/**
 * @brief initialized the task repetitions count in case of a new task.
 * The variables that track the last new task encountered by the monitor will be updated (UPDATE_LTASK function state).
 */
void init_count(){
    monitor->count_rep = 0;
    monitor->_count_rep = 0;
    monitor->function_state = UPDATE_LTASK;
}

/**
 * @brief check if the task repetiotions threshold for the current task is respected.
 * 
 * @return boolean TRUE if the current number of repetitions is lower than the threshold.
 */
boolean checkThresholdRep(){
    findCurrI();
    return (monitor->_count_rep < ((monitor->decision + monitor->taskIndex)->repeat_threshold));
}

/**
 * @brief called in case of repetition of the same task.
 * If the repetions exceed the repetitions threshold the MONITOR_ERROR state will be called.
 * After this the operations are over (FUNCTION_ENDED).
 */
void incrementCountTask(){
    monitor->_count_rep = monitor->count_rep + 1;
    if(!checkThresholdRep()){
        monitor->monitor_error_type = ERROR_REP_T;
        monitor->decision_state = NEW_ERROR;
        monitor->state = MONITOR_ERROR;
    }
    monitor->function_state = FUNCTION_ENDED;
}

/**
 * @brief increment the index to the current point on the path saved in the monitor.
 * Based on the state in thread.c (if the task is ended or not) it updates the index.
 * After this the operations are over (FUNCTION_ENDED).
 */
void incrementIndexProgression(){
    if(monitor->progress == TASKENDING){
        monitor->_index = monitor->index + 1;
    }
    monitor->function_state = FUNCTION_ENDED;
}

/**
 * @brief check the correctness of the current task based on the path saved in the monitor.
 * In case of success the index pointing to the path is updated (COUNT_PROGRESS function state).
 * In case of an unrecognized task, operations are stopped (FUNCION_ENDED) to call the MONITOR_ERROR state.
 */
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

/**
 * @brief keeps the progression.
 * In case of non repetition of the same task (so we have a new task for the monitor based on the path)
 * the monitor saves the current task as the last new current task that it encountered.
 * Then the correctness of the task based on the path will be checked (CHECK_PROGRESS function state).
 */
void updateLastTask(){
    if(monitor->progress == TASKSTARTING){
        monitor->_last_task_s = monitor->current_task;
    } else if (monitor->progress == TASKENDING){
        monitor->_last_task_e = monitor->current_task;
    }
    monitor->function_state = CHECK_PROGRESS;
}

/**
 * @brief check if the current task is repeating itself for a failure during its execution.
 * In case of repetition the repetitions count variable is incremented (COUNT_TASK function state).
 * In case of new task, the repetitions count variable is initialised (INIT_COUNT function state).
 */
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

/**
 * @brief check is the current task time threshold is respected.
 * 
 * @return boolean TRUE if the task exceeded its time threshold.
 */
boolean checkTime(){
    findCurrI();
    return (*(monitor->transactions + (monitor->index-1)) > (monitor->decision + monitor->taskIndex)->time_threshold);
}

/**
 * @brief simple and temporary function that returns a pseudo-random long int value that will be used as execution time.
 * 
 * @return long int pseudo-random value as execution time in ms.
 */
long int getTime(){
    return (100 * (monitor->index +1));
}

/**
 * @brief save tasks and application execution times.
 * Check time thresholds on tasks and application executions.
 * If times thresholds of the tasks are exceeded, the MONITOR_ERROR state will be called.
 */
void timeFunctionSt(){
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
    }
    monitor->function_state = FUNCTION_ENDED;
}

/**
 * @brief the only function called by the final monitor state.
 * It makes the backup of some important monitor variables based also on the progression variable.
 * In case of TASKENDED, last task pointers are initialised in case the next task is the same one of just finished.
 * After this function, the operations final main state (MONITOR_FINISHED) end (FUNCTION_ENDED).
 */
void backupMonitor(){
    if(monitor->progress != TASKENDED){
        if(monitor->progress == TASKSTARTING){
            monitor->last_task_s = monitor->_last_task_s;
        } else if (monitor->progress == TASKENDING){
            monitor->last_task_e = monitor->_last_task_e;
        }
        monitor->count_rep = monitor->_count_rep;
        monitor->index = monitor->_index;
    } else {
        monitor->last_task_s = NULL;
        monitor->_last_task_s = NULL;
        monitor->last_task_e = NULL;
        monitor->_last_task_e = NULL;
    }
    monitor->function_state = FUNCTION_ENDED;
}

/**
 * @brief final main state.
 * Main operations are the backup of some monitor variables.
 * The backup is made to respect the WAR property: in case of power failure you can maintain the correct continuity and progression of the code.
 */
void stMonitorFinished(){
    while(monitor->function_state != FUNCTION_ENDED){
        (*functionStateMachine[monitor->function_state].func)();
    }
    monitor->state = MONITOR_STOPPED;
}

/**
 * @brief main state that checks the different constraints.
 * In this monitor version the main operations are:
 * - check correctness of the path that is executing.
 * - check the repetition of the same task.
 * - incrementing index to following the path.
 * - save the tasks and application execution times.
 * - check time thresholds on tasks and application.
 */
void stMonitorStarted(){
    while(monitor->function_state != FUNCTION_ENDED && monitor->function_state != MONITOR_BACKUP){
        (*functionStateMachine[monitor->function_state].func)();
    }

    if(monitor->state != MONITOR_ERROR){
        monitor->function_state = MONITOR_BACKUP;
        monitor->state = MONITOR_FINISHED;
    }
}

/**
 * @brief state executed at every new call of the monitor.
 * It prepares some main variables based on the current call used during the monitor operations.
 * Based on the progress value different operations will be executed in MONITOR_STARTED state.         
 * 
 * @param task current task pointer
 * @param progress indicate in which state in thread.c was called the function
 * @param thread current thread pointer
 */
void stMonitorStopped(void *task, progress_t progress, thread_t *thread){
    monitor->monitor_rep = 0;
    monitor->current_task = task;
    monitor->progress = progress;
    monitor->current_thread = thread;

    if(progress == TASKENDED){
        monitor->function_state = MONITOR_TIME;
    } else {
        monitor->function_state = CHECK_REP;
    }
    monitor->state = MONITOR_STARTED;
}

/**
 * @brief function called to start the monitor code.
 * First if     [MONITOR_STOPPED && !monitor_is_rep]    =>  new call of the monitor.
 * Second if    [not MONITOR_STOPPED && monitor_is_rep] =>  monitor was interrupted during its execution so it restarts from the last state that was executing.
 * Third if     [MONITOR_STOPPED && monitor_is_rep]     =>  monitor was interrupted and finished the operation by the call of this function made by the reboot function of the monitor.
 *                                                          Once the monitor_entry is called again from the thread.c for the same operations, they're skipped.
 * 
 * @param task current task pointer
 * @param progress indicate in which state in thread.c was called the function
 * @param thread current thread pointer
 */
void monitor_entry(void *task, progress_t progress, thread_t *thread){
    if(!monitor->monitor_is_rep){
        monitor->control_state = CONTROL_ENTRY;
        
        (*monitorStateMachine[monitor->state].func_p)(task, progress, thread);
        
        while(monitor->state != MONITOR_STOPPED){
            (*monitorStateMachine[monitor->state].func_v)();
        }
    } else if(monitor->state != MONITOR_STOPPED){
        while(monitor->state != MONITOR_STOPPED){
            (*monitorStateMachine[monitor->state].func_v)();
        }
    } else {
        monitor->monitor_is_rep = FALSE;
    }

    monitor->control_state = CONTROL_STOPPED;
}
