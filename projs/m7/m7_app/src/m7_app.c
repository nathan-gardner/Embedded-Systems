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

#define  THOUSANDS_PLACE       4
#define  HUNDREDS_PLACE        3
#define  TENS_PLACE            2
#define  ONES_PLACE            1
#define  NUM_DIGITS_IN_PERIOD  5

#define  ASCII_CHAR_LOWER_BOUND     32    // these are inclusive
#define  ASCII_CHAR_UPPER_BOUND     127

//************** I N C L U D E S *****************
#include "esos.h"
#include "esos_stm32l4_edub_4bit_lcd44780wo.h"
#include "esos_lcd44780wo.h"
#include "main.h"
#include "ptb.h"
#include "circularBuffer.h"
#include "keypad.h"
#include "user_app.h"
#include <stdio.h>
#include <ctype.h>
#include <math.h>

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
void __initLCDMenu( uint8_t u8_LED_num );
void shiftBufferLeft(uint8_t* buffer, uint8_t PushByte, uint8_t size);

//************** G L O B A L S *****************
/*
 * Format of LED select menu on 2x16 LCD should go:
 * ^LED1    1028 ms
 * vpress D to edit
 */
uint8_t* psz_SelectMenuLineOneDefault = "         #### ms";
uint8_t* psz_SelectMenuLineTwoDefault = " press D to edit";
#define  LD2_OPTION   0
#define  LED0_OPTION  1
#define  LED1_OPTION  2
#define  LED2_OPTION  3
#define  LED3_OPTION  4
#define  NUM_LEDS     5
uint8_t* apsz_LEDNames[NUM_LEDS] = {"LD2 ", "LED0", "LED1", "LED2", "LED3"};
uint8_t  u8_LCDCurrentState = LD2_OPTION;  // initialize to "lowest" LED
enum status status_menu = SELECTING_LED;
/* Circular buffer structures */
circular_buffer_t cbuff_recv;
circular_buffer_t cbuff_send;

/* Globals and defines used for Vigenere cipher functions */
uint8_t au8_key[] = "TENNESSEETECH"; // Global encryption key

//_____________________________________________
//Used in the encrypting text state
uint8_t u8_IncomingCharBuff[16] = "                ";
uint8_t u8_OutgoingCharBuff[16] = "                ";
BOOL b_IsNotSpecialChar;
//_____________________________________________

/*
 * u32_tim2_period initialization according to M3/M4 specifications.
 * Must be between 1 and 9999 ms.
 */

