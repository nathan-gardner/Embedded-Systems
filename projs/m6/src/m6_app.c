/*******************************************************************************
 *
 *                ECE 4140 Milestone 6 Firmware Version 1.1
 *
 *******************************************************************************
 * FileName:       m6_app.c
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
 * Blake Martin          04/04/22                 System Architect
 * Nathan Gardner        04/04/22                 Original
 * Triston Whitescarver  04/04/22                 Original
 * Kester Nucum          04/04/22                 Added comments and documentation
 *********************************************************************************/

//************** I N C L U D E S *****************
#include "esos.h"
#include "main.h"
#include "ptb.h"
#include "circularBuffer.h"
#include "keypad.h"
#include "user_app.h"
#include <stdio.h>
#include <ctype.h>

//______________________________________________
// HARDWARE INDEPENDENCE LIBS:
#include "esos_stm32l4_edub_sui.h"
#include "esos_stm32l4.h"
#include "esos_sui.h"
//_____________________________________________

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
uint8_t au8_key[] = "TENNESSEETECH"; // Global encryption key
/*
 * u32_tim2_period initialization according to M3/M4 specifications.
 * Must be between 1 and 9999 ms.
 */
uint32_t nucleo_LD2_period = 1000;
uint32_t edub_LED0_period = 1000;
uint32_t edub_LED1_period = 1000;
uint32_t edub_LED2_period = 1000;
uint32_t edub_LED3_period = 1000;

ESOS_SUI_SWITCH_HANDLE B1;
ESOS_SUI_SWITCH_HANDLE SW2;
ESOS_SUI_SWITCH_HANDLE SW3;
ESOS_SUI_SWITCH_HANDLE SW4;
ESOS_SUI_SWITCH_HANDLE SW5;

ESOS_SUI_LED_HANDLE LD2;
ESOS_SUI_LED_HANDLE LED0;
ESOS_SUI_LED_HANDLE LED1;
ESOS_SUI_LED_HANDLE LED2;
ESOS_SUI_LED_HANDLE LED3;


//________________________________________________________________________________________
// LINUX
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
//_________________________________________________________________

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
 * PreCondition:     Button B1 pushed
 *
 * Input:            Command byte
 *
 * Output:           None
 *
 * Side Effects:     None
 *
 * Overview:         Allows user to display, set current LEDx period
 *
 * Note:             None
 *
 ******************************************************************************/
