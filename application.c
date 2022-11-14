#include "tiny-ink/ink.h"
#include "monitor/monitor.h"


// define thread priority
#define THREAD1 15

// define task-shared persistent variables (in FRAM).
__shared(
    uint8_t x,y;
    int16_t a[100];
)

ENTRY_TASK(task1);
TASK(task2);
TASK(task3);

/* temporary variables for testing (emulate the user choices) */
int num_tr_tmp = 6;
void *graph_tmp[6] = { task1, task2, task3, task1, task2, task3 };
long int time_threshold_tmp = 80000;
int num_task_tmp = 3;
state_machine_decision decision_tmp[] = {
    {task1, SKIP, 2, 10000, 2, 0},
    {task2, RESTART, 0, 50000, 1, 1},
    {task3, EXIT, 3, 10000, 3, 0}
};

// called at the first boot (just one time)
void _ink_init(){
    /* init the monitor (only first boot) */
    boot_init_monitor(num_tr_tmp, graph_tmp, time_threshold_tmp, num_task_tmp, decision_tmp);
    // create a thread with priority 15 and entry task task1
    __CREATE(THREAD1,task1);
    __SIGNAL(THREAD1);
}


ENTRY_TASK(task1){


    uint8_t m;

    m = __GET(a[0]);


    __SET(a[0],m + 2);
    __SET(a[4],m + 10);



    // next task is task2
    return task2;
}

TASK(task2){


    uint8_t val = __GET(a[4]);


    if(val==10)
        __led_toggle(LED1);

    __SET(a[99],0xffff);



    //there is no next task
    return task3;
}

TASK(task3){


    uint16_t val1 = __GET(a[99]);
    uint16_t val2 = __GET(a[4]);

    if(val2==10 && val1==0xffff)
        __led_toggle(LED1);



    //there is no next task
    return task1;
}
