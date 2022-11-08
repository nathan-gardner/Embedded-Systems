/*******************************************************************************
 *
 *                ECE 4140 Milestone 8 Firmware Version 1.1
 *
 *******************************************************************************
 * FileName:       esos_7seg.c
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
 * Blake Martin          04/26/22                 System Architecture
 * Nathan Gardner        04/26/22                 Original
 * Triston Whitescarver  04/26/22                 Original
 *********************************************************************************/
#include "esos.h"
#include "esos_spi.h"
#include "esos_7seg.h"
#include "esos_stm32l4_edub_7seg.h"
#include "esos_stm32l4_edub.h"
#include "circularBuffer.h"

#ifndef STM32L4
#define STM32L4
#endif // !STM32L4

#ifndef ESOS_USE_SPI
#define ESOS_USE_SPI
#endif

 // GLOBAL VARIABLES:
volatile st_esos_7seg_wrapper __ast_esos7SEGs; // struct containing 4 structs and circular buffer for managing all 4 7-seg displays
extern ESOS_CHILD_TASK(seg7_hw_write_childTask, uint8_t u8_disp, uint8_t u8_pattern);
extern void seg7_SPI_hw_config(void);

uint32_t u32_nowGlobal = 0;
// ******************************************************************************************************************************************************
// *************************************************************************************************************************************
// PUBLIC API FUNCTION DEFINITIONS:
// *************************************************************************************************************************************
/**
 * @brief initialize the 7-segment display data structures
 *
 */
void init_7seg_structs(void)
{
    uint8_t i;
    __ast_esos7SEGs.st_7seg[3].u8_charDisp = 'a';
    __ast_esos7SEGs.st_7seg[2].u8_charDisp = 'b';
    __ast_esos7SEGs.st_7seg[1].u8_charDisp = 'c';
    __ast_esos7SEGs.st_7seg[0].u8_charDisp = 'd';
    __ast_esos7SEGs.st_7seg[0].u8_segSelector = 0x01;
    __ast_esos7SEGs.st_7seg[1].u8_segSelector = 0x02;
    __ast_esos7SEGs.st_7seg[2].u8_segSelector = 0x04;
    __ast_esos7SEGs.st_7seg[3].u8_segSelector = 0x08;

    for (uint8_t k = 0; k < 26; k++)
    {
        __ast_esos7SEGs.linearBuffer[k] = k + 'a';
        __ast_esos7SEGs.numChars++;
    }

    __ast_esos7SEGs.u8_state = ON;               // set the blinking state to off
    __ast_esos7SEGs.b_blinking = OFF;            // set blinking state to off
    __ast_esos7SEGs.b_scrollMode = TRUE;        // set scroll mode to off
    __ast_esos7SEGs.u32_scrollDelay = 500;       // set scroll delay to 0
    __ast_esos7SEGs.u32_blinkPeriod = 100;      // set blink period to 0
    __ast_esos7SEGs.u32_lastEventTime = 0;       // set last event time to 0
    __ast_esos7SEGs.u32_lastScrollEventTime = 0; // set last scroll event time to 0
}
// *************************************************************************************************************************************
/**
 * @brief Will modify the ESOS 7-seg services data structures accordingly
 * so that the hidden task will blank the display at its earliest opportunity.
 */
void esos_7seg_clearScreen(void)
{
    for (int i = 0; i < 63; i++)
    {
        __ast_esos7SEGs.linearBuffer[i] = ' ';
        __ast_esos7SEGs.numChars = 0;
    }
    for (int i = 0; i < 4; i++)
    {
        __ast_esos7SEGs.st_7seg[i].u8_charDisp = ' ';
    }
    __ast_esos7SEGs.numChars = 0;
}
// *************************************************************************************************************************************
/**
 * @brief Will modify the ESOS 7-seg services data structures accordingly so that the
 * hidden task will display character u8_data in the u8_column at its earliest oppurtunity.
 * @param u8_column
 * @param u8_data
 */
