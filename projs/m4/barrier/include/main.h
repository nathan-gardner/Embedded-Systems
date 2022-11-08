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

#include "esos.h"
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


#ifndef MAIN_H
#define MAIN_H

//*************************************************************************************************************************
// GLOBAL VARIABLES:
#define NUMTHREADS 5
extern uint8_t au8_key[];              // Encryption Key
extern bool b_currentButtonState;      // State of push-button B1
extern uint32_t u32_tim2_period;       // period of tim2
extern uint16_t u16_exti_direction;    //Direction of EXTI trigger

void __user_init_hw(void);

extern ESOS_SEMAPHORE(mutex);
extern ESOS_SEMAPHORE(barrier);
//*************************************************************************************************************************
// Thread Declarations:
ESOS_USER_TASK(thread1);
ESOS_USER_TASK(thread2);
ESOS_USER_TASK(thread3);
ESOS_USER_TASK(thread4);
ESOS_USER_TASK(thread5);
ESOS_USER_TASK(thread6);
ESOS_USER_TASK(thread7);
ESOS_USER_TASK(thread8);
ESOS_USER_TASK(thread9);
ESOS_USER_TASK(thread10);
ESOS_USER_TASK(thread11);
ESOS_USER_TASK(thread12);
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
uint8_t encypher(uint8_t inByte);            // Encryption/Decryption function (Depends on b_currentButtonState value)
uint32_t a2i(uint8_t txt[4]);                // Converts ascii string to integer

//*************************************************************************************************************************
// LED2 DEFINES & MACROS
#define LED2_PORT           GPIOA
#define LED2_PIN            GPIO5
#define LED2_SETUP()        gpio_mode_setup(LED2_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED2_PIN)
#define LED2_TOGGLE()       gpio_toggle(LED2_PORT,LED2_PIN)
#define LED2_SET()          gpio_set(LED2_PORT, LED2_PIN)
#define LED2_CLEAR()        gpio_clear(LED2_PORT, LED2_PIN)
#define LED2_GETVAL()       gpio_get(LED2_PORT, LED2_PIN)
//*************************************************************************************************************************
// BUTTON B1 DEFINES & MACROS
#define BUTTON_PORT          GPIOC
#define BUTTON_PIN           GPIO13
#define BUTTON_ENABLE()      gpio_mode_setup(BUTTON_PORT,GPIO_MODE_INPUT,GPIO_PUPD_NONE,BUTTON_PIN)
#define BUTTON_PUSHED()      gpio_get(BUTTON_PORT,BUTTON_PIN)

#define BUTTON_EXTI          EXTI13
#define BUTTON_NVIC          13

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
