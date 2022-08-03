/*
 * monitor.c
 *
 *  Created on: 1 ago 2022
 *      Author: lorenzo
 */

#include "monitor.h"
#include "../tiny-ink/ink.h"

//FRAM monitor variables
__nv uint8_t st1;
__nv uint8_t st2;
__nv uint8_t st3;
__nv uint8_t et1;
__nv uint8_t et2;
__nv uint8_t et3;


//Update the count of time that the current thread started
void start_monitor(int index)
{

    if (index == 0)
    {
        st1++;

    }
    else if (index == 1)
    {
        st2++;

    }
    else if (index == 2)
    {
        st3++;

    }
}

//Update the count of time that the current thread ended
void end_monitor(int index)
{

    if (index == 0)
    {
        et1++;

    }
    else if (index == 1)
    {
        et2++;

    }
    else if (index == 2)
    {
        et3++;

    }

}

