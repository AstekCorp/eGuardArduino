/**
 * \file
 * \brief
 *
 * Copyright (c) 2016 Astek Corporation. All rights reserved.
 *
 * \astek_eguard_library_license_start
 *
 * \page eGuard_License_Derivative
 *
 * The source code contained within is subject to Astek's eGuard licensing
 * agreement located at: https://www.astekcorp.com/
 *
 * The eGuard product may be used in source and binary forms, with or without
 * modifications, with the following conditions:
 *
 * 1. The source code must retain the above copyright notice, this list of
 *    conditions, and the disclaimer.
 *
 * 2. Distribution of source code is not authorized.
 *
 * 3. This software may only be used in connection with an Astek eGuard
 *    Product.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS NOTICE
 * DO NOT WARRANT THAT THE FUNCTIONS CONTAINED IN THE SOFTWARE WILL MEET YOUR
 * REQUIREMENTS OR THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR
 * ERROR FREE. ANY USE OF THE SOFTWARE SHALL BE MADE ENTIRELY AT THE USER'S OWN
 * RISK. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR ANY CONTRIUBUTER OF
 * INTELLECTUAL PROPERTY RIGHTS TO THE SOFTWARE PROPERTY BE LIABLE FOR ANY
 * CLAIM, OR ANY DIRECT, SPECIAL, INDIRECT, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM ANY ALLEGED INFRINGEMENT
 * OR ANY LOSS OF USE, DATA, OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE, OR UNDER ANY OTHER LEGAL THEORY, ARISING OUT OF OR IN
 * CONNECTION WITH THE IMPLEMENTATION, USE, COMMERCIALIZATION, OR PERFORMANCE
 * OF THIS SOFTWARE.
 *
 * The following license file is included for completeness of documentation. 
 * This file is a derivative work owned by Astek and is also subject to Astek's
 * eGuard License agreement at https://www.astekcorp.com/
 *
 * \astek_eguard_library_license_stop
 */
/**
 * \file
 * \brief low-level HAL - methods used to setup indirection to physical layer interface
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
 *
 * \atmel_crypto_device_library_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel integrated circuit.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \atmel_crypto_device_library_license_stop
 */


#ifndef ATCA_HAL_H_
#define ATCA_HAL_H_

#include "atca_status.h"
#include "atca_iface.h"
#include "atca_start_config.h"
#include "atca_start_iface.h"

/** \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 *
   @{ */

/** \brief an intermediary data structure to allow the HAL layer to point the standard API functions
   used by the upper layers to the HAL implementation for the interface.  This isolates the upper layers
   and loosely couples the ATCAIface object from the physical implementation.
 */

typedef struct {
	// interface is a group of function pointers to a specific HAL implementation for this interface type
	// so these function pointers are initialized in the HAL layer in order to help keep the ATCAIface object
	// from needing to know the low-level details, including global naming of HAL methods and physical implementation.
	ATCA_STATUS (*halinit)(void *hal, ATCAIfaceCfg *cfg);
	ATCA_STATUS (*halpostinit)(ATCAIface iface);
	ATCA_STATUS (*halsend)(ATCAIface iface, uint8_t *txdata, uint16_t txlength);
	ATCA_STATUS (*halreceive)(ATCAIface iface, uint8_t* rxdata, uint16_t* rxlength);
	ATCA_STATUS (*halwake)(ATCAIface iface);
	ATCA_STATUS (*halidle)(ATCAIface iface);
	ATCA_STATUS (*halsleep)(ATCAIface iface);
	ATCA_STATUS (*halrelease)(void* hal_data);

	void *hal_data;       // points to whatever the HAL implementation for this interface wants it to, HAL manages.
} ATCAHAL_t;