ESOS_CHILD_TASK(cmd_child_task, uint8_t cmdByte)
{
  // We should likely make these non-static, and simply call this on each iteration a single time.
  // This child serves as the decision make for command bytes
  static uint8_t i = 0;
  static uint8_t u8_specifier = 0;
  static uint8_t au8_numeric_str_recv[4];
  static uint32_t u32_timerPeriodTemp;
  static uint8_t tempChar = 0;
  static uint16_t u16_maskbits = 0;
  ESOS_TASK_BEGIN();
  if(cmdByte == 'L'){       // Echo the current timer period via usart for specified LED
		ESOS_TASK_WAIT_UNTIL(circular_buffer_dequeue(&cbuff_recv, &u8_specifier));
    u8_specifier = u8_specifier - 0x30;
    if(u8_specifier == 4) {                                                                  // else process NUCLEO-LD2 FREQ
      // LOGIC FOR SPECIFIC LED
      sprintf((char*)au8_numeric_str_recv,"%d",(int)nucleo_LD2_period);
    }
    else if(u8_specifier == 3) {                                                             // ELSE PROCESS EDUB-LED3 FREQ
      // LOGIC FOR SPECIFIC LED
      sprintf((char*)au8_numeric_str_recv,"%d",(int)edub_LED3_period);
    }
    else if(u8_specifier == 2) {                                                             // ELSE PROCESS EDUB-LED2 FREQ
      // LOGIC FOR SPECIFIC LED
      sprintf((char*)au8_numeric_str_recv,"%d",(int)edub_LED2_period);
    }
    else if(u8_specifier == 1) {                                                             // ELSE PROCESS EDUB-LED1 FREQ
      // LOGIC FOR SPECIFIC LED
      sprintf((char*)au8_numeric_str_recv,"%d",(int)edub_LED1_period);
    }
    else if(u8_specifier == 0) {                                                             // ELSE PROCESS EDUB-LED0 FREQ
      // LOGIC FOR SPECIFIC LED
      sprintf((char*)au8_numeric_str_recv,"%d",(int)edub_LED0_period);
    }
    if(u8_specifier <= 4){
      while(i < strlen(au8_numeric_str_recv)){                          // Now send each byte individually
        circular_buffer_queue(&cbuff_send, au8_numeric_str_recv[i]);
        i++;
        ESOS_TASK_YIELD();
      }
    }
      else{
          ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
          ESOS_TASK_WAIT_ON_SEND_STRING("INVALID FREQ ");
          ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
      }
  }
  else if(cmdByte == 'S'){  // Recv new timer period via usart
      ESOS_TASK_WAIT_UNTIL((circular_buffer_dequeue(&cbuff_recv, &u8_specifier)));
      u8_specifier = u8_specifier - 48;

        ESOS_TASK_WAIT_UNTIL((circular_buffer_num_items(&cbuff_recv) >= 4) || (!NUCLEO_BUTTON_PUSHED()));
        if(!NUCLEO_BUTTON_PUSHED()) {
            break;                                                  // If the reason for breaking yield was button release, then we are done here.
        }
        else{                                                                   // else if the reason for breaking yield is sufficient data then process it
            circular_buffer_dequeue_arr(&cbuff_recv, au8_numeric_str_recv, 4);  // so, get the 4 bytes we need
            u32_timerPeriodTemp = a2i(au8_numeric_str_recv, 10);                    // then convert the 4 ascii bytes to uint32_t for use as new timer period
            if(u32_timerPeriodTemp <= 9999 && u32_timerPeriodTemp >= 0){
                if(u8_specifier == 4){
                   esos_sui_flashLED(LD2, u32_timerPeriodTemp);
                }
                else if(u8_specifier == 3){
                  esos_sui_flashLED(LED3, u32_timerPeriodTemp);
                }
                else if(u8_specifier == 2){
                    esos_sui_flashLED(LED2, u32_timerPeriodTemp);
                }
                else if(u8_specifier == 1){
                    esos_sui_flashLED(LED1, u32_timerPeriodTemp);
                }
                else if(u8_specifier == 0){
                   esos_sui_flashLED(LED0, u32_timerPeriodTemp);
                }
            }
            else{
                ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
                ESOS_TASK_WAIT_ON_SEND_STRING("INVALID FREQ ");
                ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
            }
        }
  }
  else if(cmdByte == 'R'){
        ESOS_TASK_WAIT_UNTIL((circular_buffer_num_items(&cbuff_recv) >= 4));

        ESOS_TASK_WAIT_UNTIL(circular_buffer_dequeue_arr(&cbuff_recv, au8_numeric_str_recv, 4));
        u16_maskbits = a2i(au8_numeric_str_recv, 16);
        uint16_t u16_ReadVal = 0x0000;

        uint8_t u8_buffer[4];
        u16_ReadVal = read_keypad() & u16_maskbits;
        itoa(u16_ReadVal, u8_buffer, 16);
        circular_buffer_queue(&cbuff_send, u8_buffer[0]);
        circular_buffer_queue(&cbuff_send, u8_buffer[1]);
        circular_buffer_queue(&cbuff_send, u8_buffer[2]);
        circular_buffer_queue(&cbuff_send, u8_buffer[3]);
  }
  i = 0;
  *au8_numeric_str_recv = NULL;
  u32_timerPeriodTemp = 0;
  tempChar = 0;
  ESOS_TASK_END();
}


