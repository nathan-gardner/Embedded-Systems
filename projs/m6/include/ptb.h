/*******************************************************************************
 *
 *            Nucleo-L452RE Pinouts for EduBase-L452 Peripherals
 *
 *******************************************************************************
 * FileName:       ptb.h
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
 * This header file allows for the use of the Nucleo-L452RE on the
 * EduBase-L452 peripheral target board.
 * Please refer to the schematics at
 * jwbruce.info/teaching/ece4140/datasheets/BaseBoard_L452_p1.pdf.
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Author                Date                  Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Blake Martin          3/23/2022             Original
 * Nathan Gardner
 * Kester Nucum
 * Triston Whitescarver
 ******************************************************************************/

#include "esos.h"
#ifndef PTB_H
#define PTB_H


typedef enum { command, alphaNum, toUpper, encryption, decryption, echo } systemPriority;
//*********************************************************************************************************************************
// NUCLEO DEFINES GO HERE:

// LED2 DEFINES
#define NUCLEO_LED2_PORT           GPIOA
#define NUCLEO_LED2_PIN            GPIO5
#define NUCLEO_LED2_SETUP()        gpio_mode_setup(NUCLEO_LED2_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, NUCLEO_LED2_PIN)
#define NUCLEO_LED2_TOGGLE()       gpio_toggle(NUCLEO_LED2_PORT,NUCLEO_LED2_PIN)
#define NUCLEO_LED2_SET()          gpio_set(NUCLEO_LED2_PORT, NUCLEO_LED2_PIN)
#define NUCLEO_LED2_CLEAR()        gpio_clear(NUCLEO_LED2_PORT, NUCLEO_LED2_PIN)
#define NUCLEO_LED2_GETVAL()       gpio_get(NUCLEO_LED2_PORT, NUCLEO_LED2_PIN)

// BUTTON B1 DEFINES & MACROS
#define NUCLEO_BUTTON_PORT          GPIOC
#define NUCLEO_BUTTON_PIN           GPIO13
#define NUCLEO_BUTTON_PORT_RCC      GPIOC
#define NUCLEO_BUTTON_ENABLE()      gpio_mode_setup(NUCLEO_BUTTON_PORT,GPIO_MODE_INPUT,GPIO_PUPD_NONE,NUCLEO_BUTTON_PIN)
#define NUCLEO_BUTTON_PUSHED()      !gpio_get(NUCLEO_BUTTON_PORT,NUCLEO_BUTTON_PIN)

// USART2 TX and RX pins
#define NUCLEO_USART2_TX_RX_GPIO_PORT   GPIOA
#define NUCLEO_USART2_TX_PORT           GPIOA      // USART2 Tx- A2
#define NUCLEO_USART2_TX_PIN            GPIO2
#define NUCLEO_USART2_RX_PORT           GPIOA      // USART2 Tx- A2
#define NUCLEO_USART2_RX_PIN            GPIO3
#define NUCLEO_USART2_TX_ALT_FNCT       GPIO_AF7
#define NUCLEO_USART2_RX_ALT_FNCT       GPIO_AF7

