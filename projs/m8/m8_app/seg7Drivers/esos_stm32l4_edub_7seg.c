#include "esos.h"
#include "esos_stm32l4_edub_7seg.h"
#include "esos_stm32l4_edub.h"
#include "esos_7seg.h"
#include "circularBuffer.h"

//SPI INCLUDES:
#include "esos_spi.h"
#include "esos_stm32l4_spi.h"
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/gpio.h>

#ifndef STM32L4
#define STM32L4
#endif // !STM32L4

#ifndef ESOS_USE_SPI
#define ESOS_USE_SPI
#endif

/** 
 * @brief Configure all 4 7seg Displays
 * @return void
 */
void seg7_SPI_hw_config(){
        // Configure HW Chip sel for SPI1->EDUB   
        __esos_spi_config(10000);
        gpio_mode_setup(EDUB_SPICS7_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, EDUB_SPICS7_PIN);          // Enable CS7 as output
        gpio_set_output_options(EDUB_SPICS7_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, EDUB_SPICS7_PIN); // Set CS7 as high-speed push-pull output
        gpio_set(EDUB_SPICS7_PORT, EDUB_SPICS7_PIN); 
        gpio_clear(EDUB_SPICS7_PORT, EDUB_SPICS7_PIN);   
        gpio_set(EDUB_SPICS7_PORT, EDUB_SPICS7_PIN);
}       

//*********************************************************************************************************

ESOS_CHILD_TASK(seg7_hw_write_childTask, uint8_t u8_disp, uint8_t u8_pattern){
        static uint16_t u16_disp2 = 0, u16_pattern2 = 0;
        u16_disp2 = u8_disp;
        u16_pattern2 = u8_pattern;
        ESOS_TASK_BEGIN();
        // _________________________________________________________________
        /* Verified that we are getting into this loop each time. */
        if(u16_pattern2 == '0'){
                u16_pattern2 = PATTERN_0;
        }
        else if(u16_pattern2 == '1'){
                u16_pattern2 = PATTERN_1;
        }
        else if(u16_pattern2 == '2'){
                u16_pattern2 = PATTERN_2;
        }
        else if(u16_pattern2 == '3'){
                u16_pattern2 = PATTERN_3;
        }
        else if(u16_pattern2 == '4'){
                u16_pattern2 = PATTERN_4;
        }
        else if(u16_pattern2 == '5'){
                u16_pattern2 = PATTERN_5;
        }
        else if(u16_pattern2 == '6'){
                u16_pattern2 = PATTERN_6;
        }
        else if(u16_pattern2 == '7'){
                u16_pattern2 = PATTERN_7;
        }
        else if(u16_pattern2 == '8'){
                u16_pattern2 = PATTERN_8;
        }
        else if(u16_pattern2 == '9'){
                u16_pattern2 = PATTERN_9;
        } 
        else if(u16_pattern2 == 'A' || u16_pattern2 == 'a'){
                u16_pattern2 = PATTERN_A;
        }
        else if (u16_pattern2 == 'B' || u16_pattern2 == 'b'){
                u16_pattern2 = PATTERN_B;
        }
        else if(u16_pattern2 == 'C' || u16_pattern2 == 'c'){
                u16_pattern2 = PATTERN_C;
        }
        else if(u16_pattern2 == 'D' || u16_pattern2 == 'd'){
                u16_pattern2 = PATTERN_D;
        }
        else if (u16_pattern2 == 'E' || u16_pattern2 == 'e'){
                u16_pattern2 = PATTERN_E;
        }
        else if(u16_pattern2 == 'F' || u16_pattern2 == 'f'){
                u16_pattern2 = PATTERN_F;
        }
         else if(u16_pattern2 == 'G' || u16_pattern2 == 'g'){
                u16_pattern2 = PATTERN_G;
        }
        else if(u16_pattern2 == 'H' || u16_pattern2 == 'h'){
                u16_pattern2 = PATTERN_H;
        }
        else if (u16_pattern2 == 'I' || u16_pattern2 == 'i'){
                u16_pattern2 = PATTERN_I;
        }
        else if(u16_pattern2 == 'J' || u16_pattern2 == 'j'){
                u16_pattern2 = PATTERN_J;
        }
        else if(u16_pattern2 == 'K' || u16_pattern2 == 'k'){
                u16_pattern2 = PATTERN_K;
        }
         else if(u16_pattern2 == 'L' || u16_pattern2 == 'l'){
                u16_pattern2 = PATTERN_L;
        }
        else if(u16_pattern2 == 'M' || u16_pattern2 == 'm'){
                u16_pattern2 = PATTERN_M;
        }
        else if (u16_pattern2 == 'N' || u16_pattern2 == 'n'){
                u16_pattern2 = PATTERN_N;
        }
        else if(u16_pattern2 == 'O' || u16_pattern2 == 'o'){
                u16_pattern2 = PATTERN_O;
        }
        else if(u16_pattern2 == 'P' || u16_pattern2 == 'p'){
                u16_pattern2 = PATTERN_P;
        }
         else if(u16_pattern2 == 'Q' || u16_pattern2 == 'q'){
                u16_pattern2 = PATTERN_Q;
        }
        else if(u16_pattern2 == 'R' || u16_pattern2 == 'r'){
                u16_pattern2 = PATTERN_R;
        }
        else if (u16_pattern2 == 'S' || u16_pattern2 == 's'){
                u16_pattern2 = PATTERN_S;
        }
        else if(u16_pattern2 == 'T' || u16_pattern2 == 't'){
                u16_pattern2 = PATTERN_T;
        }
        else if(u16_pattern2 == 'U' || u16_pattern2 == 'u'){
                u16_pattern2 = PATTERN_U;
        }
         else if(u16_pattern2 == 'V' || u16_pattern2 == 'v'){
                u16_pattern2 = PATTERN_V;
        }
        else if(u16_pattern2 == 'W' || u16_pattern2 == 'w'){
                u16_pattern2 = PATTERN_W;
        }
        else if (u16_pattern2 == 'X' || u16_pattern2 == 'x'){
                u16_pattern2 = PATTERN_X;
        }
        else if(u16_pattern2 == 'Y' || u16_pattern2 == 'y'){
                u16_pattern2 = PATTERN_Y;
        }
        else if(u16_pattern2 == 'Z' || u16_pattern2 == 'z'){
                u16_pattern2 = PATTERN_Z;
        }
        
        else if(u16_pattern2 == '-'){
                u16_pattern2 = PATTERN_DASH;
        }
        else if(u16_pattern2 == '_'){
                u16_pattern2 = PATTERN_UNDERSCORE;
        }
        else{
                u16_pattern2 = PATTERN_BLANK;
        }
        ESOS_TASK_WAIT_ON_AVAILABLE_SPI();
        gpio_clear(EDUB_SPICS7_PORT, EDUB_SPICS7_PIN);
        ESOS_TASK_WAIT_ON_WRITE2SPI1(u16_pattern2,u16_disp2);// Send the 16 bit word to the shift register
        gpio_set(EDUB_SPICS7_PORT, EDUB_SPICS7_PIN);   // Set CS7 to high to close SPI Channel
        ESOS_SIGNAL_AVAILABLE_SPI();
        ESOS_TASK_END();
}

//*********************************************************************************************************
// NOTES:


//*********************************************************************************************************