void esos_7seg_writeCharacter(uint8_t u8_column, uint8_t u8_data)
{
    if (u8_column > 3)
    {           // Check that the column is valid
        return; // return if not valid
    }
    __ast_esos7SEGs.st_7seg[u8_column].u8_charDisp = u8_data; // Set the character to be displayed
    __ast_esos7SEGs.b_scrollMode = FALSE;                     // Set the scroll mode to off
}

void esos_7seg_takeNumericInput(uint8_t* u8_data)
{
    for (uint8_t i = 0; i < 4; i++)
    {
        __ast_esos7SEGs.st_7seg[i].u8_charDisp = u8_data[3 - i];
    }
}
// *************************************************************************************************************************************
/**
 * @brief Will modify esos 7-seg data structs accordingly so that the
 * hidden task will display the first u8_bufflen chars of pu8_data
 * at its earliest opportunity.
 * If the buffer exceeds the # of available char displays, the
 * hidden task must "scroll" the buffer message repeatedly until directed otherwise.
 * @param pu8_data
 * @param u8_bufflen
 */
void esos_7seg_writeBuffer(uint8_t* pu8_data, uint8_t u8_bufflen)
{ // write a buffer of characters to the 7-seg display
    esos_7seg_clearScreen();
    uint8_t k = 0;
    while (k < u8_bufflen)
    { // write the buffer to the circular buffer
        __ast_esos7SEGs.linearBuffer[k] = pu8_data[k];
        __ast_esos7SEGs.numChars++;
        k++; // increment the buffer index
    }
}
// *************************************************************************************************************************************
/**
 * @brief Will return a string containing what is currently
 * displayed on the 7-seg displays.
 * @param void
 * @return void
 */
char* esos_7seg_getDisplay(void)
{ // Return a string containing what is currently displayed on the 7-seg displays
    uint8_t i;
    static char returnString[4] = { 0, 0, 0, 0 }; // Initialize the return string
    for (i = 0; i < 4; i++)
    {                                                             // Loop through the 4 7-seg displays
        returnString[i] = __ast_esos7SEGs.st_7seg[i].u8_charDisp; // Copy the character to the return string
    }
    return returnString; // Return the string
}
// *************************************************************************************************************************************
/**
 * @brief Will modify the ESOS 7-seg services data structs accordingly so
 * that the hidden task will display the zero-terminated string "psz_data"
 * at its earliest opportunity. If the string exceeds the number of available
 * char displays, the hidden task must "scroll" the buffer message repeatedly until
 * directed otherwise.
 * @param psz_data
 * @return void
 */
void esos_7seg_writeString(char* psz_data)
{ // write a string to the 7-seg display
    __ast_esos7SEGs.u8_state = ON; // set segs to ON.
    esos_7seg_writeBuffer(psz_data, strlen(psz_data));
    __ast_esos7SEGs.b_scrollMode = TRUE;
}
// *************************************************************************************************************************************
/**
 * @brief If passed a TRUE argument, the function will instruct the
 * ESOS 7seg services hidden task to rapidly blink the contents of the 7-seg
 * displays.
 * A FALSE argument will cause the displays to be constantly illuminated.
 *
 * @param b_state
 * @return void
 */
void esos_7seg_blink(BOOL b_state, uint32_t u32_period)
{ // TRUE = BLINK, FALSE = NO BLINK
    __ast_esos7SEGs.b_blinking = b_state;
    __ast_esos7SEGs.u32_blinkPeriod = u32_period;
}
// *************************************************************************************************************************************
/**
 * @brief Will modify the ESOS 7-seg services data structures accordingly
 * so that the hidden task will display char u8_data
 * as a decimal number (right-justified) on the display.
 *
 * @param u8_data
 * @return void
 */