// EXTERNAL INTERRUPT VECTORS
#define NUCLEO_BUTTON_EXTI              EXTI13
#define NUCLEO_BUTTON_NVIC              13
//*********************************************************************************************************************************
//*
//**
//****
//*******
//***********
//                  INTENTIONAL WHITESPACE
//****************
//*******************
//**********************
//*************************
//*********************************************************************************************************************************
// EDUBASE-L452 BOARD DEFINES & MACROS GO BELOW:
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// EduBase-L452 LEDs
#define EDUB_LED0_PORT  GPIOA   // LED0 - A1
#define EDUB_LED0_PIN   GPIO1
#define EDUB_LED1_PORT  GPIOA   // LED1 - A2
#define EDUB_LED1_PIN   GPIO0
#define EDUB_LED2_PORT  GPIOC   // LED2 - C7
#define EDUB_LED2_PIN   GPIO7
#define EDUB_LED3_PORT  GPIOC   // LED3 - C8
#define EDUB_LED3_PIN   GPIO8
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// EduBase-L452 Pushbuttons Switches: SW2 - B11, SW3 - B10, SW4 - B9, BW5 - B8
#define EDUB_SW2_PORT   GPIOB   // SW2 - B11
#define EDUB_SW2_PIN    GPIO11
#define EDUB_SW3_PORT   GPIOB   // SW3 - B10
#define EDUB_SW3_PIN    GPIO10
#define EDUB_SW4_PORT   GPIOB   // SW4 - B9
#define EDUB_SW4_PIN    GPIO9
#define EDUB_SW5_PORT   GPIOB   // SW5 - B8
#define EDUB_SW5_PIN    GPIO8
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// EduBase-L452 Keypad Buttons
#define EDUB_KEYPAD_ROW0_PORT    GPIOB   // Keypad ROW0 - B8
#define EDUB_KEYPAD_ROW0_PIN     GPIO8
#define EDUB_KEYPAD_ROW1_PORT    GPIOB   // Keypad ROW1 - B9
#define EDUB_KEYPAD_ROW1_PIN     GPIO9
#define EDUB_KEYPAD_ROW2_PORT    GPIOB   // Keypad ROW2 - B10
#define EDUB_KEYPAD_ROW2_PIN     GPIO10
#define EDUB_KEYPAD_ROW3_PORT    GPIOB   // Keypad ROW3 - B11
#define EDUB_KEYPAD_ROW3_PIN     GPIO11
#define EDUB_KEYPAD_COL0_PORT    GPIOB   // Keypad COL0 - B1
#define EDUB_KEYPAD_COL0_PIN     GPIO1
#define EDUB_KEYPAD_COL1_PORT    GPIOB   // Keypad COL1 - B2
#define EDUB_KEYPAD_COL1_PIN     GPIO2
#define EDUB_KEYPAD_COL2_PORT    GPIOB   // Keypad COL2 - B3
#define EDUB_KEYPAD_COL2_PIN     GPIO3
#define EDUB_KEYPAD_COL3_PORT    GPIOB   // Keypad COL3 - B4
#define EDUB_KEYPAD_COL3_PIN     GPIO4
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// EDUB LCD DISPLAY DEFINES
#define EDUB_LCD_RS_PORT    GPIOA        // LCD_RS - A12
#define EDUB_LCD_RS_PIN     GPIO12
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// EDUB PWM DEFINES
#define EDUB_PWM1_PORT       GPIOC     // EDUB PWM1 -> GPIOC6
#define EDUB_PWM1_PIN        GPIO6
// PWM2 currently unknown
#define EDUB_PWM3_PORT       GPIOA     // EDUB PWM3 -> GPIOA11
#define EDUB_PWM3_PIN        GPIO11
#define EDUB_PWM4_PORT       GPIOB     // EDUB PWM4 -> GPIOB14
#define EDUB_PWM4_PIN        GPIO14


