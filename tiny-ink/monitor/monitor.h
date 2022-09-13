/*
 * monitor.h
 *
 *  Created on: 1 ago 2022
 *      Author: lorenzo
 */

#ifndef MONITOR_MONITOR_H_
#define MONITOR_MONITOR_H_

//Max number of tasks that can be managed by the monitor
#define MAXTASK 64
//Error number if the number of tasks created exceed MAXTASK
#define ERRORTASK -1


void shiftArray(int final);
int checkTask(void *currentTask);
void init_monitor_fram();
void start_monitor(void *task);
void end_monitor(void *task);


#endif /* MONITOR_MONITOR_H_ */
