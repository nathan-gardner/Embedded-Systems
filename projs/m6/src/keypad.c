/*******************************************************************************
 *
 *               Vigenere Cipher Encryption Library Version 1.1
 *
 *******************************************************************************
 * FileName:       keypad.c
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
 * This library can be used to encrypt and decrypt strings using
 * a Vigenere cipher.
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Author                Date                  Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nathan Gardner        3/17/22               Original
 *****************************************************************************
 */
#ifndef STM32L4
   #define STM32L4
#endif
#include "keypad.h"
#include "ptb.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <libopencm3/stm32/gpio.h> // Should be somewhere else??

/*
 * When reading in the keypad (calling gpio_get(PORTB, PINx)),
 * the gpio_get() returns a uint16_t of the entire port,
 * so rows 0-3 are connected to Port B pins 8-11, respectively.
 * Thus, any button on row X will return the following:
 * ROW0 -> 0x0100 (bit/pin 8)
 * ROW1 -> 0x0200 (bit/pin 9)
 * ROW2 -> 0x0400 (bit/pin 10)
 * ROW3 -> 0x0800 (bit/pin 11)
 */
#define  ROW0_BIT_OFFSET   8
#define  ROW1_BIT_OFFSET   9
#define  ROW2_BIT_OFFSET   10
#define  ROW3_BIT_OFFSET   11

/*
 * read_keypad() will return a 16-bit value in which each bit corresponds
 * to the on/off status (on-1, off-0) of a key on the keypad,
 * with bit 0 as the Lsb and bit 15 as the Msb
 * Bits 0 to 9 correspond to keys 0 to 9, respectively
 * Bits 10 to 13 correspond to keys A to D, respectively
 * Bit 14 corresponds to key # (pound)
 * Bit 15 corresponds to key * (star)
 */
#define  KEY0_BIT_OFFSET       0
#define  KEY1_BIT_OFFSET       1
#define  KEY2_BIT_OFFSET       2
#define  KEY3_BIT_OFFSET       3
#define  KEY4_BIT_OFFSET       4
#define  KEY5_BIT_OFFSET       5
#define  KEY6_BIT_OFFSET       6
#define  KEY7_BIT_OFFSET       7
#define  KEY8_BIT_OFFSET       8
#define  KEY9_BIT_OFFSET       9
#define  KEYA_BIT_OFFSET      10
#define  KEYB_BIT_OFFSET      11
#define  KEYC_BIT_OFFSET      12
#define  KEYD_BIT_OFFSET      13
#define  KEYPOUND_BIT_OFFSET  14
#define  KEYSTAR_BIT_OFFSET   15

//-------------------------------------------------------------------------------------------
/* Used to introduce a small delay to allow the voltage to settle */
static void delay_loop(int32_t loops)
{
    while(loops > 0)
    {
       asm("nop");
       loops--;
    }
}

