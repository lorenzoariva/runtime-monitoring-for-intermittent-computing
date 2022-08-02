/*
 * monitor.h
 *
 *  Created on: 1 ago 2022
 *      Author: lorenzo
 */

#ifndef MONITOR_MONITOR_H_
#define MONITOR_MONITOR_H_

#define THREAD1 0x0001057E
#define THREAD2 0x00010528
#define THREAD3 0x000104DC

int selectThread(void *next);
void start_monitor(void *next);
void end_monitor(void *next);

#endif /* MONITOR_MONITOR_H_ */
