#include "esos.h"
#include "main.h"
#include "user_app.h"
#include <stdio.h>
#include <string.h>

// *************************************************************************************************
// Barrier Semaphore Implementation Example:
//
// n = the number of threads
// threadCount = 0
// mutex = Semaphore(1)
// barrier = Semaphore(0)
//
//
// mutex.wait()
// threadCount = threadCount + 1
// mutex.signal()
//
// if threadCount == n: barrier.signal() # unblock ONE thread
//
// barrier.wait()
// barrier.signal() # once we are unblocked, it's our duty to unblock the next thread
// *************************************************************************************************


//  Generally, the user-created semaphores will be defined/allocated here
// static uint8_t psz_CRNL[3]= {0x0D, 0x0A, 0};

// timer globals

#ifdef _linux
/*
 * Simulate the timer ISR found on a MCU
 *   The PC doesn't have a timer ISR, so this task will periodically
 *   call the timer services callback instead.
 *   USED ONLY FOR DEVELOPMENT AND TESTING ON PC.
 *   Real MCU hardware doesn't need this task
 */
ESOS_USER_TASK(__simulated_isr)
{
    ESOS_TASK_BEGIN();
    while (TRUE)
    {
        // call the ESOS timer services callback just like a real H/W ISR would
        __esos_tmrSvcsExecute();
        ESOS_TASK_WAIT_TICKS(1);

    } // endof while(TRUE)
    ESOS_TASK_END();
} // end child_task
#endif
//__________________________________________________________________________
ESOS_USER_TASK(flash_LED)
{
    ESOS_TASK_BEGIN();
    while (TRUE)
    {
        gpio_toggle(LED2_PORT, LED2_PIN);
        ESOS_TASK_WAIT_TICKS(250);
    } // endof while(TRUE)
    ESOS_TASK_END();
} // end flash_LED()
//________________________________________________________________________
//************************************************************************************************************************
// CHILD TASKS FOR thread_n:

// MAIN thread_n:
/************************************************************************
 * User supplied functions
 ************************************************************************
 */
void __user_init_hw(void)
{
    /* GPIO Ports Clock Enable */
    rcc_periph_clock_enable(GPIOA);

    /*Configure GPIO pin : LED2_Pin */
    gpio_mode_setup(LED2_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED2_PIN);
}

/******************************************************************************
 * Function:        void user_init(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        user_init is a centralized initialization routine where
 *                  the user can setup their application.   It is called
 *                  automagically by ES_OS during the operating system
 *                  initialization.
 *
 * Note:            The user should set up any state machines and init
 *                  all application variables.  They can also turn on
 *                  any needed peripherals here.
 *
 *                  The user SHALL NOT mess with the interrupt hardware
 *                  directly!!!  The ES_OS must be aware of the interrupts
 *                  and provides osXXXXXXX functions for the user to use.
 *                  Using these ES_OS-provided functions, the user may
 *                  (and probably should) initialize, register, and enable
 *                  interrupts in this routine.
 *
 *                  Furthermore, the user should register AT LEAST one
 *                  user application task here (via esos_RegisterTask) or
 *                  the ES_OS scheduler will have nothing to schedule
 *                  to run when this function returns.
 *
 *****************************************************************************/
void user_init(void)
{
    // call our function to setup the hardware we like it.
    __user_init_hw();

    ESOS_INIT_SEMAPHORE(mutex, 1);
    ESOS_INIT_SEMAPHORE(barrier, 0);

    /*
     * Now, let's get down and dirty with ESOS and our user tasks
     *
     *   Once tasks are registered, they will start executing in
     *   the ESOS scheduler.
     */
    esos_RegisterTask(flash_LED);
    // generate NUMTHREADS threads defined pre-compile at top of file
    esos_RegisterTask(thread1);
    esos_RegisterTask(thread2);
    esos_RegisterTask(thread3);
    esos_RegisterTask(thread4);
    esos_RegisterTask(thread5);


} // end user_init()