// Indexed array of LED periods [0] = LD2, [1] = LED0, ..., [4] = LED3
uint32_t au32_LEDPeriods[5] = {1000,1000,1000,1000,1000};

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
      sprintf((char*)au8_numeric_str_recv,"%d",(int)au32_LEDPeriods[0]);
    }
    else if(u8_specifier == 3) {                                                             // ELSE PROCESS EDUB-LED3 FREQ
      // LOGIC FOR SPECIFIC LED
      sprintf((char*)au8_numeric_str_recv,"%d",(int)au32_LEDPeriods[4]);
    }
    else if(u8_specifier == 2) {                                                             // ELSE PROCESS EDUB-LED2 FREQ
      // LOGIC FOR SPECIFIC LED
      sprintf((char*)au8_numeric_str_recv,"%d",(int)au32_LEDPeriods[3]);
    }
    else if(u8_specifier == 1) {                                                             // ELSE PROCESS EDUB-LED1 FREQ
      // LOGIC FOR SPECIFIC LED
      sprintf((char*)au8_numeric_str_recv,"%d",(int)au32_LEDPeriods[2]);
    }
    else if(u8_specifier == 0) {                                                             // ELSE PROCESS EDUB-LED0 FREQ
      // LOGIC FOR SPECIFIC LED
      sprintf((char*)au8_numeric_str_recv,"%d",(int)au32_LEDPeriods[1]);
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
                   au32_LEDPeriods[0] = u32_timerPeriodTemp;
                   if(u8_LCDCurrentState == 0) // update LCD period if currently displayed
                   {
                     __initLCDMenu(0);
                   }
                }
                else if(u8_specifier == 3){
                  esos_sui_flashLED(LED3, u32_timerPeriodTemp);
                  au32_LEDPeriods[4] = u32_timerPeriodTemp;
                   if(u8_LCDCurrentState == 4) // update LCD period if currently displayed
                   {
                     __initLCDMenu(4);
                   }
                }
                else if(u8_specifier == 2){
                    esos_sui_flashLED(LED2, u32_timerPeriodTemp);
                    au32_LEDPeriods[3] = u32_timerPeriodTemp;
                    if(u8_LCDCurrentState == 3) // update LCD period if currently displayed
                   {
                     __initLCDMenu(3);
                   }
                }
                else if(u8_specifier == 1){
                    esos_sui_flashLED(LED1, u32_timerPeriodTemp);
                    au32_LEDPeriods[2] = u32_timerPeriodTemp;
                   if(u8_LCDCurrentState == 2) // update LCD period if currently displayed
                   {
                     __initLCDMenu(2);
                   }
                }
                else if(u8_specifier == 0){
                   esos_sui_flashLED(LED0, u32_timerPeriodTemp);
                   au32_LEDPeriods[1] = u32_timerPeriodTemp;
                   if(u8_LCDCurrentState == 1) // update LCD period if currently displayed
                   {
                     __initLCDMenu(1);
                   }
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
                b_IsNotSpecialChar = (u8_byteToProcess >= ASCII_CHAR_LOWER_BOUND) && (u8_byteToProcess <= ASCII_CHAR_UPPER_BOUND);
                if(b_IsNotSpecialChar)
                {
                  shiftBufferLeft(u8_IncomingCharBuff, u8_byteToProcess - '0', 16);
                }
                if(!isalpha(u8_byteToProcess)) // don't encrypt special characters
                {
                  circular_buffer_queue(&cbuff_send, u8_byteToProcess);
                  if(b_IsNotSpecialChar)
                  {
                    shiftBufferLeft(u8_OutgoingCharBuff, u8_byteToProcess - '0', 16);
                  }
                }
                else // encrypt
                {
                  u8_byteToProcess = encrypt(u8_byteToProcess);             // if you want you can add if(isalphanum(u8_byteToProcess)){} and/or toupper(u8_byteToProcess) to only encrypt uppers
                  circular_buffer_queue(&cbuff_send, u8_byteToProcess);
                  if(b_IsNotSpecialChar)
                  {
                    shiftBufferLeft(u8_OutgoingCharBuff, u8_byteToProcess - '0', 16);
                  }
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
                  b_IsNotSpecialChar = (u8_byteToProcess >= ASCII_CHAR_LOWER_BOUND) && (u8_byteToProcess <= ASCII_CHAR_UPPER_BOUND);
                  if(b_IsNotSpecialChar)
                  {
                    shiftBufferLeft(u8_IncomingCharBuff, u8_byteToProcess - '0', 16);
                  }
                  if(!isalpha(u8_byteToProcess)) // don't decrypt special characters
                  {
                    circular_buffer_queue(&cbuff_send, u8_byteToProcess);
                    if(b_IsNotSpecialChar)
                    {
                      shiftBufferLeft(u8_OutgoingCharBuff, u8_byteToProcess - '0', 16);
                    }
                  }
                  else
                  {
                    currentPriority = decryption;
                    u8_byteToProcess = decrypt(u8_byteToProcess);
                    circular_buffer_queue(&cbuff_send, u8_byteToProcess);
                    if(b_IsNotSpecialChar)
                    {
                      shiftBufferLeft(u8_OutgoingCharBuff, u8_byteToProcess - '0', 16);
                    }
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
                  if(status_menu != SELECTING_LED)
                  {
                    status_menu = SELECTING_LED;
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

/*******************************************************************************
 * Function:         ESOS_USER_TASK(keypad_thread)
 *
 * PreCondition:     None
 *
 * Input:            None
 *
 * Output:           None
 *
 * Side Effects:     None
 *
 * Overview:         Maintains LCD functionality throughout program operation
 *                   Status menu options: SELECTING_LED, SETTING_LED_PERIOD,
 *                   and ENCRYPTING_TEXT
 *
 * Note:             None
 *
 ******************************************************************************/
ESOS_USER_TASK(keypad_thread)
{
    uint16_t u16_KeypadRead;
    uint8_t u16_lastUpdate; // stops from repeated updates
    static uint8_t au8_userBuffer[4] = {'_','_','_','_'};
    static uint8_t bufferpoint = 3;
    uint8_t base_2n;
    uint32_t u32_period = 0;
    uint32_t u32_PeriodDigits[NUM_DIGITS_IN_PERIOD] = {'0','0','0','0'};
    static ESOS_TASK_HANDLE edit_led;
    ESOS_TASK_BEGIN();
    while(TRUE)
    {
        u16_KeypadRead = read_keypad();
        /*
         * If keypad_read is none zero key is pressed and check all keys,
         * otherwhile yield this stucture purposefully takes key of highest
         * priority and ignores all other keys
         */
        if (status_menu == SELECTING_LED) {
            if(u16_KeypadRead & KEYAMASK) {
                // Ensures that LED3 remains the last possible setting
                u8_LCDCurrentState++;
                if(u8_LCDCurrentState > LED3_OPTION) {
                    u8_LCDCurrentState = LED3_OPTION;
                }

                /* Accounts for debounce */
                ESOS_TASK_WAIT_TICKS(250);
                ESOS_TASK_WAIT_UNTIL((u16_KeypadRead & KEYAMASK) == 0); // wait for key A to be released

                //--------------------THIS IS THE SAME AS __initLCDMenu(u8_LCDCurrentState)----------------------
                // Write the select menu to the screen with current LED option
                esos_lcd44780_writeString(0, 0, psz_SelectMenuLineOneDefault);
                if (u8_LCDCurrentState < LED3_OPTION) { // only print up arrow if there is an option > current
                    esos_lcd44780_writeChar(0, 0, '^');
                }
                esos_lcd44780_writeString(1, 0, psz_SelectMenuLineTwoDefault);
                esos_lcd44780_writeChar(1, 0, 'v');
                esos_lcd44780_writeString(0, 1, apsz_LEDNames[u8_LCDCurrentState]); // print the current LED name on LCD

                /* Stores the flash period as an array of digits */
                u32_period = au32_LEDPeriods[u8_LCDCurrentState];
                u32_PeriodDigits[THOUSANDS_PLACE] = u32_period / 1000;
                u32_PeriodDigits[HUNDREDS_PLACE]  = (u32_period / 100)
                                                      - (u32_PeriodDigits[THOUSANDS_PLACE] * 10);
                u32_PeriodDigits[TENS_PLACE]      =  (u32_period / 10)
                                                      - (u32_PeriodDigits[THOUSANDS_PLACE] * 100)
                                                      - (u32_PeriodDigits[HUNDREDS_PLACE] * 10);
                u32_PeriodDigits[ONES_PLACE]      =  u32_period
                                                      - (u32_PeriodDigits[THOUSANDS_PLACE] * 1000)
                                                      - (u32_PeriodDigits[HUNDREDS_PLACE] * 100)
                                                      - (u32_PeriodDigits[TENS_PLACE] * 10);

                // Write period to the display
                esos_lcd44780_writeChar(0, 9,  '0' + u32_PeriodDigits[THOUSANDS_PLACE]);
                esos_lcd44780_writeChar(0, 10, '0' + u32_PeriodDigits[HUNDREDS_PLACE]);
                esos_lcd44780_writeChar(0, 11, '0' + u32_PeriodDigits[TENS_PLACE]);
                esos_lcd44780_writeChar(0, 12, '0' + u32_PeriodDigits[ONES_PLACE]);
                //--------------------------------------------------------------------------------
            }
            else if(u16_KeypadRead & KEYBMASK) {
                /*
                 * Ensures that LD2 remains the first possible setting
                 * since "subtracting" from 0 (LD2) produces 255 (invalid)
                 */
                u8_LCDCurrentState--;
                if(u8_LCDCurrentState > LED3_OPTION) {
                    u8_LCDCurrentState = LD2_OPTION;
                }

                /* Accounts for debounce */
                ESOS_TASK_WAIT_TICKS(250);
                ESOS_TASK_WAIT_UNTIL((u16_KeypadRead & KEYBMASK) == 0);

                //------------------------THIS IS THE SAME AS __initLCDMenu(u8_LCDCurrentState)----------------------
                /* Write the select menu to the screen with current LED option */
                esos_lcd44780_writeString(0, 0, psz_SelectMenuLineOneDefault);
                if (u8_LCDCurrentState > LD2_OPTION) { // only print the down arrow is there is an option < current
                    esos_lcd44780_writeChar(1, 0, 'v');
                }
                esos_lcd44780_writeString(1, 0, psz_SelectMenuLineTwoDefault);
                esos_lcd44780_writeChar(0, 0, '^');
                esos_lcd44780_writeString(0, 1, apsz_LEDNames[u8_LCDCurrentState]);

                u32_period = au32_LEDPeriods[u8_LCDCurrentState];
                /* Stores the flash period as an array of digits */
                u32_PeriodDigits[THOUSANDS_PLACE] = u32_period / 1000;
                u32_PeriodDigits[HUNDREDS_PLACE] = (u32_period / 100)
                                                      - (u32_PeriodDigits[THOUSANDS_PLACE] * 10);
                u32_PeriodDigits[TENS_PLACE] =  (u32_period / 10)
                                                  - (u32_PeriodDigits[THOUSANDS_PLACE] * 100)
                                                  - (u32_PeriodDigits[HUNDREDS_PLACE] * 10);
                u32_PeriodDigits[ONES_PLACE] =  u32_period
                                                  - (u32_PeriodDigits[THOUSANDS_PLACE] * 1000)
                                                  - (u32_PeriodDigits[HUNDREDS_PLACE] * 100)
                                                  - (u32_PeriodDigits[TENS_PLACE] * 10);

                // Write period to the display
                esos_lcd44780_writeChar(0, 9, '0' + u32_PeriodDigits[THOUSANDS_PLACE]);
                esos_lcd44780_writeChar(0, 10, '0' + u32_PeriodDigits[HUNDREDS_PLACE]);
                esos_lcd44780_writeChar(0, 11, '0' + u32_PeriodDigits[TENS_PLACE]);
                esos_lcd44780_writeChar(0, 12, '0' + u32_PeriodDigits[ONES_PLACE]);
                //--------------------------------------------------------------------------------
            }
        }
        else if (status_menu == SETTING_LED_PERIOD)
        {
          ESOS_TASK_WAIT_UNTIL(!read_keypad()); // waiting for user to release D Key
          while(!(read_keypad()&KEYSTARMASK)) { // hold in state until the star key is pressed
              if(read_keypad()&ONLY_NUM_KEYPAD) {
                  base_2n = log(read_keypad())/(log(2));
                  shiftBufferLeft(au8_userBuffer,base_2n,4);
                  ESOS_TASK_WAIT_TICKS(250);
              }
              else {
                  esos_lcd44780_clearScreen();
                  esos_lcd44780_writeString(0, 1, apsz_LEDNames[u8_LCDCurrentState]);
                  esos_lcd44780_writeString(0,9,au8_userBuffer);
                  esos_lcd44780_writeString(0,13," ms");
                  esos_lcd44780_writeString(1,1,"Confirm with *");
              }
              ESOS_TASK_YIELD();
          }
          for(uint8_t i = 0; i < 4; i++)//change any underscores to char 0
          {
            if(au8_userBuffer[i]=='_')
              au8_userBuffer[i] = '0';
          }
          esos_sui_flashLED(u8_LCDCurrentState,atoi(au8_userBuffer));
          au32_LEDPeriods[u8_LCDCurrentState] = atoi(au8_userBuffer);
          au8_userBuffer[0]=au8_userBuffer[1]=au8_userBuffer[2]=au8_userBuffer[3]='_';
          __initLCDMenu(u8_LCDCurrentState);
          status_menu = SELECTING_LED; // once you complete change back to default state
        }
        else if (status_menu == ENCRYPTING_TEXT)
        {
          esos_lcd44780_clearScreen();
          while(esos_sui_isSWPressed(SW4) || esos_sui_isSWPressed(SW5)) // hold in state until SW4 or SW5 is released
          {
            ESOS_TASK_WAIT_TICKS(250); // "refresh rate" of LCD. Human eye cannot recognize updates much more frequent than 250 ms.
            // update LCD according to what is in the incoming and outdoing buffer
            esos_lcd44780_writeString(0,0,u8_IncomingCharBuff);
            esos_lcd44780_writeString(1,0,u8_OutgoingCharBuff);
            ESOS_TASK_YIELD();
          }
          __initLCDMenu(u8_LCDCurrentState); // rewrite menu on lcd
          for(int i = 0; i < 16; i++) // clear character buffers for the next entrance into this state
          {
            u8_IncomingCharBuff[i] = ' ';
            u8_OutgoingCharBuff[i] = ' ';
          }
          status_menu = SELECTING_LED; // once you complete change back to default state
        }

        if(u16_KeypadRead & KEYDMASK) { // set SETTING_LED_PERIOD state if key D is pressed
          status_menu = SETTING_LED_PERIOD;
        }
        if(esos_sui_isSWPressed(SW4) || esos_sui_isSWPressed(SW5))
        {
          status_menu = ENCRYPTING_TEXT;
        }
      ESOS_TASK_YIELD();
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

  esos_lcd44780_configDisplay();
  __esos_lcd44780_init();
  esos_lcd44780_clearScreen();
  __initLCDMenu(u8_LCDCurrentState); // initialize LCD character display initialize to LD2
  //writeLCDPeriod(0); // send the period of [0] = LD2 to the display
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

  esos_sui_flashLED(LD2, au32_LEDPeriods[0]);
  esos_sui_flashLED(LED0, au32_LEDPeriods[1]);
  esos_sui_flashLED(LED1, au32_LEDPeriods[2]);
  esos_sui_flashLED(LED2, au32_LEDPeriods[3]);
  esos_sui_flashLED(LED3, au32_LEDPeriods[4]);


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
  esos_RegisterTask(keypad_thread);
  __esos_InitSUI();

} // end user_init()

/******************************************************************************
 * Function:        void __initLCDMenu(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The __initLCDMenu() function is called in the hardware setup and
 *                  puts the LCD is its initial state for program initialization.
 *
 * Note:            None  
 * 
 *****************************************************************************/

void __initLCDMenu( uint8_t u8_LED_num )
{
  // Write the initial menu to the screen
  esos_lcd44780_writeString(0,0,psz_SelectMenuLineOneDefault);
  esos_lcd44780_writeChar(0,0,'^');
  esos_lcd44780_writeString(1,0,psz_SelectMenuLineTwoDefault);

  // Write LED name to the display
  esos_lcd44780_writeString(0, 1, apsz_LEDNames[u8_LED_num]);

  uint32_t u32_period = au32_LEDPeriods[u8_LED_num];
  /* Stores the flash period as an array of digits */
  uint32_t u32_PeriodDigits[NUM_LEDS];
  u32_PeriodDigits[THOUSANDS_PLACE] = u32_period / 1000;
  u32_PeriodDigits[HUNDREDS_PLACE]  = (u32_period / 100)
                                                - (u32_PeriodDigits[THOUSANDS_PLACE] * 10);
  u32_PeriodDigits[TENS_PLACE]      =  (u32_period / 10)
                                                - (u32_PeriodDigits[THOUSANDS_PLACE] * 100)
                                                - (u32_PeriodDigits[HUNDREDS_PLACE] * 10);
  u32_PeriodDigits[ONES_PLACE]      =  u32_period
                                                - (u32_PeriodDigits[THOUSANDS_PLACE] * 1000)
                                                - (u32_PeriodDigits[HUNDREDS_PLACE] * 100)
                                                - (u32_PeriodDigits[TENS_PLACE] * 10);

  // Write period to the display
  esos_lcd44780_writeChar(0, 9,  '0' + u32_PeriodDigits[THOUSANDS_PLACE]);
  esos_lcd44780_writeChar(0, 10, '0' + u32_PeriodDigits[HUNDREDS_PLACE]);
  esos_lcd44780_writeChar(0, 11, '0' + u32_PeriodDigits[TENS_PLACE]);
  esos_lcd44780_writeChar(0, 12, '0' + u32_PeriodDigits[ONES_PLACE]);
}

/******************************************************************************
 * Function:        void shiftBufferLeft(void)
 *
 * PreCondition:    None
 *
 * Input:           Pointer to the array to be shifted, data unsigned 8 bit data
 *                  to be pushed, and the size of the array.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The shiftBufferLeft(uint8_t* buffer, uint8_t PushByte, uint8_t size) function
 *                  is used to shift arrays throughout program operation. 
 *
 * Note:            None  
 * 
 *****************************************************************************/

void shiftBufferLeft(uint8_t* buffer, uint8_t PushByte, uint8_t size)
{
  for(uint8_t i = 0; i < size-1; i++)
  {
    buffer[i] = buffer[i+1];
  }

  buffer[size-1] = PushByte+'0';


}
