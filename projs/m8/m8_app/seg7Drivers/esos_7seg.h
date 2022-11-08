/*
This files contains the macros and service variables 
for the seven-segment display service.
esos_7seg files are programming API files.
*/
#ifndef ESOS_7SEG_H
#define ESOS_7SEG_H
#include "esos.h"
#include "esos_cb.h"
#include <circularBuffer.h>

#ifndef STM32L4
#define STM32L4
#endif // !STM32L4

#ifndef ESOS_USE_SPI
#define ESOS_USE_SPI
#endif
//*******************************************************************************************************************************************************
//*******************************************************************************************************************************************************

typedef enum SEG7_b_SCROLLMODE {
    SMOOTH = 0,
    FAST,
    NONE,
} scroll_mode_t; 

//*******************************************************************************************************************************************************
// STRUCTURES GO HERE:
/**
 * @brief Structure for individual 7-segment display
 * Member - u8_segSelector: Selects which segment to display
 * Member - u8_charDisp : The character to be displayed
 */
typedef struct {
    uint8_t u8_segSelector;
    uint8_t u8_charDisp;        // The character to be displayed now
} st_esos_7seg;
//_______________________________________________________________________________________________________________
/**
 * @brief Wrapper containing all 4 7-segment display structs and circular buffer for managing the displays
 */
typedef struct {
    st_esos_7seg st_7seg[4];         // 4 7-segment displays

    BOOL b_blinking;             // current blinking state
    uint32_t u32_lastEventTime;  // time of last event
    uint32_t u32_blinkPeriod;    // The period of the flashing
    BOOL u8_state;               // The current state of the display (ON/OFF for blinking purposes)

    circular_buffer_t* cb_7seg;  // circular buffer for managing chars to the 7seg

    BOOL b_scrollMode;           // scroll mode for the 7seg ON/OFF
    uint32_t u32_scrollDelay;    // delay between characters in the scroll mode
    scroll_mode_t scrollType;    // scroll mode for the 7seg ON/OFF

    BOOL b_stopAtEdge;

} st_esos_7seg_wrapper;
//_______________________________________________________________________________________________________________
extern volatile st_esos_7seg_wrapper  __ast_esos7SEGs;  

typedef uint8_t ESOS_7SEG_HANDLE;

//*******************************************************************************************************************************************************
void esos_init_7seg_service(void);
void esos_7seg_reen(void);
void esos_7seg_writeCharacter(uint8_t u8_column, uint8_t u8_data);
void esos_7seg_writeBuffer(uint8_t* pu8_data, uint8_t u8_bufflen);
char* esos_7seg_getDisplay(void);
void esos_7seg_writeString(char* psz_data);
void esos_7seg_blink(BOOL b_state, uint32_t u32_period);
void esos_7seg_writeu8Decimal(uint8_t u8_data);
void esos_7seg_writeU8Hex(uint8_t u8_data);
void esos_7seg_writeI8Decimal(int8_t i8_data);
void esos_7seg_writeI8Hex(int8_t i8_data);
void esos_7seg_writeU16Decimal(uint16_t u16_data);
void esos_7seg_writeU16Hex(uint16_t u16_data);
void shiftBufferLeft(uint8_t* buffer, uint8_t PushByte, uint8_t size);
void esos_7seg_takeNumericInput(uint8_t* u8_data);
//______________________________________________________________________________________________________________
// PUBLIC HARDWARE PROTOTYPES:
extern void seg7_SPI_hw_config();


// PRIVATE FUNCTION PROTOTYPES GO HERE:
ESOS_USER_TASK(__esos_7seg_service);



#endif
