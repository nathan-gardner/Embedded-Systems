/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_APP_H
#define __USER_APP_H

#ifdef __cplusplus
extern "C" {
#endif


#include <inttypes.h>

/* Exported functions prototypes ---------------------------------------------*/
    void Error_Handler(void);

/* Private defines -----------------------------------------------------------*/
#ifdef __PTB_APOLLO_BOARD
#define SW1_Pin GPIO13
#define SW1_GPIO_Port GPIOC
#define USART_TX_Pin GPIO2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO3
#define USART_RX_GPIO_Port GPIOA
#define LED2_Pin GPIO5
#define LED2_GPIO_Port GPIOA
#define TMS_Pin GPIO13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO3
#define SWO_GPIO_Port GPIOB


#define LEFT_BTN_PORT GPIOC
#define LEFT_BTN_PIN  GPIO7
#define RIGHT_BTN_PORT  GPIOA
#define RIGHT_BTN_PIN GPIO8
#endif



#ifdef __NUCLEO_BOARD

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

#endif

#ifdef __EDUB_BOARD
#define SW1_Pin GPIO8
#define SW1_GPIO_Port GPIOB
#define USART_TX_Pin GPIO2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO3
#define USART_RX_GPIO_Port GPIOA
#define LED2_Pin GPIO7
#define LED2_GPIO_Port GPIOC
#endif

#ifdef __cplusplus
}
#endif

#endif /* __USER_APP_H */
