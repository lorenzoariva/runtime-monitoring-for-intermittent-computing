/*
 * monitor.c
 *
 *  Created on: 1 ago 2022
 *      Author: lorenzo
 */

#include "monitor.h"
#include "../tiny-ink/ink.h"

//Static Version
uint8_t st1,st2,st3 = 0;
uint8_t et1,et2,et3 = 0;

/* FRAM VERSION
__shared(
    uint8_t st1,st2,st3;
    uint8_t et1,et2,et3;
)*/

// Based on the pointer memory value return the index of the current thread
int selStartThread(void *next)
{
    int selected = 0;
    if ((long) next == THREAD1)
    {
        selected = 0;
    }
    else if ((long) next == THREAD2)
    {
        selected = 1;
    }
    else if ((long) next == THREAD3)
    {
        selected = 2;
    }
    return selected;
}

// Based on the pointer memory value return the index of the previous thread
int selEndThread(void *next)
{
    int selected = 0;
    if ((long) next == THREAD1)
    {
        selected = 2;
    }
    else if ((long) next == THREAD2)
    {
        selected = 0;
    }
    else if ((long) next == THREAD3)
    {
        selected = 1;
    }

    return selected;
}

//Update the count of time that the current thread started
void start_monitor(void *next)
{
    int numT = selStartThread(next);
    //uint8_t tmp;
    if (numT == 0)
    {
        st1++;
        //tmp = __GET(st1);
        //__SET(st1, tmp + 1);

    }
    else if (numT == 1)
    {
        st2++;
        //tmp = __GET(st2);
        //__SET(st2, tmp + 1);

    }
    else if (numT == 2)
    {
        st3++;
        //tmp = __GET(st3);
        //__SET(st3, tmp + 1);

    }
}

//Update the count of time that the current thread ended
void end_monitor(void *next)
{
    int numT = selEndThread(next);
    //uint8_t tmp;
    if (numT == 0)
    {
        et1++;
        //tmp = __GET(et1);
        //__SET(et1, tmp + 1);

    }
    else if (numT == 1)
    {
        et2++;
        //tmp = __GET(et2);
        //__SET(et2, tmp + 1);

    }
    else if (numT == 2)
    {
        et3++;
        //tmp = __GET(et3);
        //__SET(et3, tmp + 1);

    }

}

