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
 * \addtogroup ESOS_SPI_Service
 * @{
 */



// Documentation for this file. If the \file tag isn't present,
// this file won't be documented.
/** \file
* \brief Template for creating hardware-specific support for ESOS32 
* SPI communications service
*/


/*** I N C L U D E S *************************************************/
#include "esos_hwxxx_spi.h"

/*** G L O B A L S *************************************************/

/*** T H E   C O D E *************************************************/

/*********************************************************
 * Public functions intended to be called by other files *
 *********************************************************/

// Documentation for this file. If the \file tag is not present,
// this file will not be documented.
// Note: place this comment below the #if NUM_I2C_MODS so Doxygen
// will only see it once.
/** \file
 *  SPI support functions. \see pic24_spi.h for details.
 */

/*
Transaction: Does hardware configuration to setup the SPI
periperal given the SPI transfer bitrate \em u32_spibps.
\note The SPI peripheral setup determines whether 8-bit or 16-bit data
is written.
 */
extern void __esos_spi_hw_config(uint32_t u32_spibps){
	// write the required hardware code to setup the SPI peripheral
	
}  // end __esos_spi_hw_config()

/*
Transaction: Writes \em u16_cnt words stored in
buffer \em *pu16_out to SPI device, while reading \em u16_cnt words from
SPI device placing results into buffer \em *pu16_in
\note Assumes that SPI peripheral has been properly configured.
\note The SPI peripheral setup determines whether 8-bit or 16-bit data
is written.
\param pu16_out Pointer to buffer containing data to send.  If \em pu16_out is \em NULLPTR this function will send zeroes to the SPI device and only "read"
\param pu16_in Pointer to buffer to catch incoming data.  If \em pu16_in is \em NULLPTR this function will only "write" the SPI device
\param u16_cnt Number of words to send
 */
ESOS_CHILD_TASK( __esos_hw_xferNSPI, uint16_t* pu16_out, uint16_t* pu16_in, uint16_t u16_cnt) {

  ESOS_TASK_BEGIN();

	// write the hardware-specific code to implement the
	// functional requirements given above

  ESOS_TASK_END();
} // end __esos_hw_xferNSPI

/**
 * @}
 */
