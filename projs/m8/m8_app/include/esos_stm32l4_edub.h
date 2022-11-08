#include "esos.h"
#include "esos_lcd44780wo.h"
#include "libopencm3/stm32/gpio.h"

#ifndef ESOS_STM32L4_EDUB_H
#define ESOS_STM32L4_EDUB_H


//*  7-Segment Display Hardware Defines
#define EDUB_SPICS7_PORT GPIOC  // SPI1 Chip Select for 7-Segment Display
#define EDUB_SPICS7_PIN GPIO10  // GPIOC10

#define EDUB_SPIMOSI_PORT GPIOB   // GPIOB5
#define EDUB_SPIMOSI_PIN GPIO5

#define EDUB_SPIMISO_PORT GPIOA    // GPIOA6
#define EDUB_SPIMISO_PIN GPIO6

#define EDUB_SPICLK_PORT GPIOA    // GPIOA1
#define EDUB_SPICLK_PIN GPIO5






#endif // !ESOS_STM32L4_EDUB_H
