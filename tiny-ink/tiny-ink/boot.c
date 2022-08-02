#include <msp430.h>
#include "ink.h"

// indicates if this is the first boot.
__nv uint8_t __inited = 0;

//global time in ticks
extern uint32_t current_ticks;

// this is the entry function for the application initialization.
// applications should implement it.
extern void __app_init();
extern void __app_reboot();

/* Tells the compiler that the application layer can choose to implement
 * these functions or not. By default, Default_Handler will be called. */
void _default_init (void) __attribute__((weak));
void _default_reboot (void) __attribute__((weak));

void _default_init(void){};
void _default_reboot(void){};

/* app can implement these functions. Otherwise, the default implementations
 * will be linked. */
extern void _ink_init(void) __attribute__((weak,alias("_default_init")));
extern void _ink_reboot(void) __attribute__((weak,alias("_default_reboot")));

int ink_boot(void)
{
    // always init microcontroller
    __mcu_init();

    // if this is the first boot
    if(!__inited){
        // init the scheduler state
        __scheduler_boot_init();
        // init the applications
        //_default_init();
        _ink_init();
        // the first and initial boot is finished
        __inited = 1;
    }

    // will be called at each reboot of the application
    _ink_reboot();

    // activate the scheduler
    __scheduler_run();

    return 0;
}
