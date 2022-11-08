/*
 * "Copyright (c) 2019 J. W. Bruce ("AUTHOR(S)")"
 * All rights reserved.
 * (J. W. Bruce, jwbruce_AT_tntech.edu, Tennessee Tech University)
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice, the following
 * two paragraphs and the authors appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE "AUTHORS" BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE "AUTHORS"
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE "AUTHORS" SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE "AUTHORS" HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Please maintain this header in its entirety when copying/modifying
 * these files.
 *
 *
 */


 /**
  * \addtogroup ESOS_Simple_User_Interface_Service
  * @{
  */

  // Documentation for this file. If the \file tag isn't present,
  // this file won't be documented.
  /**
  * /** \file
  * \brief Template for creating hardware-specific support for the
  * ESOS32 watchdog service
  */

  /*** I N C L U D E S *************************************************/
#include "esos_stm32l4_edub_sui.h"
#include "esos_stm32l4.h"
#include "esos_sui.h"
#include "esos.h"


 /**
 * ESOS callback to user to perform hardware initialization for
 * connected LEDs
 *
 * \param h_LED ESOS_SUI_LED_HANDLE to switch that was returned to user
 * when the LED was registered
 *
 * \sa esos_sui_registerLED()
 * \sa esos_hw_sui_turnLEDOn()
 * \sa esos_hw_sui_turnLEDOff()
 * \hideinitializer
 */
void esos_hw_sui_configLED(ESOS_SUI_LED_HANDLE h_led) {
    rcc_periph_clock_enable(esos_sui_getLEDUserData1(h_led));
    gpio_mode_setup(esos_sui_getLEDUserData1(h_led), GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, esos_sui_getLEDUserData2(h_led));
}

/**
* ESOS callback to user to do whatever hardware gymnastics is required
* to turn "ON" a connected LED
*
* \param h_LED ESOS_SUI_LED_HANDLE to switch that was returned to user
* when the LED was registered
*
* \sa esos_sui_registerLED()
* \sa esos_hw_sui_configLED()
* \sa esos_hw_sui_turnLEDOff()
* \hideinitializer
*/
void esos_hw_sui_turnLEDOn(ESOS_SUI_LED_HANDLE h_led) {
    gpio_set(esos_sui_getLEDUserData1(h_led), esos_sui_getLEDUserData2(h_led));
}

void esos_hw_sui_toggleLED(ESOS_SUI_LED_HANDLE h_led) {
    gpio_toggle(esos_sui_getLEDUserData1(h_led), esos_sui_getLEDUserData2(h_led));
}

/**
* ESOS callback to user to do whatever hardware gymnastics is required
* to turn "OFF" a connected LED
*
* \param h_led ESOS_SUI_LED_HANDLE to switch that was returned to user
* when the LED was registered
*
* \sa esos_sui_registerLED()
* \sa esos_hw_sui_configLED()
* \sa esos_hw_sui_turnLEDOn()
* \hideinitializer
*/
void esos_hw_sui_turnLEDOff(ESOS_SUI_LED_HANDLE h_led) {
    gpio_clear(esos_sui_getLEDUserData1(h_led), esos_sui_getLEDUserData2(h_led));
}

/**
* ESOS callback to user to perform hardware initialization for
* switches
*
* \param h_sw ESOS_SUI_SWITCH_HANDLE to switch that was returned to user
* when the switch was registered
*
* \sa esos_sui_registerSwitch()
* \sa esos_hw_sui_isSwitchPressed()
* \sa esos_hw_sui_isSwitchReleased()
* \hideinitializer
*/
void esos_hw_sui_configSwitch(ESOS_SUI_SWITCH_HANDLE h_sw) {
    rcc_periph_clock_enable(esos_sui_getSwitchUserData1(h_sw));
    gpio_mode_setup(esos_sui_getSwitchUserData1(h_sw), GPIO_MODE_INPUT, GPIO_PUPD_NONE, esos_sui_getSwitchUserData2(h_sw));
}

/**
* ESOS callback to user to answer whether switch is pressed
*
* \param h_sw ESOS_SUI_SWITCH_HANDLE to switch that was returned to user
* when the switch was registered
* \retval TRUE if switch is pressed, FALSE if it is not pressed
*
* \sa esos_sui_registerSwitch()
* \sa esos_hw_sui_isSwitchPressed()
* \sa esos_hw_sui_isSwitchReleased()
* \hideinitializer
*/
BOOL esos_hw_sui_isSwitchPressed(ESOS_SUI_SWITCH_HANDLE h_sw) {
    if (gpio_get(esos_sui_getSwitchUserData1(h_sw), esos_sui_getSwitchUserData2(h_sw)) != 0) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

/**
* ESOS callback to user to answer whether switch is pressed
*
* \param h_sw ESOS_SUI_SWITCH_HANDLE to switch that was returned to user
* when the switch was registered
* \retval TRUE if switch is released (not pressed), FALSE if it is pressed
*
* \sa esos_sui_registerSwitch()
* \sa esos_hw_sui_isSwitchPressed()
* \sa esos_hw_sui_isSwitchReleased()
* \hideinitializer
*/
BOOL esos_hw_sui_isSwitchReleased(ESOS_SUI_SWITCH_HANDLE h_sw) {
    if (gpio_get(esos_sui_getSwitchUserData1(h_sw), esos_sui_getSwitchUserData2(h_sw)) != 0) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}
