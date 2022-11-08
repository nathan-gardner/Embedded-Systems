#include "main.h"
#include "esos.h"
#include "user_app.h"
#include <stdio.h>
#include <string.h>

#ifdef __linux
#include "esos_pc.h"
#include "esos_pc_stdio.h"
// INCLUDE these so our printf and other PC hacks work
#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#else
#include "esos_stm32l4.h"
#include "esos_stm32l4_rs232.h"
#endif


uint8_t threadCount = 0;
uint8_t n = NUMTHREADS;
uint8_t TID = 0;
uint8_t psz_CRNL[3] = { 0x0D,0x0A,0 };
ESOS_SEMAPHORE(mutex);
ESOS_SEMAPHORE(barrier);

//_____________________________________________________________________________________________________________________________
/**
 * @brief delays using __asm("nop") for a number of cycles defined by paramter ops
 *
 * @param ops
 */
void delayOPs(uint32_t ops) {
    for (int i = 0; i < ops; i++) {
        __asm("nop");
    }
}
//_________________________________________________________________________________________________________________
ESOS_CHILD_TASK(print_mutex_wait, uint32_t n) {

    ESOS_TASK_BEGIN();
    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING("TASK ");
    ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(n);
    ESOS_TASK_WAIT_ON_SEND_STRING(": waiting at mutex");
    ESOS_TASK_WAIT_ON_SEND_STRING(psz_CRNL);
    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
    ESOS_TASK_END();
}
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
ESOS_CHILD_TASK(print_barrier_wait, uint32_t n) {

    ESOS_TASK_BEGIN();
    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING("TASK ");
    ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(n);
    ESOS_TASK_WAIT_ON_SEND_STRING(": waiting at barrier");
    ESOS_TASK_WAIT_ON_SEND_STRING(psz_CRNL);
    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
    ESOS_TASK_END();
}
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
ESOS_CHILD_TASK(print_mutex_signal, uint32_t n) {

    ESOS_TASK_BEGIN();
    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING("TASK ");
    ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(n);
    ESOS_TASK_WAIT_ON_SEND_STRING(": signaling mutex to unblock another task.");
    ESOS_TASK_WAIT_ON_SEND_STRING(psz_CRNL);
    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
    ESOS_TASK_END();
}
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
ESOS_CHILD_TASK(print_barrier_signal, uint32_t n) {

    ESOS_TASK_BEGIN();
    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING("TASK ");
    ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING(n);
    ESOS_TASK_WAIT_ON_SEND_STRING(": signaling barrier to open for another task.");
    ESOS_TASK_WAIT_ON_SEND_STRING(psz_CRNL);
    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
    ESOS_TASK_END();
}