#ifdef __cplusplus
extern "C" {
#endif

extern ATCA_STATUS hal_iface_init(ATCAIfaceCfg *cfg, ATCAHAL_t *hal);
extern ATCA_STATUS hal_iface_release(ATCAIfaceType ifacetype, void *hal_data);

// Added one or more of the following defines to your compiler's defines to include add support for
// that particular interface in your application. For example, if you're writing an I2C to SWI
// bridge, add both ATCA_HAL_I2C and ATCA_HAL_SWI defines to your compiler settings and then
// include implementations for both interfaces in the HAL.

// At least one of these symbols will be defined in the project or makefile for each application
//ATCA_HAL_I2C
//ATCA_HAL_SWI
//ATCA_HAL_SPI
//ATCA_HAL_UART
//ATCA_HAL_KIT_HID
//ATCA_HAL_KIT_CDC

//If nothing defined than use I2C
#ifndef ATCA_HAL_I2C
	#ifndef ATCA_HAL_SWI
		#ifndef ATCA_HAL_UART
			#ifndef ATCA_HAL_KIT_CDC
				#ifndef ATCA_HAL_KIT_HID
					#define ATCA_HAL_I2C
				#endif
			#endif
		#endif
	#endif
#endif

// forward declare known physical layer APIs that must be implemented by the HAL layer (./hal/xyz) for this interface type

#ifdef ATCA_HAL_I2C
ATCA_STATUS hal_i2c_init( void *hal, ATCAIfaceCfg *cfg);
ATCA_STATUS hal_i2c_post_init(ATCAIface iface);
ATCA_STATUS hal_i2c_send(ATCAIface iface, uint8_t *txdata, uint16_t txlength);
ATCA_STATUS hal_i2c_receive( ATCAIface iface, uint8_t *rxdata, uint16_t *rxlength);
ATCA_STATUS hal_i2c_wake(ATCAIface iface);
ATCA_STATUS hal_i2c_idle(ATCAIface iface);
ATCA_STATUS hal_i2c_sleep(ATCAIface iface);
ATCA_STATUS hal_i2c_release(void *hal_data );
ATCA_STATUS hal_i2c_discover_buses(uint16_t i2c_buses[], uint16_t max_buses);
ATCA_STATUS hal_i2c_discover_devices(uint16_t busNum, ATCAIfaceCfg cfg[], uint16_t *found );
//Standard I2C Functions
void change_i2c_speed( ATCAIface iface, uint32_t speed );
ATCA_STATUS i2c_master_write(ATCAIface iface, uint8_t *txdata, uint16_t txlength);
ATCA_STATUS i2c_master_read( ATCAIface iface, uint8_t *rxdata, uint16_t *rxlength);
#endif

#ifdef ATCA_HAL_SWI
ATCA_STATUS hal_swi_init(void *hal, ATCAIfaceCfg *cfg);
ATCA_STATUS hal_swi_post_init(ATCAIface iface);
ATCA_STATUS hal_swi_send(ATCAIface iface, uint8_t *txdata, uint16_t txlength);
ATCA_STATUS hal_swi_receive( ATCAIface iface, uint8_t *rxdata, uint16_t *rxlength);
ATCA_STATUS hal_swi_wake(ATCAIface iface);
ATCA_STATUS hal_swi_idle(ATCAIface iface);
ATCA_STATUS hal_swi_sleep(ATCAIface iface);
ATCA_STATUS hal_swi_release(void *hal_data );
ATCA_STATUS hal_swi_discover_buses(uint16_t swi_buses[], uint16_t max_buses);
ATCA_STATUS hal_swi_discover_devices(uint16_t busNum, ATCAIfaceCfg *cfg, uint16_t *found);
#endif

#ifdef ATCA_HAL_UART
ATCA_STATUS hal_uart_init(void *hal, ATCAIfaceCfg *cfg);
ATCA_STATUS hal_uart_post_init(ATCAIface iface);
ATCA_STATUS hal_uart_send(ATCAIface iface, uint8_t *txdata, uint16_t txlength);
ATCA_STATUS hal_uart_receive( ATCAIface iface, uint8_t *rxdata, uint16_t *rxlength);
ATCA_STATUS hal_uart_wake(ATCAIface iface);
ATCA_STATUS hal_uart_idle(ATCAIface iface);
ATCA_STATUS hal_uart_sleep(ATCAIface iface);
ATCA_STATUS hal_uart_release(ATCAIface iface);
ATCA_STATUS hal_uart_discover_buses(uint16_t uart_buses[], uint16_t max_buses);
ATCA_STATUS hal_uart_discover_devices(uint16_t busNum, ATCAIfaceCfg *cfg, uint16_t *found);
#endif

#ifdef ATCA_HAL_KIT_CDC
ATCA_STATUS hal_kit_cdc_init(void *hal, ATCAIfaceCfg *cfg);
ATCA_STATUS hal_kit_cdc_post_init(ATCAIface iface);
ATCA_STATUS hal_kit_cdc_send(ATCAIface iface, uint8_t *txdata, uint16_t txlength);
ATCA_STATUS hal_kit_cdc_receive( ATCAIface iface, uint8_t *rxdata, uint16_t *rxlength);
ATCA_STATUS hal_kit_cdc_wake(ATCAIface iface);
ATCA_STATUS hal_kit_cdc_idle(ATCAIface iface);
ATCA_STATUS hal_kit_cdc_sleep(ATCAIface iface);
ATCA_STATUS hal_kit_cdc_release(void *hal_data);
ATCA_STATUS hal_kit_cdc_discover_buses(uint16_t i2c_buses[], uint16_t max_buses);
ATCA_STATUS hal_kit_cdc_discover_devices(uint16_t busNum, ATCAIfaceCfg *cfg, uint16_t *found);
#endif

#ifdef ATCA_HAL_KIT_HID
ATCA_STATUS hal_kit_hid_init(void *hal, ATCAIfaceCfg *cfg);
ATCA_STATUS hal_kit_hid_post_init(ATCAIface iface);
ATCA_STATUS hal_kit_hid_send(ATCAIface iface, uint8_t *txdata, uint16_t txlength);
ATCA_STATUS hal_kit_hid_receive(ATCAIface iface, uint8_t *rxdata, uint16_t *rxlength);
ATCA_STATUS hal_kit_hid_wake(ATCAIface iface);
ATCA_STATUS hal_kit_hid_idle(ATCAIface iface);
ATCA_STATUS hal_kit_hid_sleep(ATCAIface iface);
ATCA_STATUS hal_kit_hid_release(void *hal_data);
ATCA_STATUS hal_kit_hid_discover_buses(uint16_t i2c_buses[], uint16_t max_buses);
ATCA_STATUS hal_kit_hid_discover_devices(uint16_t busNum, ATCAIfaceCfg *cfg, uint16_t *found);
#endif



/** \brief Timer API implemented at the HAL level */
void atca_delay_us(uint32_t delay);
void atca_delay_10us(uint32_t delay);
void atca_delay_ms(uint32_t delay);


/************************************************************************/
/* Customer HAL Implementation Functions                                */
/************************************************************************/




/**********************************************************************************************//**
 * \fn	ATCA_STATUS hal_random_number(uint8_t* random_number)
 *
 * \brief	HAL random number. Implementation of SW pseudo random number generator (PRG)
 *
 * \param [in,out]	random_number	32 byte random number
 *
 * \return	ATCA_STATUS		Status of operation
 **************************************************************************************************/
ATCA_STATUS hal_random_number(uint8_t* random_number);



#ifdef __cplusplus
}
#endif

/** @} */

#endif /* ATCA_HAL_H_ */
