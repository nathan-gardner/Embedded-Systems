/*
*************************************************************************************************************************
* File        - main.h
* Author      - Blake Martin
* Data        - 03-06-2022
* Description - Primary header file for main.c, encryption.c, and user_functions.c

* Active Peripherals:
         GPIO C13 - Button B1
         GPIO A2  - USART2 Rx
         GPIO A3  - USART2 Tx
         GPIO A5  - LED2
*************************************************************************************************************************
*/

#include "circularBuffer.h"
#include <inttypes.h>
#include <stdbool.h>

#ifndef MAIN_H
#define MAIN_H

//*************************************************************************************************************************
// GLOBAL VARIABLES:
extern uint8_t au8_key[];              // Encryption Key
extern bool b_currentButtonState;      // State of push-button B1
extern uint32_t u32_tim2_period;       // period of tim2
extern uint16_t u16_exti_direction;    //Direction of EXTI trigger

//*************************************************************************************************************************
// USER-DEFINED FUNCTIONS
//_____________________________ PERIPHERAL SETUP FUNCTIONS: _______________________________________________________________
void clock_setup(void);                      // Initialize system clock
void exti_button_setup(void);                // Initialize button C13 as External interrupt
void usart_setup(void);                      // Initialize USART2/USART_CONSOLE
void tim2_setup(void);                       // Initialize TIM2 for LED Toggling
void tim7_setup(void);                       // Initialize TIM7 for button debouncing

//_____________________________ USER-DEFINED HELPING FUNCTIONS: ___________________________________________________________
void rotateKeyLeft(void);                    // Rotates the global encryption key to the left by 1 space
void rotateKeyLeftBy(uint16_t num);          // Rotates global encryption key num times
void resetKey(void);                         // Resets the global encryption key to "TENNESSEETECH"
void set_LED_period(uint16_t newLedPeriod);  // Sets period of LED with parameter - desired LED period in ms
void delayOPs(uint32_t ops);                 // Delays using __asm("nop") ops times
uint8_t encrypt(uint8_t inByte);             // Encryption algorithm
uint8_t decrypt(uint8_t inByte);             // Decryption algorithm
uint16_t a2i(uint8_t txt[4], uint8_t base);                // Converts ascii string to integer
uint8_t isAlpha(uint8_t input);              // checks if char is Alphanumeric

//Struct for managing button state / debouncing
typedef struct button_state_t button_state_t;
struct button_state_t {
    uint8_t falling;
    uint8_t last_hold;
    uint8_t tickcount;
};
//*************************************************************************************************************************
//USART2 DEFINES & MACROS
#define USART_CONSOLE USART2  // NOTES ON USART2: (GPIOA2 -> Rx), (GPIOA3 -> Tx), (AF7-> usart2 (Rx & Tx))
#define DATABITS = 8          // 8 data bits
#define ODDBITS = 1           // 1 odd bit
//*************************************************************************************************************************
//TIM2 DEFINES & MACROS
#define TIMER_DEFAULT_PERIOD  9999 // TIM2 Default Frequency = 9999

//*************************************************************************************************************************
// Used in usart_send_message()
#define ENDLINE               1   // Bool signaling to not endline in usart_send_message()
#define DONTENDLINE           0   // Bool signaling to end line in usart_send_message()
//*************************************************************************************************************************

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