//___________________________________________________________________________________________________________________________________________
// ALL EDUB MACROS:

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// EDUB LED0 MACROS
#define EDUB_LED0_SETUP()        gpio_mode_setup(EDUB_LED0_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, EDUB_LED0_PIN)
#define EDUB_LED0_TOGGLE()       gpio_toggle(EDUB_LED0_PORT,EDUB_LED0_PIN)
#define EDUB_LED0_SET()          gpio_set(EDUB_LED0_PORT, EDUB_LED0_PIN)
#define EDUB_LED0_CLEAR()        gpio_clear(EDUB_LED0_PORT, EDUB_LED0_PIN)
#define EDUB_LED0_GETVAL()       gpio_get(EDUB_LED0_PORT, EDUB_LED0_PIN)
// EDUB LED1 MACROS
#define EDUB_LED1_SETUP()        gpio_mode_setup(EDUB_LED1_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, EDUB_LED1_PIN)
#define EDUB_LED1_TOGGLE()       gpio_toggle(EDUB_LED1_PORT,EDUB_LED1_PIN)
#define EDUB_LED1_SET()          gpio_set(EDUB_LED1_PORT, EDUB_LED1_PIN)
#define EDUB_LED1_CLEAR()        gpio_clear(EDUB_LED1_PORT, EDUB_LED1_PIN)
#define EDUB_LED1_GETVAL()       gpio_get(EDUB_LED1_PORT, EDUB_LED1_PIN)
// EDUB LED2 MACROS
#define EDUB_LED2_SETUP()        gpio_mode_setup(EDUB_LED2_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, EDUB_LED2_PIN)
#define EDUB_LED2_TOGGLE()       gpio_toggle(EDUB_LED2_PORT,EDUB_LED2_PIN)
#define EDUB_LED2_SET()          gpio_set(EDUB_LED2_PORT, EDUB_LED2_PIN)
#define EDUB_LED2_CLEAR()        gpio_clear(EDUB_LED2_PORT, EDUB_LED2_PIN)
#define EDUB_LED2_GETVAL()       gpio_get(EDUB_LED2_PORT, EDUB_LED2_PIN)
// EDUB LED3 MACROS
#define EDUB_LED3_SETUP()        gpio_mode_setup(EDUB_LED3_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, EDUB_LED3_PIN)
#define EDUB_LED3_TOGGLE()       gpio_toggle(EDUB_LED3_PORT,EDUB_LED3_PIN)
#define EDUB_LED3_SET()          gpio_set(EDUB_LED3_PORT, EDUB_LED3_PIN)
#define EDUB_LED3_CLEAR()        gpio_clear(EDUB_LED3_PORT, EDUB_LED3_PIN)
#define EDUB_LED3_GETVAL()       gpio_get(EDUB_LED3_PORT, EDUB_LED3_PIN)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// EDUB BUTTON MACROS
//SW2
#define EDUB_SW2_ENABLE()      gpio_mode_setup(EDUB_SW2_PORT,GPIO_MODE_INPUT,GPIO_PUPD_NONE,EDUB_SW2_PIN)
#define EDUB_SW2_PUSHED()      gpio_get(EDUB_SW2_PORT,EDUB_SW2_PIN)
//SW3
#define EDUB_SW3_ENABLE()      gpio_mode_setup(EDUB_SW3_PORT,GPIO_MODE_INPUT,GPIO_PUPD_NONE,EDUB_SW3_PIN)
#define EDUB_SW3_PUSHED()      gpio_get(EDUB_SW3_PORT,EDUB_SW3_PIN)
//SW4
#define EDUB_SW4_ENABLE()      gpio_mode_setup(EDUB_SW4_PORT,GPIO_MODE_INPUT,GPIO_PUPD_NONE,EDUB_SW4_PIN)
#define EDUB_SW4_PUSHED()      gpio_get(EDUB_SW4_PORT,EDUB_SW4_PIN)
//SW5
#define EDUB_SW5_ENABLE()      gpio_mode_setup(EDUB_SW5_PORT,GPIO_MODE_INPUT,GPIO_PUPD_NONE,EDUB_SW5_PIN)
#define EDUB_SW5_PUSHED()      gpio_get(EDUB_SW5_PORT,EDUB_SW5_PIN)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// EDUB LCD MACROS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// EduBase-L452 Keypad Initialization Macros
#define EDUB_KEYPAD_ROW0_SETUP()   gpio_mode_setup(EDUB_KEYPAD_ROW0_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, EDUB_KEYPAD_ROW0_PIN)
#define EDUB_KEYPAD_ROW1_SETUP()   gpio_mode_setup(EDUB_KEYPAD_ROW1_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, EDUB_KEYPAD_ROW1_PIN)
#define EDUB_KEYPAD_ROW2_SETUP()   gpio_mode_setup(EDUB_KEYPAD_ROW2_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, EDUB_KEYPAD_ROW2_PIN)
#define EDUB_KEYPAD_ROW3_SETUP()   gpio_mode_setup(EDUB_KEYPAD_ROW3_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, EDUB_KEYPAD_ROW3_PIN)
#define EDUB_KEYPAD_COL0_SETUP()   gpio_mode_setup(EDUB_KEYPAD_COL0_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, EDUB_KEYPAD_COL0_PIN)
#define EDUB_KEYPAD_COL1_SETUP()   gpio_mode_setup(EDUB_KEYPAD_COL1_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, EDUB_KEYPAD_COL1_PIN)
#define EDUB_KEYPAD_COL2_SETUP()   gpio_mode_setup(EDUB_KEYPAD_COL2_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, EDUB_KEYPAD_COL2_PIN)
#define EDUB_KEYPAD_COL3_SETUP()   gpio_mode_setup(EDUB_KEYPAD_COL3_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, EDUB_KEYPAD_COL3_PIN)
// EduBase-L452 Keypad Functional Macros
#define EDUB_KEYPAD_COL0_SET()     gpio_set(EDUB_KEYPAD_COL0_PORT, EDUB_KEYPAD_COL0_PIN)
#define EDUB_KEYPAD_COL0_CLEAR()   gpio_clear(EDUB_KEYPAD_COL0_PORT, EDUB_KEYPAD_COL0_PIN)
#define EDUB_KEYPAD_COL1_SET()     gpio_set(EDUB_KEYPAD_COL1_PORT, EDUB_KEYPAD_COL1_PIN)
#define EDUB_KEYPAD_COL1_CLEAR()   gpio_clear(EDUB_KEYPAD_COL1_PORT, EDUB_KEYPAD_COL1_PIN)
#define EDUB_KEYPAD_COL2_SET()     gpio_set(EDUB_KEYPAD_COL2_PORT, EDUB_KEYPAD_COL2_PIN)
#define EDUB_KEYPAD_COL2_CLEAR()   gpio_clear(EDUB_KEYPAD_COL2_PORT, EDUB_KEYPAD_COL2_PIN)
#define EDUB_KEYPAD_COL3_SET()     gpio_set(EDUB_KEYPAD_COL3_PORT, EDUB_KEYPAD_COL3_PIN)
#define EDUB_KEYPAD_COL3_CLEAR()   gpio_clear(EDUB_KEYPAD_COL3_PORT, EDUB_KEYPAD_COL3_PIN)
//#define EDUB_KEYPAD_COL_CLEARALL() do{ EDUB_KEYPAD_COL0_CLEAR(); EDUB_KEYPAD_COL1_CLEAR(); EDUB_KEYPAD_COL2_CLEAR(); EDUB_KEYPAD_COL3_CLEAR(); }while(0)
#define EDUB_KEY0_GETVAL()         gpio_get(EDUB_KEYPAD_ROW3_PORT, EDUB_KEYPAD_ROW3_PIN)
#define EDUB_KEY1_GETVAL()         gpio_get(EDUB_KEYPAD_ROW0_PORT, EDUB_KEYPAD_ROW0_PIN)
#define EDUB_KEY2_GETVAL()         gpio_get(EDUB_KEYPAD_ROW0_PORT, EDUB_KEYPAD_ROW0_PIN)
#define EDUB_KEY3_GETVAL()         gpio_get(EDUB_KEYPAD_ROW0_PORT, EDUB_KEYPAD_ROW0_PIN)
#define EDUB_KEY4_GETVAL()         gpio_get(EDUB_KEYPAD_ROW1_PORT, EDUB_KEYPAD_ROW1_PIN)
#define EDUB_KEY5_GETVAL()         gpio_get(EDUB_KEYPAD_ROW1_PORT, EDUB_KEYPAD_ROW1_PIN)
#define EDUB_KEY6_GETVAL()         gpio_get(EDUB_KEYPAD_ROW1_PORT, EDUB_KEYPAD_ROW1_PIN)
#define EDUB_KEY7_GETVAL()         gpio_get(EDUB_KEYPAD_ROW2_PORT, EDUB_KEYPAD_ROW2_PIN)
#define EDUB_KEY8_GETVAL()         gpio_get(EDUB_KEYPAD_ROW2_PORT, EDUB_KEYPAD_ROW2_PIN)
#define EDUB_KEY9_GETVAL()         gpio_get(EDUB_KEYPAD_ROW2_PORT, EDUB_KEYPAD_ROW2_PIN)
#define EDUB_KEYA_GETVAL()        gpio_get(EDUB_KEYPAD_ROW0_PORT, EDUB_KEYPAD_ROW0_PIN)
#define EDUB_KEYB_GETVAL()        gpio_get(EDUB_KEYPAD_ROW1_PORT, EDUB_KEYPAD_ROW1_PIN)
#define EDUB_KEYC_GETVAL()        gpio_get(EDUB_KEYPAD_ROW2_PORT, EDUB_KEYPAD_ROW2_PIN)
#define EDUB_KEYD_GETVAL()        gpio_get(EDUB_KEYPAD_ROW3_PORT, EDUB_KEYPAD_ROW3_PIN)
#define EDUB_KEYSTAR_GETVAL()        gpio_get(EDUB_KEYPAD_ROW3_PORT, EDUB_KEYPAD_ROW3_PIN)
#define EDUB_KEYPOUND_GETVAL()        gpio_get(EDUB_KEYPAD_ROW3_PORT, EDUB_KEYPAD_ROW3_PIN)
// must complete key getval's
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// EDUB PWM MACROS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// EDUB SPI MACROS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// EDUB I2C MACROS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//*********************************************************************************************************************************
// USEFUL MACROS OR USER-DEF FUNCTIONS GO HERE:

#endif