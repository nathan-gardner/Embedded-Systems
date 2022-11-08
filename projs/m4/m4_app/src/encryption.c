/*******************************************************************************
 *
 *               Vigenere Cipher Encryption Library Version 1.1
 *
 *******************************************************************************
 * FileName:       encryption.c
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
 * Blake Martin          3/17/22               Original
 ******************************************************************************/
#include "main.h"
#include <stdbool.h>

#define  NUM_LETTERS_IN_ALPHABET  26

/*******************************************************************************
 * Function:         void rotateKeyLeft()
 *
 * PreCondition:     None
 *
 * Input:            None
 *
 * Output:           None
 *
 * Side Effects:     None
 *
 * Overview:         Rotates the key stored au8_key[] "to the left" by
 *                   1 character, i.e. "PATROL" becomes "ATROPL"
 *
 * Note:             FIXME: Change function name to rotate_key_left()
 *
 ******************************************************************************/

void rotateKeyLeft(void){
    int8_t i8_length = 13;   // length of Vigenere cipher key - FIXME: uint_8? macro?
    int8_t j;                // FIXME: change to i8_j, maybe change to uint8_t?
    uint8_t temp = au8_key[0];
    for(j = 0; j < i8_length-1; j++){
        //shift to the left
        au8_key[j] = au8_key[j+1];
    }
    au8_key[j] = temp;
}

/*******************************************************************************
 * Function:         void rotateKeyLeftBy(uint16_t num)
 *
 * PreCondition:     None
 *
 * Input:            None
 *
 * Output:           None
 *
 * Side Effects:     None
 *
 * Overview:         Rotates the key stored au8_key[] "to the left" by
 *                   the number of characters given,
 *                   i.e. "PATROL" shifted by 4 becomes "OLPATR"
 *
 * Note:             FIXME: Change function name to rotate_key_left_by()
 *
 ******************************************************************************/

void rotateKeyLeftBy(uint16_t num){
    for(int i = 0;i < num; i++){
        rotateKeyLeft();
    }
}

/*******************************************************************************
 * Function:         void encypher(uint8_t u8_inChar)
 *
 * PreCondition:     u8_inByte must be an ASCII character corresponding to
 *                   a letter in the alphabet from 'a' to 'Z' and can
 *                   either lowercase or uppercase
 *
 * Input:            uint8_t u8_InLetter- ASCII character to be
 *                                        encrypted/decrypted
 *
 * Output:           uint8_t u8_OutLetter - encrypted/decrypted ASCII character
 *
 * Side Effects:     None
 *
 * Overview:         Capitalizes the given ASCII letter and then
 *                   applies a Vigenere cipher to the provided byte
 *                   character as it shifts the given character
 *                   forward or reverse in the alphabet by the number of number
 *                   of letters corresponding to the current indexed
 *                   letter in the cipher key (i.e. A - shift by 0,
 *                   B - shift by 1, etc.), based on if B1 is pressed
 *
 * Note:             This Vigenere cipher algorithm only applies to
 *                   ASCII letters corresponding to capitalized 'A' to 'Z.'
 *                   However, u8_InLetter is capitalized at the start of
 *                   the algorithm.
 *
 ******************************************************************************/

uint8_t encypher(uint8_t u8_InLetter) {

    uint8_t u8_OutLetter;                // return value of function
    u8_InLetter = toupper(u8_InLetter);  // ensures letter is capitalized

    /*
     * Encipher u8_inLetter if button is not pressed and
     * decipher u8_outLetter if button is pressed
     */
    if(b_currentButtonState == false) {
        /*
         * Encrypt the provided letter using the Vigenere cipher method by
         * shifting the given letter forward in the alphabet by the number
         * of shifts corresponding to the current indexed letter in the key.
         * Ex. A -> E = shift A right 4 letters, or shift A left 22 letters
         */
        u8_OutLetter = ((u8_InLetter + au8_key[0]) % NUM_LETTERS_IN_ALPHABET) + 'A';
        rotateKeyLeft();
        return u8_OutLetter;
    }
    else if(b_currentButtonState == true) {
        /*
         * Decrypt the provided letter using the Vigenere cipher method by
         * shifting the given letter backward in the alphabet by the number
         * of shifts corresponding to the current indexed letter in the key.
         * Ex. E -> A = shift E left 4 letters, or shift E right 22 letters
         */
        u8_OutLetter = ( ((u8_InLetter - au8_key[0]) + NUM_LETTERS_IN_ALPHABET)
                           % NUM_LETTERS_IN_ALPHABET) + 'A';
        rotateKeyLeft();
        return u8_OutLetter;
    }
    else {
        return -1;
    }
}

void resetKey(void){
    uint8_t au8_tempKey[] = "TENNESSEETECH";
    for(int i = 0; i < 13; i++){
        au8_key[i] = au8_tempKey[i];
    }
}

//_____________________________________________________________________________________________________________________________
/**
 * @brief Convert 4-bit ASCII string to integer
 *
 * @param txt
 * @return uint32_t
 */
uint32_t a2i(uint8_t txt[4])
{
    int sum, digit, i;
    sum = 0;
    for (i = 0; i < 4; i++) {
		if(txt[i] != '0'){
        	digit = txt[i] - 0x30;
		}
		else{
			digit = 0;
		}
        sum = (sum * 10) + digit;
    }
    return sum;
}
