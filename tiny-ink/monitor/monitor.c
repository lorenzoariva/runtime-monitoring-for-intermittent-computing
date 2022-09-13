/*
 * monitor.c
 *
 *  Created on: 1 ago 2022
 *      Author: lorenzo
 */

#include "monitor.h"
#include "../tiny-ink/ink.h"

/* FRAM monitor variables */
// __trackTask array keeps track dynamically of the tasks it finds out during the execution of the code.
__nv void *__trackTask[MAXTASK];

// __startTask and __endTask arrays keep track of the number of starts and ends for each task in __trackTask.
// Both of them upload themselves when __trackTask is uploaded.
__nv int __startTask[MAXTASK];
__nv int __endTask[MAXTASK];


/**
 * Shift all the element in __trackTask, __startTask and __endTask arrays of one position to the right
 * from the last element to the element in "final" position.
 */
void shiftArray(int final)
{
    int i = MAXTASK - 1;

    while (i >= 0 && i >= final)
    {
        if (__trackTask[i] != 0)
        {
            __trackTask[i + 1] = __trackTask[i];
            __startTask[i + 1] = __startTask[i];
            __endTask[i + 1] = __endTask[i];
        }
        i--;
    }
}

/**
 * Based on the address of currentTask and the addresses present in __trackTask array:
 *  -   if the currentTask address is already present in __trackTask it returns the index of its position in __trackTask.
 *  -   if the address saved in __trackTask in position i is grater than currentTask address, shiftArray function is called. Then currentTask address
 *      is saved in the __trackTask array in position i while __startTask and __endTask are set to 0 in position i (as the address of currentTask is of a newly discovered task).
 *  -   if the currentTask address refers to a new task that is "greater" than all the other already saved in __trackTask, it's saved in the first empty position in __trackTask (the first one with value "0").
 *  -   if the condition of the while cycle is no longer respected ERRORTASK is returned. This means that a new task is found out, but the __trackTask is already full.
 *      In this case the number of tasks present in the code are greater than the limit imposed by the MAXTASK variable definied in "monitor.h".
 */
int checkTask(void *currentTask)
{
    int i = 0;
    while (i < MAXTASK)
    {
        if (currentTask == __trackTask[i])
        {
            return i;
        }
        else if (currentTask < __trackTask[i])
        {
            shiftArray(i);
            __trackTask[i] = currentTask;
            __startTask[i] = 0;
            __endTask[i] = 0;
            return i;
        }
        else if (__trackTask[i] == 0)
        {
            __trackTask[i] = currentTask;
            return i;
        }
        else
        {
            i++;
        }

    }

    //if any cases there is an error because there are too many tasks
    return ERRORTASK;
}

/**
 * Initialize the FRAM variables used by the monitor (should be called only on the fist boot).
 */
void init_monitor_fram()
{
    for (int i = 0; i < MAXTASK; i++)
    {
        __trackTask[i] = NULL;
        __startTask[i] = 0;
        __endTask[i] = 0;
    }
}

/**
 * Update the count of time that the current task started.
 * If ERRORTASK is returned by checkTask, the number of tasks exceeded the limit imposed by MAXTASK definied in "monitor.h".
 */
void start_monitor(void *task) //pointer parameter
{
    int pos = checkTask(task);
    if (pos == ERRORTASK)
    {
        //[ERROR] The number of tasks exceeded the limit imposed by MAXTASK.
    }
    else
    {
        __startTask[pos]++;
    }


}

/**
 * Update the count of time that the current task ended.
 * If ERRORTASK is returned by checkTask, the number of tasks exceeded the limit imposed by MAXTASK definied in "monitor.h".
 */
void end_monitor(void *task)
{
    int pos = checkTask(task);
    if (pos == ERRORTASK)
    {
        //[ERROR] The number of tasks exceeded the limit imposed by MAXTASK.
    }
    else
    {
        __endTask[pos]++;
    }
}
