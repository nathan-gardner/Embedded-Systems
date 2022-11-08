/*******************************************************************************
 *
 *                ECE 4140 Milestone 4 Firmware Version 1.1
 *
 *******************************************************************************
 * FileName:       m4_app.c
 * Dependencies:   See INCLUDES section below
 * Processor:      STM32 Arm Cortex-M4
 * Compiler:       GCC 10.3-2021.10
 * Company:        Tennessee Tech ECE
 *
 * Software License Agreement
 *
 * The software supplied herewith by Tennessee Tech University.
 * The software is owned by Tennessee Tech University, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS-IS" CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO,
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE APPLY TO THIS SOFTWARE. TENNESSEE TECH UNIVERSITY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR CONSEQUENTIAL
 * DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * MODULE FUNCTION:
 * This module will encode and decode text sent via the USART2
 * on the NUCLEO-L452RE using a Vigenere cipher.
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Author                Date                  Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Blake Martin          3/6/22                Original
 * Nathan Gardner        3/6/22                Original
 * Triston Whitescarver  3/6/22                Original
 * Kester Nucum          3/20/22               Added comments and documentation
 ******************************************************************************/

//************** I N C L U D E S *****************
#include "esos.h"
#include "main.h"
#include "circularBuffer.h"
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

//******* S U P P O R T    F U N C T I O N    P R O T O T Y P E S ************
void __user_init_hw(void);

//************** G L O B A L S *****************
/* Circular buffer structures */
circular_buffer_t cbuff_recv;
circular_buffer_t cbuff_send;

/* Globals and defines used for Vigenere cipher functions */
uint8_t au8_key[] = "TENNESSEETECH";    // Global encryption key
bool b_currentButtonState = false;      // Global Current State of button B1
/*
 * u32_tim2_period initialization according to M3/M4 specifications.
 * Must be between 1 and 9999 ms.
 */
uint32_t u32_tim2_period = 1000;

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

/*******************************************************************************
 * Function:         ESOS_USER_TASK(flash_LED)
 *
 * PreCondition:     None
 *
 * Input:            None
 *
 * Output:           None
 *
 * Side Effects:     None
 *
 * Overview:         Toggles LED2 on and off with a period determined by
 *                   u32_tim2_period with a 50% duty cycle
 *
 * Note:             None
 *
 ******************************************************************************/
ESOS_USER_TASK(flash_LED)
{
  ESOS_TASK_BEGIN();
  while (TRUE)
  {
    gpio_toggle(LED2_PORT, LED2_PIN);
    ESOS_TASK_WAIT_TICKS(u32_tim2_period/2);
  } // endof while(TRUE)
  ESOS_TASK_END();
} // end flash_LED()

/*******************************************************************************
 * Function:         ESOS_USER_TASK(recv_thread)
 *
 * PreCondition:     None
 *
 * Input:            None
 *
 * Output:           None
 *
 * Side Effects:     None
 *
 * Overview:         Reads in characters from USART2
 *
 * Note:             None
 *
 ******************************************************************************/
ESOS_USER_TASK(recv_thread)
{
  static uint8_t u8_incoming;

  ESOS_TASK_BEGIN();
  while (TRUE)
  {
    ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
    ESOS_TASK_WAIT_ON_GET_UINT8(u8_incoming);
    circular_buffer_queue(&cbuff_recv, u8_incoming);
    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
    ESOS_TASK_YIELD();
  } // endof while(TRUE)
  ESOS_TASK_END();
} // end recv_thread()

/*******************************************************************************
 * Function:         ESOS_USER_TASK(send_thread)
 *
 * PreCondition:     None
 *
 * Input:            None
 *
 * Output:           None
 *
 * Side Effects:     None
 *
 * Overview:         Sends out characters to USART2
 *
 * Note:             None
 *
 ******************************************************************************/
ESOS_USER_TASK(send_thread)
{
  static uint8_t u8_outgoing;

  ESOS_TASK_BEGIN();
  while (TRUE)
  {
    if(circular_buffer_dequeue(&cbuff_send, &u8_outgoing)){
      ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
      ESOS_TASK_WAIT_ON_SEND_UINT8(u8_outgoing);
      ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
      ESOS_TASK_YIELD();
    }
    else{
      ESOS_TASK_YIELD();
    }

  } // endof while(TRUE)
  ESOS_TASK_END();
} // end recv_thread()

/*******************************************************************************
 * Function:         ESOS_CHILD_TASK(cmd_child_task, uint8_t cmdByte)
 *
 * PreCondition:     None
 *
 * Input:            None
 *
 * Output:           None
 *
 * Side Effects:     None
 *
 * Overview:         Allows user to display or set current timer period
 *
 * Note:             None
 *
 ******************************************************************************/
