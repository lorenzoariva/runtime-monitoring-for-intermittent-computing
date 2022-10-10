#ifndef THREAD_H_
#define THREAD_H_

// the state of the threads
typedef enum {TASK_READY = 1, TASK_FINISHED = 2, TASK_COMMIT = 4,THREAD_STOPPED = 8} state_t;

// each thread will hold the double buffer for the variables
// shared by the tasks it is encapsulating.
typedef struct {
    void *buf[2];           // holds original and temporary stack pointers
    volatile uint8_t idx;   // index of the original buffer
    volatile uint8_t _idx;  // index of the new buffer
    uint16_t size;          // sizes of the buffers
}buffer_t;

// the task definition (single C function)
// the parameter param will be passed by the run-time
// and it holds the thread structure defined below.
typedef void* (*task_t) (buffer_t *);

// the entry task should take event data as an argument.
typedef void* (*entry_task_t) (buffer_t *,void *event);

// the main thread structure that holds all necessary info
// to execute the computation represented by the wired
// tasks
typedef struct {
    uint8_t priority;       // thread priority (unique)
    volatile state_t state; // thread state
    void *entry;            // the first task to be executed
    void *next;             // the current task to be executed
    void *_next;             // the current task to be executed
    buffer_t buffer;        // holds task shared persistent variables
}thread_t;

// allocates a double buffer for the persistent variables in FRAM
#define __shared(...)   \
        typedef struct {    \
            __VA_ARGS__     \
        } FRAM_data_t  __attribute__ ((aligned (2)));    \
        static __nv FRAM_data_t __persistent_vars[2];

// runs one task inside the current thread.
void __tick(thread_t *thread);

#endif /* THREAD_H_ */
