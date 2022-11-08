/*
 *******************************************************************************
 *
 *           ECE 4140 Milestone 7 Test App Firmware Version 1.1
 *
 *******************************************************************************
 * FileName:       m7_test.c
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
 * This module will test out the ESOS LCD display functions.
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Author                Date                  Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Kester Nucum          04/12/22              Original
 *******************************************************************************
 *
 */

/** \file
 * \brief Test application for the ESOS LCD services
 *
 */

/*******************************************************************************
 * m7_test.c
 *******************************************************************************
 * Main source code for LCD testing application functions
 */

//*** I N C L U D E S *********************************************************/
#include "esos.h"
#include "esos_stm32l4_edub_4bit_lcd44780wo.h"
#include "esos_lcd44780wo.h"
#include "ptb.h"
#include "user_app.h"
#include <stdio.h>
#include <ctype.h>
#include <math.h>

// HARDWARE INDEPENDENCE LIBS:
#include "esos_stm32l4_edub_sui.h"
#include "esos_stm32l4.h"
#include "esos_sui.h"

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

//*** G L O B A L S ***********************************************************/
#define  LD2_PERIOD     0
#define  LED0_PERIOD    1
#define  LED1_PERIOD    2
#define  LED2_PERIOD    3
#define  LED3_PERIOD    4
#define  NUM_LEDS       5
uint32_t au32_LEDPeriods_Ticks[NUM_LEDS] = {1000,1000,1000,1000,1000};
ESOS_SUI_LED_HANDLE LD2;
ESOS_SUI_LED_HANDLE LED0;
ESOS_SUI_LED_HANDLE LED1;
ESOS_SUI_LED_HANDLE LED2;
ESOS_SUI_LED_HANDLE LED3;

/*
Tests all of the possible ESOS LCD service functions that can
be performed on the Hitachi HD44780 LCD Screen on the EduBase-V2-L452 PTB
 */

ESOS_USER_TASK(test_thread){
    uint8_t au8_String1[] = {'T','e','s','t','i','n','g',' ','d','o','n','t'};
    uint8_t au8_String2[] = {'w','r','i','t','e','B','u','f','f','e','r',' ',
                             'p','r','i','n','t'};
    char* psz_String3  = "Testing";
    char* psz_String4  = "writeString";
    char* psz_String5  = "Cursor blink";
    char* psz_String6  = "No blink";
    char* psz_String7  = "Cursor off";
    char* psz_String8  = "Does not print";
    char* psz_String9  = "Display Visible";

    ESOS_TASK_BEGIN();
    while(TRUE) {
        // Tests clearScreen()
        esos_lcd44780_clearScreen();
        esos_lcd44780_setCursorDisplay(false);
        ESOS_TASK_WAIT_TICKS(2000);

        // Tests writeChar()
        esos_lcd44780_writeChar(0, 0, 'T');
        esos_lcd44780_writeChar(0, 1, 'e');
        esos_lcd44780_writeChar(0, 2, 's');
        esos_lcd44780_writeChar(0, 3, 't');
        esos_lcd44780_writeChar(0, 4, 'i');
        esos_lcd44780_writeChar(0, 5, 'n');
        esos_lcd44780_writeChar(0, 6, 'g');
        esos_lcd44780_writeChar(1, 0, 'w');
        esos_lcd44780_writeChar(1, 1, 'r');
        esos_lcd44780_writeChar(1, 2, 'i');
        esos_lcd44780_writeChar(1, 3, 't');
        esos_lcd44780_writeChar(1, 4, 'e');
        esos_lcd44780_writeChar(1, 5, 'C');
        esos_lcd44780_writeChar(1, 6, 'h');
        esos_lcd44780_writeChar(1, 7, 'a');
        esos_lcd44780_writeChar(1, 8, 'r');
        ESOS_TASK_WAIT_TICKS(4000);

        // Tests writeBuffer()
        // Note: writeBuffer seems to cause alternating characters to switch rows
        esos_lcd44780_clearScreen();
        esos_lcd44780_writeBuffer(0, 2, au8_String1, 7);
        esos_lcd44780_writeBuffer(1, 0, au8_String2, 11);
        ESOS_TASK_WAIT_TICKS(4000);

        // Tests writeString()
        esos_lcd44780_clearScreen();
        esos_lcd44780_writeString(0, 6, psz_String3);
        esos_lcd44780_writeString(1, 4, psz_String4);
        ESOS_TASK_WAIT_TICKS(4000);

        // Tests cursorDisplay() and cursorBlink()
        // Rotates through blinking cursor, non-blinking cursor, and no cursor
        esos_lcd44780_clearScreen();
        esos_lcd44780_setCursorDisplay(true);
        esos_lcd44780_setCursorBlink(true);
        esos_lcd44780_setCursorHome();
        esos_lcd44780_writeString(1, 0, psz_String5);
        ESOS_TASK_WAIT_TICKS(4000);
        esos_lcd44780_clearScreen();
        esos_lcd44780_setCursorBlink(false);
        esos_lcd44780_writeString(1, 0, psz_String6);
        ESOS_TASK_WAIT_TICKS(4000);
        esos_lcd44780_setCursorDisplay(false);
        esos_lcd44780_writeString(1, 0, psz_String7);
        ESOS_TASK_WAIT_TICKS(4000);

        // Tests setDisplayVisible()
        // Should not output anything on the LCD screen at first
        // Then turns the display back on
        esos_lcd44780_clearScreen();
        esos_lcd44780_setDisplayVisible(false);
        esos_lcd44780_writeString(0, 0, psz_String8);
        ESOS_TASK_WAIT_TICKS(4000);
        esos_lcd44780_setDisplayVisible(true);
        esos_lcd44780_clearScreen();
        esos_lcd44780_writeString(0, 0, psz_String9);
        ESOS_TASK_WAIT_TICKS(4000);
    }
    ESOS_TASK_END();
}