void esos_7seg_writeu8Decimal(uint8_t u8_data)
{
    __ast_esos7SEGs.u8_state = ON; // set segs to ON.
    __ast_esos7SEGs.b_scrollMode = FALSE;
    esos_7seg_writeU16Decimal((uint16_t)u8_data);
}
// *************************************************************************************************************************************
/**
 * @brief Will modify the ESOS 7seg data structs accordingly so that
 * the hidden task will display character u8_data as a hex number (right-justified) on the display.
 * @param u8_data
 */
void esos_7seg_writeU8Hex(uint8_t u8_data)
{ // Write an unsigned 8 bit hex number to the 7-seg display
    __ast_esos7SEGs.b_scrollMode = FALSE;
    __ast_esos7SEGs.u8_state = ON; // set segs to ON.
    esos_7seg_writeU16Hex((uint16_t)u8_data);
}
// *************************************************************************************************************************************
/**
 * @brief Will modify the ESOS 7-seg data structures accordingly
 * so that the hidden task will display the character i8_data as
 * a decimal # (right-justified) on the display.
 * A negative sign, if needed, will be in the left-most "empty" digit.
 *
 * @param i8_data
 */
void esos_7seg_writeI8Decimal(int8_t i8_data)
{ // Write Signed data to the 7-seg display:
    __ast_esos7SEGs.u8_state = ON; // set segs to ON.
    __ast_esos7SEGs.b_scrollMode = FALSE;
    uint8_t u8_digit[4] = { 0, 0, 0, 0 };
    uint8_t largestDigit = 0, isNegative = 0;
    if (i8_data < 0)
    { // If the number is negative
        isNegative = 1;
        i8_data = -i8_data; // Make the number positive
    }
    // Calculate the decimal representation and determine the largest digit
    for (int i = 0; i < 4; i++)
    {
        u8_digit[i] = i8_data % 10;
        if (u8_digit[i] != 0)
        {
            largestDigit = i;
        }
        i8_data = i8_data / 10;
    }
    // Write the digits to the display with blanks for the left-most '0' digits
    if (u8_digit[largestDigit] == 0)
    {
        __ast_esos7SEGs.st_7seg[0].u8_charDisp = u8_digit[0] + '0'; // add '0' to the digit to get the correct character
        for (int i = 1; i < 4; i++)
        {
            __ast_esos7SEGs.st_7seg[i].u8_charDisp = ' ';
        }
    }
    else
    {
        for (uint8_t i = 0; i <= largestDigit; i++)
        {
            u8_digit[i] += '0';
            __ast_esos7SEGs.st_7seg[i].u8_charDisp = u8_digit[i]; // add '0' to the digit to get the correct character
        }
        if (isNegative)
        {
            __ast_esos7SEGs.st_7seg[largestDigit + 1].u8_charDisp = '-';
        }
        else
        {
            __ast_esos7SEGs.st_7seg[largestDigit + 1].u8_charDisp = ' ';
        }
        for (int i = largestDigit + 2; i < 4; i++)
        {
            __ast_esos7SEGs.st_7seg[i].u8_charDisp = ' ';
        }
    }
}
// *************************************************************************************************************************************
/**
 * @brief Will modify the ESOS 7-seg data structures accordingly
 * so that the hidden task will display the character i8_data as
 * a hexadecimal # (right-justified) on the display.
 * A negative sign, if needed, will be in the left-most "empty" digit.
 * @param i8_data
 */