//__________________________________________________________________________________________________________________
ESOS_USER_TASK(thread1) {
    static uint32_t local_TID;
    static ESOS_TASK_HANDLE mutex_waitMSG_child, barrier_waitMSG_child, mutex_signalMSG_child, barrier_signalMSG_child;

    ESOS_TASK_BEGIN();
    local_TID = TID;
    TID++;
    while (TRUE)
    {
        // Send message to serial stating that we are signaling the mutex
        ESOS_ALLOCATE_CHILD_TASK(mutex_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_waitMSG_child, print_mutex_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        threadCount = threadCount + 1;

        ESOS_ALLOCATE_CHILD_TASK(mutex_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_signalMSG_child, print_mutex_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);

        // unblock one thread
        if (threadCount == NUMTHREADS) {
            // Send message to serial stating that we are signaling the barrier
            ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
            ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();


        }

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_waitMSG_child, print_barrier_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();  // once we are unblocked, we must unblock the next thread


    }
    ESOS_TASK_END();
}
//__________________________________________________________________________________________________________________
ESOS_USER_TASK(thread2) {
    static uint32_t local_TID;
    static ESOS_TASK_HANDLE mutex_waitMSG_child, barrier_waitMSG_child, mutex_signalMSG_child, barrier_signalMSG_child;

    ESOS_TASK_BEGIN();
    local_TID = TID;
    TID++;
    while (TRUE)
    {
        // Send message to serial stating that we are signaling the mutex
        ESOS_ALLOCATE_CHILD_TASK(mutex_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_waitMSG_child, print_mutex_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        threadCount = threadCount + 1;

        ESOS_ALLOCATE_CHILD_TASK(mutex_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_signalMSG_child, print_mutex_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);
        // unblock one thread
        if (threadCount == NUMTHREADS) {
            // Send message to serial stating that we are signaling the barrier
            ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
            ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
        }

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_waitMSG_child, print_barrier_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
        // once we are unblocked, we must unblock the next thread

    }
    ESOS_TASK_END();
}
//__________________________________________________________________________________________________________________
ESOS_USER_TASK(thread3) {
    static uint32_t local_TID;
    static ESOS_TASK_HANDLE mutex_waitMSG_child, barrier_waitMSG_child, mutex_signalMSG_child, barrier_signalMSG_child;

    ESOS_TASK_BEGIN();
    local_TID = TID;
    TID++;
    while (TRUE)
    {
        // Send message to serial stating that we are signaling the mutex
        ESOS_ALLOCATE_CHILD_TASK(mutex_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_waitMSG_child, print_mutex_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        threadCount = threadCount + 1;

        ESOS_ALLOCATE_CHILD_TASK(mutex_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_signalMSG_child, print_mutex_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);
        // unblock one thread
        if (threadCount == NUMTHREADS) {
            // Send message to serial stating that we are signaling the barrier
            ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
            ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
        }

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_waitMSG_child, print_barrier_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
        // once we are unblocked, we must unblock the next thread

    }
    ESOS_TASK_END();
}
//__________________________________________________________________________________________________________________
ESOS_USER_TASK(thread4) {
    static uint32_t local_TID;
    static ESOS_TASK_HANDLE mutex_waitMSG_child, barrier_waitMSG_child, mutex_signalMSG_child, barrier_signalMSG_child;

    ESOS_TASK_BEGIN();
    local_TID = TID;
    TID++;
    while (TRUE)
    {
        // Send message to serial stating that we are signaling the mutex
        ESOS_ALLOCATE_CHILD_TASK(mutex_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_waitMSG_child, print_mutex_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        threadCount = threadCount + 1;

        ESOS_ALLOCATE_CHILD_TASK(mutex_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_signalMSG_child, print_mutex_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);
        // unblock one thread
        if (threadCount == NUMTHREADS) {
            // Send message to serial stating that we are signaling the barrier
            ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
            ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
        }

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_waitMSG_child, print_barrier_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
        // once we are unblocked, we must unblock the next thread

    }
    ESOS_TASK_END();
}
//__________________________________________________________________________________________________________________
ESOS_USER_TASK(thread5) {
    static uint32_t local_TID;
    static ESOS_TASK_HANDLE mutex_waitMSG_child, barrier_waitMSG_child, mutex_signalMSG_child, barrier_signalMSG_child;

    ESOS_TASK_BEGIN();
    local_TID = TID;
    TID++;
    while (TRUE)
    {
        // Send message to serial stating that we are signaling the mutex
        ESOS_ALLOCATE_CHILD_TASK(mutex_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_waitMSG_child, print_mutex_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        threadCount = threadCount + 1;

        ESOS_ALLOCATE_CHILD_TASK(mutex_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_signalMSG_child, print_mutex_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);
        // unblock one thread
        if (threadCount == NUMTHREADS) {
            // Send message to serial stating that we are signaling the barrier
            ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
            ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
        }

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_waitMSG_child, print_barrier_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
        // once we are unblocked, we must unblock the next thread

    }
    ESOS_TASK_END();
}
//__________________________________________________________________________________________________________________
ESOS_USER_TASK(thread6) {
    static uint32_t local_TID;
    static ESOS_TASK_HANDLE mutex_waitMSG_child, barrier_waitMSG_child, mutex_signalMSG_child, barrier_signalMSG_child;

    ESOS_TASK_BEGIN();
    local_TID = TID;
    TID++;
    while (TRUE)
    {
        // Send message to serial stating that we are signaling the mutex
        ESOS_ALLOCATE_CHILD_TASK(mutex_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_waitMSG_child, print_mutex_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        threadCount = threadCount + 1;

        ESOS_ALLOCATE_CHILD_TASK(mutex_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_signalMSG_child, print_mutex_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);
        // unblock one thread
        if (threadCount == NUMTHREADS) {
            // Send message to serial stating that we are signaling the barrier
            ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
            ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
        }

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_waitMSG_child, print_barrier_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
        // once we are unblocked, we must unblock the next thread

    }
    ESOS_TASK_END();
}
//__________________________________________________________________________________________________________________
ESOS_USER_TASK(thread7) {
    static uint32_t local_TID;
    static ESOS_TASK_HANDLE mutex_waitMSG_child, barrier_waitMSG_child, mutex_signalMSG_child, barrier_signalMSG_child;

    ESOS_TASK_BEGIN();
    local_TID = TID;
    TID++;
    while (TRUE)
    {
        // Send message to serial stating that we are signaling the mutex
        ESOS_ALLOCATE_CHILD_TASK(mutex_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_waitMSG_child, print_mutex_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        threadCount = threadCount + 1;

        ESOS_ALLOCATE_CHILD_TASK(mutex_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_signalMSG_child, print_mutex_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);
        // unblock one thread
        if (threadCount == NUMTHREADS) {
            // Send message to serial stating that we are signaling the barrier
            ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
            ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();

        }

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_waitMSG_child, print_barrier_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
        // once we are unblocked, we must unblock the next thread

    }
    ESOS_TASK_END();
}
//__________________________________________________________________________________________________________________
ESOS_USER_TASK(thread8) {
    static uint32_t local_TID;
    static ESOS_TASK_HANDLE mutex_waitMSG_child, barrier_waitMSG_child, mutex_signalMSG_child, barrier_signalMSG_child;

    ESOS_TASK_BEGIN();
    local_TID = TID;
    TID++;
    while (TRUE)
    {
        // Send message to serial stating that we are signaling the mutex
        ESOS_ALLOCATE_CHILD_TASK(mutex_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_waitMSG_child, print_mutex_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        threadCount = threadCount + 1;

        ESOS_ALLOCATE_CHILD_TASK(mutex_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_signalMSG_child, print_mutex_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);
        // unblock one thread
        if (threadCount == NUMTHREADS) {
            // Send message to serial stating that we are signaling the barrier
            ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
            ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();

        }

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_waitMSG_child, print_barrier_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
        // once we are unblocked, we must unblock the next thread

    }
    ESOS_TASK_END();
}
//__________________________________________________________________________________________________________________
ESOS_USER_TASK(thread9) {
    static uint32_t local_TID;
    static ESOS_TASK_HANDLE mutex_waitMSG_child, barrier_waitMSG_child, mutex_signalMSG_child, barrier_signalMSG_child;

    ESOS_TASK_BEGIN();
    local_TID = TID;
    TID++;
    while (TRUE)
    {
        // Send message to serial stating that we are signaling the mutex
        ESOS_ALLOCATE_CHILD_TASK(mutex_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_waitMSG_child, print_mutex_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        threadCount = threadCount + 1;

        ESOS_ALLOCATE_CHILD_TASK(mutex_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_signalMSG_child, print_mutex_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);
        // unblock one thread
        if (threadCount == NUMTHREADS) {
            // Send message to serial stating that we are signaling the barrier
            ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
            ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();

        }

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_waitMSG_child, print_barrier_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
        // once we are unblocked, we must unblock the next thread

    }
    ESOS_TASK_END();
}
//__________________________________________________________________________________________________________________
ESOS_USER_TASK(thread10) {
    static uint32_t local_TID;
    static ESOS_TASK_HANDLE mutex_waitMSG_child, barrier_waitMSG_child, mutex_signalMSG_child, barrier_signalMSG_child;

    ESOS_TASK_BEGIN();
    local_TID = TID;
    TID++;
    while (TRUE)
    {
        // Send message to serial stating that we are signaling the mutex
        ESOS_ALLOCATE_CHILD_TASK(mutex_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_waitMSG_child, print_mutex_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        threadCount = threadCount + 1;

        ESOS_ALLOCATE_CHILD_TASK(mutex_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_signalMSG_child, print_mutex_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);
        // unblock one thread
        if (threadCount == NUMTHREADS) {
            // Send message to serial stating that we are signaling the barrier
            ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
            ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();

        }

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_waitMSG_child, print_barrier_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
        // once we are unblocked, we must unblock the next thread

    }
    ESOS_TASK_END();
}
//__________________________________________________________________________________________________________________
ESOS_USER_TASK(thread11) {
    static uint32_t local_TID;
    static ESOS_TASK_HANDLE mutex_waitMSG_child, barrier_waitMSG_child, mutex_signalMSG_child, barrier_signalMSG_child;

    ESOS_TASK_BEGIN();
    local_TID = TID;
    TID++;
    while (TRUE)
    {
        // Send message to serial stating that we are signaling the mutex
        ESOS_ALLOCATE_CHILD_TASK(mutex_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_waitMSG_child, print_mutex_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        threadCount = threadCount + 1;

        ESOS_ALLOCATE_CHILD_TASK(mutex_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_signalMSG_child, print_mutex_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);
        // unblock one thread
        if (threadCount == NUMTHREADS) {
            // Send message to serial stating that we are signaling the barrier
            ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
            ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();

        }

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_waitMSG_child, print_barrier_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
        // once we are unblocked, we must unblock the next thread

    }
    ESOS_TASK_END();
}
//__________________________________________________________________________________________________________________
ESOS_USER_TASK(thread12) {
    static uint32_t local_TID;
    static ESOS_TASK_HANDLE mutex_waitMSG_child, barrier_waitMSG_child, mutex_signalMSG_child, barrier_signalMSG_child;

    ESOS_TASK_BEGIN();
    local_TID = TID;
    TID++;
    while (TRUE)
    {
        // Send message to serial stating that we are signaling the mutex
        ESOS_ALLOCATE_CHILD_TASK(mutex_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_waitMSG_child, print_mutex_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(mutex, 1);
        threadCount = threadCount + 1;

        ESOS_ALLOCATE_CHILD_TASK(mutex_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(mutex_signalMSG_child, print_mutex_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(mutex, 1);
        // unblock one thread
        if (threadCount == NUMTHREADS) {
            // Send message to serial stating that we are signaling the barrier
            ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
            ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
            ESOS_TASK_WAIT_TICKS(10000);
            ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();

        }

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_waitMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_waitMSG_child, print_barrier_wait, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_TASK_WAIT_SEMAPHORE(barrier, 1);

        // Send message to serial stating that we are waiting for the barrier
        ESOS_ALLOCATE_CHILD_TASK(barrier_signalMSG_child);
        ESOS_TASK_SPAWN_AND_WAIT(barrier_signalMSG_child, print_barrier_signal, local_TID);
        ESOS_TASK_WAIT_TICKS(10000);
        ESOS_SIGNAL_SEMAPHORE(barrier, 1); ESOS_TASK_YIELD();
        // once we are unblocked, we must unblock the next thread

    }
    ESOS_TASK_END();
}
//__________________________________________________________________________________________________________________
