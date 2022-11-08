/*******************************************************************************
 *
 *                ECE 4140 Milestone 6 Firmware Version 1.1
 *
 *******************************************************************************
 * FileName:       esos_stm32l4_edub_7seg.h
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
 * Authors                Date                  Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Blake Martin          04/26/22                 System Architect
 * Nathan Gardner        04/26/22                 Original
 * Triston Whitescarver  04/26/22                 Original
 *********************************************************************************/

#ifndef ESOS_STM32L$_EDUB_7SEG_H
#define ESOS_STM32L$_EDUB_7SEG_H
#include "esos.h"
#include "esos_stm32l4_edub.h"
#include "esos_7seg.h"
#include "circularBuffer.h"

#ifndef STM32L4
#define STM32L4
#endif // !STM32L4

ESOS_CHILD_TASK(seg7_hw_write_childTask, uint8_t u8_disp, uint8_t u8_pattern);

#ifndef ESOS_USE_SPI
#define ESOS_USE_SPI
#endif
/* Header file for hardware-dependent c file */



typedef uint8_t ESOS_7SEG_HANDLE;

extern ESOS_USER_TASK(__esos_7seg_service);

//**************************************

#define MIN_7SEG_BLINK_FREQ 500000
#define MAX_7SEG_BLINK_PERIOD (1000 / MIN_7SEG_BLINK_FREQ)
#define SHIFT_FREQ 500
//**************************************


// ***************************************************************************
// ESOS 7seg Character Map:
#define DIG0                0x80    //DISP4
#define DIG1                0x40    //DISP3
#define DIG2                0x20    //DISP2
#define DIG3                0x10    //DISP1

// 7-Segment Display Character Map:
#define PATTERN_0           0b11000000
#define PATTERN_1           0b11111001
#define PATTERN_2           0b10100100
#define PATTERN_3           0b10110000
#define PATTERN_4           0b10011001
#define PATTERN_5           0b10010010
#define PATTERN_6           0b10000010
#define PATTERN_7           0b11111000
#define PATTERN_8           0b10000000
#define PATTERN_9           0b10010000
#define PATTERN_A           0b10001000
#define PATTERN_B           0b10000011
#define PATTERN_C           0b11000110
#define PATTERN_D           0b10100001
#define PATTERN_E           0b10000110
#define PATTERN_F           0b10001110
#define PATTERN_G           0b10000010
#define PATTERN_H           0b10001001
#define PATTERN_I           0b11111001
#define PATTERN_J           0b11110001
#define PATTERN_K           0b10100111
#define PATTERN_L           0b11000111
#define PATTERN_M           0b10111111
#define PATTERN_N           0b10101011
#define PATTERN_O           PATTERN_0
#define PATTERN_P           0b10001100
#define PATTERN_Q           0b10000100
#define PATTERN_R           0b11001100
#define PATTERN_S           0b10010010
#define PATTERN_T           0b01000001
#define PATTERN_U           0b11010011
#define PATTERN_V           0b11000001
#define PATTERN_W           0b11010111
#define PATTERN_X           0b11000101
#define PATTERN_Y           0b11011001
#define PATTERN_Z           0b10001110
#define PATTERN_DASH        0b10111111
#define PATTERN_UNDERSCORE  0b11110111
#define PATTERN_BLANK       0b11111111
//***************************************************************************

extern ESOS_CHILD_TASK(seg7_hw_write_childTask, uint8_t u8_disp, uint8_t u8_pattern);
void seg7_SPI_hw_config();
void __esos_7seg_setCharacter(uint8_t u8_disp, uint8_t u8_pattern);
void __esos_7seg_illuminate(uint8_t disp);



#endif