void esos_7seg_writeI8Hex(int8_t i8_data)
{
    __ast_esos7SEGs.u8_state = ON; // set segs to ON.
    __ast_esos7SEGs.b_scrollMode = FALSE;
    uint8_t u8_digit[4] = { 0, 0, 0, 0 };
    uint8_t largestDigit = 0, isNegative = 0;
    if (i8_data < 0)
    { // If the number is negative
        isNegative = 1;
        i8_data = -i8_data; // Make the number positive
    }
    // Calculate the decimal representation and determine the largest digit
    for (int i = 0; i < 4; i++)
    {
        u8_digit[i] = i8_data % 16;
        if (u8_digit[i] != 0)
        {
            largestDigit = i;
        }
        i8_data = i8_data / 16;
    }
    // Write the digits to the display with blanks for the left-most '0' digits
    if (u8_digit[largestDigit] == 0)
    {
        if (u8_digit[largestDigit] >= 10) {
            u8_digit[largestDigit] -= 10;
            __ast_esos7SEGs.st_7seg[0].u8_charDisp = u8_digit[0] + 'A'; // add '0' to the digit to get the correct character
        }
        else {
            __ast_esos7SEGs.st_7seg[0].u8_charDisp = u8_digit[0] + '0'; // add '0' to the digit to get the correct character
        }
        for (int i = 1; i < 4; i++)
        {
            __ast_esos7SEGs.st_7seg[i].u8_charDisp = ' ';
        }
    }
    else
    {
        for (uint8_t i = 0; i <= largestDigit; i++)
        {
            if (u8_digit[i] > 10)
            {
                u8_digit[i] -= 10;
                u8_digit[i] += 'A';
            }
            else
            {
                u8_digit[i] += '0';
            }
            __ast_esos7SEGs.st_7seg[i].u8_charDisp = u8_digit[i]; // add '0' to the digit to get the correct character
        }
        if (isNegative)
        {
            __ast_esos7SEGs.st_7seg[largestDigit + 1].u8_charDisp = '-';
        }
        else
        {
            __ast_esos7SEGs.st_7seg[largestDigit + 1].u8_charDisp = ' ';
        }
        for (int i = largestDigit + 2; i < 4; i++)
        {
            __ast_esos7SEGs.st_7seg[i].u8_charDisp = ' ';
        }
    }
}
// *************************************************************************************************************************************
// *************************************************************************************************************************************
/**
 * @brief Will modify the ESOS 7-seg data structures accordingly
 * so that the hidden task will display the character u16_data as
 * a decimal # (right-justified) on the display.
 * @param u16_data
 */
void esos_7seg_writeU16Decimal(uint16_t u16_data) {
    esos_7seg_clearScreen();
    __ast_esos7SEGs.u8_state = ON; // set segs to ON.
    __ast_esos7SEGs.b_scrollMode = FALSE;
    uint8_t u8_digit[4] = { 0,0,0,0 };
    uint8_t largestDigit = 0;
    //Calculate the decimal representation and determine the largest digit
    for (int i = 0; i < 4; i++) {
        u8_digit[i] = u16_data % 10;
        if (u8_digit[i] != 0) {
            largestDigit = i;
        }
        u16_data = u16_data / 10;
    }
    // Write the digits to the display with blanks for the left-most '0' digits
    if (u8_digit[largestDigit] == 0) {
        __ast_esos7SEGs.st_7seg[0].u8_charDisp = u8_digit[0] + '0';   // add '0' to the digit to get the correct character
        for (int i = 1; i < 4; i++) {
            __ast_esos7SEGs.st_7seg[i].u8_charDisp = ' ';
        }
    }
    else {
        for (int i = 0; i <= largestDigit; i++) {
            __ast_esos7SEGs.st_7seg[i].u8_charDisp = u8_digit[i] + '0';   // add '0' to the digit to get the correct character
        }
        for (int i = largestDigit + 1; i < 4; i++) {
            __ast_esos7SEGs.st_7seg[i].u8_charDisp = ' ';
        }
    }
}
// *************************************************************************************************************************************
/**
 * @brief Will modify the ESOS 7-seg data structures accordingly
 * so that the hidden task will display the character u16_data as
 * a hexadecimal # (right-justified) on the display.
 * @param u16_data
 */
