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

// called at the first boot (just one time)
void _ink_init(){
    // create a thread with priority 15 and entry task task1
    __CREATE(THREAD1,task1);
    __SIGNAL(THREAD1);
}


ENTRY_TASK(task1){
    //tell monitor that task1 started
    start_monitor(TASK1);

    uint8_t m;

    m = __GET(a[0]);

    __SET(a[4],m + 10);

    //tell monitor that task1 ended
    end_monitor(TASK1);

    // next task is task2
    return task2;
}

TASK(task2){
    //tell monitor that task2 started
    start_monitor(TASK2);

    uint16_t val = __GET(a[4]);


    if(val==10)
        __led_toggle(LED1);

    __SET(a[99],0xffff);

    //tell monitor that task2 ended
    end_monitor(TASK2);

    //there is no next task
    return task3;
}

TASK(task3){
    //tell monitor that task3 started
    start_monitor(TASK3);

    uint16_t val1 = __GET(a[99]);
    uint16_t val2 = __GET(a[4]);

    if(val2==10 && val1==0xffff)
        __led_toggle(LED1);

    //tell monitor that task3 ended
    end_monitor(TASK3);

    //there is no next task
    return task1;
}