/*
Initializes the hardware used in this application
 */
void __user_init_hw(void)
{

  esos_lcd44780_configDisplay();
  __esos_lcd44780_init();
  esos_lcd44780_clearScreen();

  LD2 = esos_sui_registerLED(LED2_PORT,LED2_PIN);
  LED0 = esos_sui_registerLED(EDUB_LED0_PORT, EDUB_LED0_PIN);
  LED1 = esos_sui_registerLED(EDUB_LED1_PORT, EDUB_LED1_PIN);
  LED2 = esos_sui_registerLED(EDUB_LED2_PORT,EDUB_LED2_PIN);
  LED3 = esos_sui_registerLED(EDUB_LED3_PORT, EDUB_LED3_PIN);

  esos_sui_flashLED(LD2, au32_LEDPeriods_Ticks[LD2_PERIOD]);
  esos_sui_flashLED(LED0, au32_LEDPeriods_Ticks[LED0_PERIOD]);
  esos_sui_flashLED(LED1, au32_LEDPeriods_Ticks[LED1_PERIOD]);
  esos_sui_flashLED(LED2, au32_LEDPeriods_Ticks[LED2_PERIOD]);
  esos_sui_flashLED(LED3, au32_LEDPeriods_Ticks[LED3_PERIOD]);
}

/*
user_init is a centralized initialization routine where
the user can setup their application. It is called
automagically by ES_OS during the operating system
initialization.
/note The user should set up any state machines and init
all application variables. They can also turn on any needed peripherals here.
\note The user SHALL NOT mess with the interrupt hardware directly!!!
The ES_OS must be aware of the interrupts and provides osXXXXXXX functions
for the user to use. Using these ES_OS-provided functions, the user may
(and probably should) initialize, register, and enable interrupts in this routine.
\note Furthermore, the user should register AT LEAST one user application
task here (via esos_RegisterTask) or the ES_OS scheduler will have nothing
to schedule to run when this function returns.
 *
 *****************************************************************************/
void user_init(void)
{
  // Initializes hardware
  __user_init_hw();

  // Drive the UART directly to print the HELLO_MSG
  __esos_unsafe_PutString(HELLO_MSG);

  esos_RegisterTask(test_thread);
  __esos_InitSUI();

}

/**
 * @}
 */
