#include "../ink.h"

// prepares the stack of the thread for the task execution
static inline void __prologue(thread_t *thread)
{
    buffer_t *buffer = &thread->buffer;
    // copy original stack to the temporary stack
    __dma_word_copy(buffer->buf[buffer->idx],buffer->buf[buffer->idx ^ 1], buffer->size>>1);
}

// runs one task inside the current thread
void __tick(thread_t *thread)
{
    void *buf;
    void *currentTask;
    switch (thread->state)
    {
    case TASK_READY:
        // refresh thread stack
        __prologue(thread);
        // get thread buffer
        buf = thread->buffer.buf[thread->buffer._idx^1];

        //Keep a copy of the address of the current task (so it can be used also by end_monitor function).
        currentTask = thread->next;
        //Increment the number of time that the current task started.
        start_monitor(currentTask);

        // Check if it is the entry task. The entry task always
        // consumes an event in the event queue.
        thread->next = (void *)(((task_t)thread->next)(buf));
        thread->state = TASK_FINISHED;

    case TASK_FINISHED:
        //switch stack index to commit changes
        thread->buffer._idx = thread->buffer.idx ^ 1;

        //Increment the number of time that the current task ended.
        end_monitor(currentTask);

        thread->state = TASK_COMMIT;
    case TASK_COMMIT:
        // copy the real index from temporary index
        thread->buffer.idx = thread->buffer._idx;
        // Task execution finished. Check if the whole tasks are executed (thread finished)
        if (thread->next == NULL) {
            /* stop thread since we have no tasks */
            __stop_thread(thread);
        }
        else{
            // ready to execute successive tasks
            thread->state = TASK_READY;
        }
    }
}

