#include "../ink.h"
#include "scheduler.h"
#include "priority.h"

// all threads in the system
static __nv thread_t _threads[MAX_THREADS];

// variables for keeping track of the ready threads
static __nv priority_t _priorities;

enum { SCHED_SELECT, SCHED_BUSY };

// the id of the current thread being executed.
static __nv thread_t *_thread = NULL;

static volatile __nv uint8_t _sched_state = SCHED_SELECT;

void __scheduler_boot_init() {
    uint8_t i;

    // clear priority variables for the threads
    __priority_init(&_priorities);

    for (i = MAX_THREADS; i > 0; i--){
        // threads are not created yet
        _threads[i].state == THREAD_STOPPED;
    }
    _sched_state = SCHED_SELECT;
}

// Assigns a slot to a thread. Should be called ONLY at the first system boot
void __create_thread(uint8_t priority, void *entry, void *data_org,
                     void *data_temp, uint16_t size)
{
    // init properties
    _threads[priority].priority = priority;
    _threads[priority].entry = entry;
    _threads[priority].next = entry;
    _threads[priority].state = THREAD_STOPPED;

    // init shared buffer
    _threads[priority].buffer.buf[0] = data_org;
    _threads[priority].buffer.buf[1] = data_temp;
    _threads[priority].buffer.idx = 0;
    _threads[priority].buffer.size = size;
}

// puts the thread in waiting state
inline void __stop_thread(thread_t *thread){
    __priority_remove(thread->priority, &_priorities);
    thread->state = THREAD_STOPPED;
}

// puts the thread in active state
inline void __start_thread(thread_t *thread) {
    thread->next = thread->entry;
    __priority_insert(thread->priority, &_priorities);
    thread->state = TASK_READY;
}

// returns the highest-priority thread
static inline thread_t *__next_thread(){
    uint8_t idx = __priority_highest(&_priorities);
    if(idx)
        return &_threads[idx];

    return NULL;
}

inline thread_t *__get_thread(uint8_t priority){
    return &_threads[priority];
}

// finish the interrupted task before enabling interrupts
static inline void __task_commit(){
    if(_thread){
        __tick(_thread);
    }
}

// at each step, the scheduler selects the highest priority thread and
// runs the next task within the thread
void __scheduler_run()
{
    /* always finalize the latest task */
    __task_commit();
    /* enable interrupts */
    __enable_interrupt();

    while (1){
        switch (_sched_state){
        case SCHED_SELECT:
            // the scheduler selects the highest priority task right
            // after it has finished the execution of a single task
            _thread = __next_thread();
            _sched_state = SCHED_BUSY;
        case SCHED_BUSY:
            // always execute the selected task to completion
            // execute one task inside the highest priority thread
            if (_thread){
                __tick(_thread);
                // after execution of one task, check the events
                _sched_state = SCHED_SELECT;
                break;
            }
            _sched_state = SCHED_SELECT;
            // check the ready queue for the last time
            if(!__next_thread()){
                /* put MCU in sleep mode */
                __mcu_sleep();
            }
        }
    }
}