//_______________________________________________________________________________
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
  static uint8_t u8_byteToProcess = 0;
  static uint8_t cryptedByte = 0;
  static ESOS_TASK_HANDLE cmd_child;                // accept only keyboard commands for B1
  static systemPriority currentPriority = echo;
  ESOS_TASK_BEGIN();
  while(TRUE)
  {
          if(circular_buffer_dequeue(&cbuff_recv, &u8_byteToProcess)){                           // Only process control signals & data when data becomes available
              //_________________________________________________________________________________________
              // PRIORTY 1 CHECK: IS BUTTTON PUSHED? -> THEN ENTER COMMAND RECV STATE
              if(!esos_sui_isSWPressed(B1)){
                    currentPriority = command;
                    ESOS_ALLOCATE_CHILD_TASK(cmd_child);                                       //  Spawn child task and wait for button to be released
                    ESOS_TASK_SPAWN_AND_WAIT(cmd_child, cmd_child_task, u8_byteToProcess);     //
                    ESOS_TASK_YIELD();
              }
              //_____________________________________________________________________________________________
              // PRIORITY 2 CHECK: IS SW2 PUSHED? -> THEN REMOVE non-alphaNum processing
              else if(esos_sui_isSWPressed(SW2)){
                  currentPriority = alphaNum;
                  if(isAlpha(u8_byteToProcess)){
                      circular_buffer_queue(&cbuff_send, u8_byteToProcess);
                  }
                  ESOS_TASK_YIELD();
                  // remove non-alphanumerics
              }
              //_____________________________________________________________________________________________
              // PRIORITY 3: TOUPPER()
              else if(esos_sui_isSWPressed(SW3)){
                  currentPriority = toUpper;
                  u8_byteToProcess = toupper(u8_byteToProcess);        // ctype.h function for converting lowercase to upper
                  circular_buffer_queue(&cbuff_send, u8_byteToProcess);
                  ESOS_TASK_YIELD();
              }
              //_____________________________________________________________________________________________
              // PRIORITY 4: ENCYPTION
              else if(esos_sui_isSWPressed(SW4)){
                if(currentPriority != encryption){
                  resetKey();
                }
                currentPriority = encryption;
                u8_byteToProcess = toupper(u8_byteToProcess);
                if(!isalpha(u8_byteToProcess)) // don't encrypt special characters
                {
                  circular_buffer_queue(&cbuff_send, u8_byteToProcess);
                }
                else // encrypt
                {
                  u8_byteToProcess = encrypt(u8_byteToProcess);             // if you want you can add if(isalphanum(u8_byteToProcess)){} and/or toupper(u8_byteToProcess) to only encrypt uppers
                  circular_buffer_queue(&cbuff_send, u8_byteToProcess);
                }
                ESOS_TASK_YIELD();
              }
              //_____________________________________________________________________________________________
              // PRIORIRTY 5: DECRYPTION
              else if(esos_sui_isSWPressed(SW5)){
                  if(currentPriority != decryption){
                    resetKey();
                  }
                  u8_byteToProcess = toupper(u8_byteToProcess);
                  if(!isalpha(u8_byteToProcess)) // don't decrypt special characters
                  {
                    circular_buffer_queue(&cbuff_send, u8_byteToProcess);
                  }
                  else
                  {
                    currentPriority = decryption;
                    u8_byteToProcess = decrypt(u8_byteToProcess);
                    circular_buffer_queue(&cbuff_send, u8_byteToProcess);
                  }
                  ESOS_TASK_YIELD();
              }
              //_____________________________________________________________________________________________
              // PRIORITY 6: ECHO
              else{
                	if(currentPriority != echo){
                    	resetKey();
                    	currentPriority = echo;
                	}
                	circular_buffer_queue(&cbuff_send, u8_byteToProcess);
                	currentPriority = echo;
                  ESOS_TASK_YIELD();
              }
              //_____________________________________________________________________________________________
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

  //_______________________________________________________________________________________________________
  // REGISTER HW-INDEPENDENT SWITCHES:
  B1 = esos_sui_registerSwitch(NUCLEO_BUTTON_PORT, NUCLEO_BUTTON_PIN);
  SW2 = esos_sui_registerSwitch(EDUB_SW2_PORT, EDUB_SW2_PIN);
  SW3 = esos_sui_registerSwitch(EDUB_SW3_PORT, EDUB_SW3_PIN);
  SW4 = esos_sui_registerSwitch(EDUB_SW4_PORT, EDUB_SW4_PIN);
  SW5 = esos_sui_registerSwitch(EDUB_SW5_PORT, EDUB_SW5_PIN);
  //__________________________________________________________________________________________________________________

  LD2 = esos_sui_registerLED(LED2_PORT,LED2_PIN);
  LED0 = esos_sui_registerLED(EDUB_LED0_PORT, EDUB_LED0_PIN);
  LED1 = esos_sui_registerLED(EDUB_LED1_PORT, EDUB_LED1_PIN);
  LED2 = esos_sui_registerLED(EDUB_LED2_PORT,EDUB_LED2_PIN);
  LED3 = esos_sui_registerLED(EDUB_LED3_PORT, EDUB_LED3_PIN);


  /*esos_hw_sui_changeLEDTimer(LD2, nucleo_LD2_period);
  esos_hw_sui_changeLEDTimer(LED0, edub_LED0_period);
  esos_hw_sui_changeLEDTimer(LED1, edub_LED1_period);
  esos_hw_sui_changeLEDTimer(LED2, edub_LED2_period);
  esos_hw_sui_changeLEDTimer(LED3, edub_LED0_period);
 
  esos_hw_sui_makeFlash(LD2);
  esos_hw_sui_makeFlash(LED0);
  esos_hw_sui_makeFlash(LED1);
  esos_hw_sui_makeFlash(LED2);
  esos_hw_sui_makeFlash(LED3);
  */
 esos_sui_flashLED(LD2, nucleo_LD2_period);
 esos_sui_flashLED(LED0, edub_LED0_period);
 esos_sui_flashLED(LED1, edub_LED1_period);
 esos_sui_flashLED(LED2, edub_LED2_period);
 esos_sui_flashLED(LED3, edub_LED0_period);


//____________________________________________________________________________________________________________________

  EDUB_KEYPAD_COL0_SETUP();
  EDUB_KEYPAD_COL1_SETUP();
  EDUB_KEYPAD_COL2_SETUP();
  EDUB_KEYPAD_COL3_SETUP();
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
  /* Sets up software structures needed for program execution */
  circular_buffer_init(&cbuff_recv);
  circular_buffer_init(&cbuff_send);

  /* Initializes hardware */
  __user_init_hw();

  /* Drive the UART directly to print the HELLO_MSG */
  __esos_unsafe_PutString(HELLO_MSG);


  esos_RegisterTask(send_thread);
  esos_RegisterTask(recv_thread);
  esos_RegisterTask(dataProcessing_thread);
  __esos_InitSUI();

} // end user_init()
