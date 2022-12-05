#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "task.h"
#include "thread.h"

#define MAX_THREADS NUM_PRIORITIES

void __scheduler_boot_init();
void __scheduler_run();
void __create_thread(uint8_t priority, void *entry, void *data_org,
                    void *data_temp, uint16_t size);

// restart thread
void __start_thread(thread_t *thread);

// stop thread
void __stop_thread(thread_t *thread);

// priority to thread conversion
thread_t *__get_thread(uint8_t priority);


#endif /* SCHEDULER_H_ */
