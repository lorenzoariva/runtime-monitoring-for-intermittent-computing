/*
 * monitor.c
 *
 *  Created on: 1 ago 2022
 *      Author: lorenzo
 */

#include "monitor.h"
#include "../tiny-ink/ink.h"

/*

//FRAM monitor variables
__nv uint8_t st1;
__nv uint8_t st2;
__nv uint8_t st3;
__nv uint8_t et1;
__nv uint8_t et2;
__nv uint8_t et3;
*/

//PRAGMA VERSION
#pragma NOINIT(__st1)
uint8_t __st1;
#pragma NOINIT(__st2)
uint8_t __st2;
#pragma NOINIT(__st3)
uint8_t __st3;
#pragma NOINIT(__et1)
uint8_t __et1;
#pragma NOINIT(__et2)
uint8_t __et2;
#pragma NOINIT(__et3)
uint8_t __et3;

//init the fram variables if it's the first boot
void init_monitor_fram(){
    __st1 = 0;
    __st2 = 0;
    __st3 = 0;
    __et1 = 0;
    __et2 = 0;
    __et3 = 0;
}


//Update the count of time that the current thread started
void start_monitor(int index)
{

    if (index == 0)
    {
        __st1++;

    }
    else if (index == 1)
    {
        __st2++;

    }
    else if (index == 2)
    {
        __st3++;

    }
}

//Update the count of time that the current thread ended
void end_monitor(int index)
{

    if (index == 0)
    {
        __et1++;

    }
    else if (index == 1)
    {
        __et2++;

    }
    else if (index == 2)
    {
        __et3++;

    }

}