uint16_t read_keypad(void)
{
    /*
     * u16_Keys represents the on/off status of all keys in the keypad,
     * with bit 0 as the Lsb and bit 15 as the Msb
     * Bits 0 to 9 correspond to keys 0 to 9, respectively
     * Bits 10 to 13 correspond to keys A to D, respectively
     * Bit 14 corresponds to key # (pound)
     * Bit 15 corresponds to key * (star)
     */
    uint16_t u16_Keys = 0x0000;

    /*
     * Turns off all columns before driving column 0 to read keypad keys,
     * then allows signal to settle
     */
    EDUB_KEYPAD_COL0_CLEAR();
    EDUB_KEYPAD_COL1_CLEAR();
    EDUB_KEYPAD_COL2_CLEAR();
    EDUB_KEYPAD_COL3_CLEAR();
    EDUB_KEYPAD_COL0_SET();
    delay_loop(50);

    /* Reads all keys on col 0 (keys 1, 4, 7, and *) */
    u16_Keys |= (EDUB_KEY1_GETVAL() >> ROW0_BIT_OFFSET) << KEY1_BIT_OFFSET;
    u16_Keys |= (EDUB_KEY4_GETVAL() >> ROW1_BIT_OFFSET) << KEY4_BIT_OFFSET;
    u16_Keys |= (EDUB_KEY7_GETVAL() >> ROW2_BIT_OFFSET) << KEY7_BIT_OFFSET;
    u16_Keys |= (EDUB_KEYSTAR_GETVAL() >> ROW3_BIT_OFFSET) << KEYSTAR_BIT_OFFSET;

    /*
     * Turns off all columns before driving column 1 to read keypad keys,
     * then allows signal to settle
     */
    EDUB_KEYPAD_COL0_CLEAR();
    EDUB_KEYPAD_COL1_CLEAR();
    EDUB_KEYPAD_COL2_CLEAR();
    EDUB_KEYPAD_COL3_CLEAR();
    EDUB_KEYPAD_COL1_SET();
    delay_loop(50);

    /* Reads all keys on col 1 (keys 2, 5, 8, and 0) */
    u16_Keys |= (EDUB_KEY2_GETVAL() >> ROW0_BIT_OFFSET) << KEY2_BIT_OFFSET;
    u16_Keys |= (EDUB_KEY5_GETVAL() >> ROW1_BIT_OFFSET) << KEY5_BIT_OFFSET;
    u16_Keys |= (EDUB_KEY8_GETVAL() >> ROW2_BIT_OFFSET) << KEY8_BIT_OFFSET;
    u16_Keys |= (EDUB_KEY0_GETVAL() >> ROW3_BIT_OFFSET) << KEY0_BIT_OFFSET;

    /*
     * Turns off all columns before driving column 2 to read keypad keys,
     * then allows signal to settle
     */
    EDUB_KEYPAD_COL0_CLEAR();
    EDUB_KEYPAD_COL1_CLEAR();
    EDUB_KEYPAD_COL2_CLEAR();
    EDUB_KEYPAD_COL3_CLEAR();
    EDUB_KEYPAD_COL2_SET();
    delay_loop(50);

    /* Reads all keys on col 2 (keys 3, 6, 9, and #) */
    u16_Keys |= (EDUB_KEY3_GETVAL() >> ROW0_BIT_OFFSET) << KEY3_BIT_OFFSET;
    u16_Keys |= (EDUB_KEY6_GETVAL() >> ROW1_BIT_OFFSET) << KEY6_BIT_OFFSET;
    u16_Keys |= (EDUB_KEY9_GETVAL() >> ROW2_BIT_OFFSET) << KEY9_BIT_OFFSET;
    u16_Keys |= (EDUB_KEYPOUND_GETVAL() >> ROW3_BIT_OFFSET) << KEYPOUND_BIT_OFFSET;

    /*
     * Turns off all columns before driving column 3 to read keypad keys,
     * then allows signal to settle
     */
    EDUB_KEYPAD_COL0_CLEAR();
    EDUB_KEYPAD_COL1_CLEAR();
    EDUB_KEYPAD_COL2_CLEAR();
    EDUB_KEYPAD_COL3_CLEAR();
    EDUB_KEYPAD_COL3_SET();
    delay_loop(50);

    /* Reads all keys on col 3 (keys A, B, C, and D) */
    u16_Keys |= (EDUB_KEYA_GETVAL() >> ROW0_BIT_OFFSET) << KEYA_BIT_OFFSET;
    u16_Keys |= (EDUB_KEYB_GETVAL() >> ROW1_BIT_OFFSET) << KEYB_BIT_OFFSET;
    u16_Keys |= (EDUB_KEYC_GETVAL() >> ROW2_BIT_OFFSET) << KEYC_BIT_OFFSET;
    u16_Keys |= (EDUB_KEYD_GETVAL() >> ROW3_BIT_OFFSET) << KEYD_BIT_OFFSET;

    return u16_Keys;
}