void esos_7seg_writeU16Hex(uint16_t u16_data)
{
    esos_7seg_clearScreen();
    __ast_esos7SEGs.u8_state = ON; // set segs to ON.
    __ast_esos7SEGs.b_scrollMode = FALSE;
    uint8_t u8_digit[4] = { 0,0,0,0 };
    uint8_t largestDigit = 0;
    //Calculate the hex representation and determine the largest digit
    for (int i = 0; i < 4; i++) {
        u8_digit[i] = u16_data % 16;
        if (u8_digit[i] != 0) {
            largestDigit = i;
        }
        u16_data = u16_data / 16;
    }
    // Write the digits to the display with blanks for the left-most '0' digits
    if (largestDigit == 0) {
        if (u8_digit[largestDigit] >= 10) {
            u8_digit[0] -= 10;
            u8_digit[0] += 'A';
        }
        else {
            u8_digit[largestDigit] += '0';
        }
        __ast_esos7SEGs.st_7seg[0].u8_charDisp = u8_digit[0];   // add '0' to the digit to get the correct character
        for (int i = 1; i < 4; i++) {
            __ast_esos7SEGs.st_7seg[i].u8_charDisp = ' ';
        }
    }
    else {
        for (int i = 0; i <= largestDigit; i++) {
            if (u8_digit[i] >= 10) {
                u8_digit[i] -= 10;
                u8_digit[i] += 'A';
            }
            else {
                u8_digit[i] += '0';
            }
            __ast_esos7SEGs.st_7seg[i].u8_charDisp = u8_digit[i];   // add '0' to the digit to get the correct character
        }
        for (int i = largestDigit + 1; i < 4; i++) {
            __ast_esos7SEGs.st_7seg[i].u8_charDisp = ' ';
        }
    }
}

// *************************************************************************************************************************************
//*******************************************************************************
/**
 * @brief This service will periodically scan the data structures created by your service and illuminate the appropriate segments
 * on the displays. The task will do this for each digit/char, in turn, rapidly enough
 * that the display appears to be constant.
 * You must determine the appropriate duration for each digit display to balance between
 * loading on the cpu and flickering observed by the human eye.\n
 * NOTE: Flickering can be detected by the human eye at < ~50-60Hz.
 * TODO: SETUP SPI SUPPORT FOR 7-SEG DISPLAYS , SETUP TIMER FOR 7-SEG DISPLAY SHIFTING, CHECK DATA STRUCTURES FOR ACTIONS TO PERFORM
 */
 //*******************************************************************************************************************************************************