ESOS_CHILD_TASK(cmd_child_task, uint8_t cmdByte)
{
  static uint8_t i = 0;
  static uint8_t au8_numeric_str_recv[4];
  static uint32_t u32_timerPeriodTemp;
  static uint8_t tempChar = 0;
  ESOS_TASK_BEGIN();
  if(cmdByte == 'L'){       // Echo the current timer period via usart
      sprintf((char*)au8_numeric_str_recv,"%d",(int)u32_tim2_period);
      while(i < strlen(au8_numeric_str_recv)){
        circular_buffer_queue(&cbuff_send, au8_numeric_str_recv[i]);
        i++;
      }
  }
  else if(cmdByte == 'S'){  // Recv new timer period via usart
      ESOS_TASK_WAIT_UNTIL(circular_buffer_num_items(&cbuff_recv) >= 4);
      circular_buffer_dequeue_arr(&cbuff_recv, au8_numeric_str_recv, 4);

      // Now convert string to integer:
      u32_timerPeriodTemp = a2i(au8_numeric_str_recv);
      if(u32_timerPeriodTemp > 9999 || u32_timerPeriodTemp <= 0){
        u32_tim2_period = 250;
      }
      else{
        u32_tim2_period = u32_timerPeriodTemp;
      }
  }
  i = 0;
  *au8_numeric_str_recv = NULL;
  u32_timerPeriodTemp = 0;
  tempChar = 0;
  ESOS_TASK_END();
}

/*******************************************************************************
 * Function:         ESOS_USER_TASK(dataProcessing_thread)
 *
 * PreCondition:     None
 *
 * Input:            None
 *
 * Output:           None
 *
 * Side Effects:     None
 *
 * Overview:         Encrypts/decrypts data between RX and TX buffers
 *
 * Note:             None
 *
 ******************************************************************************/
ESOS_USER_TASK(dataProcessing_thread)
{
  static uint8_t byteToProcess = 0;
  static uint8_t cryptedByte = 0;
  static uint8_t cmdCount = 0;
  static ESOS_TASK_HANDLE cmd_child;
  ESOS_TASK_BEGIN();
  while(TRUE)
  {
    if(circular_buffer_dequeue(&cbuff_recv, &byteToProcess)){
      if(cmdCount == 3){
          ESOS_ALLOCATE_CHILD_TASK(cmd_child);
          ESOS_TASK_SPAWN_AND_WAIT(cmd_child, cmd_child_task, byteToProcess);
          cmdCount = 0;
        }
      else{
          if(byteToProcess != '!'){
              if((b_currentButtonState == false && !BUTTON_PUSHED())
                   || (b_currentButtonState == true && BUTTON_PUSHED())){
                b_currentButtonState = !b_currentButtonState;
                resetKey();
              }
              if((byteToProcess <= 'z') && (byteToProcess >= 'A')){
                cryptedByte = encypher(byteToProcess);
                circular_buffer_queue(&cbuff_send, cryptedByte);
                ESOS_TASK_YIELD();
              }
          }
          else{
            cmdCount++;
            // deal with commands
            ESOS_TASK_YIELD();
          }
      }
    }
    else{
      ESOS_TASK_YIELD();
    }

  }
  ESOS_TASK_END();
}


/************************************************************************
 * User supplied functions
 ************************************************************************/

/*******************************************************************************
 * Function:         __user_init_hw(void)
 *
 * PreCondition:     None
 *
 * Input:            None
 *
 * Output:           None
 *
 * Side Effects:     None
 *
 * Overview:         Initializes the hardware used in this application
 *
 * Note:             None
 *
 ******************************************************************************/
void __user_init_hw(void)
{
  /* Enables the GPIO clocks for LED2 and BUTTON on the L452RE */
  rcc_periph_clock_enable(LED2_PORT_RCC);
  rcc_periph_clock_enable(BUTTON_PORT_RCC);

  /* Enables the clock and input mode for button */
  gpio_mode_setup(BUTTON_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, BUTTON_PIN);

  /*
   * Sets LED2 GPIO port/pin to "output push/pull" mode,
   * and initializes LED2 to be off upon reset
   */
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

  /* Sets up software structures needed for program execution */
  circular_buffer_init(&cbuff_recv);
  circular_buffer_init(&cbuff_send);

  /* Initializes hardware */
  __user_init_hw();

  /* Drive the UART directly to print the HELLO_MSG */
  __esos_unsafe_PutString(HELLO_MSG);

  /* Registers tasks to be executed by ESOS scheduler */
  esos_RegisterTask(flash_LED);
  esos_RegisterTask(send_thread);
  esos_RegisterTask(recv_thread);
  esos_RegisterTask(dataProcessing_thread);

} // end user_init()