ESOS_USER_TASK(__esos_7seg_service)
{
    static ESOS_7SEG_HANDLE h_7seg;
    static ESOS_TASK_HANDLE writeToSegmentChild;
    static uint32_t u32_now;         // current time
    static uint8_t u8_dispIndex = 0; // The character to be displayed now
    static uint8_t i = 0, j = 0;     // Loop counters
    static uint8_t scrollIndex = 0;  // Num scrolls
    static uint8_t seg3_data = 0, seg4_data = 0;
    ESOS_TASK_BEGIN(); // begin task
    //______________________________________________________________________________________________________________________
    while (TRUE)
    {
        u32_now = esos_GetSystemTick(); // get the current time
        u32_nowGlobal = u32_now;        // for string function call
        //_ _ _ _ _ __ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
        // Check the blinking and deal with it if necessary
        if (__ast_esos7SEGs.b_blinking == TRUE)
        {
            if (u32_now - __ast_esos7SEGs.u32_lastEventTime >= __ast_esos7SEGs.u32_blinkPeriod / 2)
            {
                __ast_esos7SEGs.u32_lastEventTime = u32_now;
                if (__ast_esos7SEGs.u8_state == ON)
                {
                    __ast_esos7SEGs.u8_state = OFF;
                }
                else if (__ast_esos7SEGs.u8_state == OFF)
                {
                    __ast_esos7SEGs.u8_state = ON;
                }
            }
        }
        // _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ __ _ _ _ _ __ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
        // Check the scroll mode and deal with it if necessary
        if (__ast_esos7SEGs.b_scrollMode == TRUE)
        {
            if (u32_now - __ast_esos7SEGs.u32_lastScrollEventTime >= __ast_esos7SEGs.u32_scrollDelay)
            {
                __ast_esos7SEGs.u32_lastScrollEventTime = u32_now;
                shiftBufferRight(__ast_esos7SEGs.linearBuffer, __ast_esos7SEGs.linearBuffer[4], 16);
            }
            for (i = 0; i < 4; i++)
            {
                __ast_esos7SEGs.st_7seg[i].u8_charDisp = __ast_esos7SEGs.linearBuffer[__ast_esos7SEGs.numChars + 4 - i];
                if ((__ast_esos7SEGs.u8_state == ON) && (__ast_esos7SEGs.st_7seg[i].u8_charDisp != ' '))
                {
                    ESOS_ALLOCATE_CHILD_TASK(writeToSegmentChild);
                    ESOS_TASK_SPAWN_AND_WAIT(writeToSegmentChild, seg7_hw_write_childTask, __ast_esos7SEGs.st_7seg[i].u8_segSelector, __ast_esos7SEGs.st_7seg[i].u8_charDisp);
                    ESOS_TASK_WAIT_TICKS(1);
                }
                else
                {
                    ESOS_ALLOCATE_CHILD_TASK(writeToSegmentChild);
                    ESOS_TASK_SPAWN_AND_WAIT(writeToSegmentChild, seg7_hw_write_childTask, __ast_esos7SEGs.st_7seg[i].u8_segSelector, ' ');
                }
            }
        }
        else {
            for (i = 0; i < 4; i++) {
                if ((__ast_esos7SEGs.u8_state == ON) && (__ast_esos7SEGs.st_7seg[i].u8_charDisp != ' ')) {
                    ESOS_ALLOCATE_CHILD_TASK(writeToSegmentChild);
                    ESOS_TASK_SPAWN_AND_WAIT(writeToSegmentChild, seg7_hw_write_childTask, __ast_esos7SEGs.st_7seg[i].u8_segSelector, __ast_esos7SEGs.st_7seg[i].u8_charDisp);
                    ESOS_TASK_WAIT_TICKS(1);
                }
                else {
                    ESOS_ALLOCATE_CHILD_TASK(writeToSegmentChild);
                    ESOS_TASK_SPAWN_AND_WAIT(writeToSegmentChild, seg7_hw_write_childTask, __ast_esos7SEGs.st_7seg[i].u8_segSelector, ' ');
                }
            }   // end for loop
        } // end for loop
        ESOS_TASK_YIELD();
        // _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ __ _ _ _ _ __ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
    } // END OF WHILE LOOP
    //___________________________________________________________________________
    ESOS_TASK_END(); // end task
}

/******************************************************************************
 * Function: void shiftBufferLeft(void)
 *
 * PreCondition: None
 *
 * Input: Pointer to the array to be shifted, data unsigned 8 bit data
 * to be pushed, and the size of the array.
 *
 * Output: None
 *
 * Side Effects: None
 *
 * Overview: The shiftBufferLeft(uint8_t* buffer, uint8_t PushByte, uint8_t size) function
 * is used to shift arrays throughout program operation.
 *
 * Note: None
 *
 *****************************************************************************/
void shiftBufferLeft(uint8_t* buffer, uint8_t PushByte, uint8_t size)
{
    uint8_t i;
    for (i = 0; buffer[i] != '\0'; i++)
    {
        buffer[i] = buffer[i + 1];
    }
}
//______________________________________________________________________________________________________________________
void shiftBufferRight(uint8_t* buffer, uint8_t PushByte, uint8_t size)
{
    uint8_t i;
    for (i = 63; i > 0; i--)
    {
        buffer[i] = buffer[i - 1];
    }
    buffer[0] = ' ';
    __ast_esos7SEGs.numChars--;
    __esos_unsafe_PutString((char*)buffer);
}

void esos_init_7seg_service(void)
{
    init_7seg_structs();                    // initialize the 7-seg structs
    seg7_SPI_hw_config();                   // Configure the SPI hardware
    esos_RegisterTask(__esos_7seg_service); // Register the 7-seg service
}

// *************************************************************************************************************************************
// END OF FILE