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
 *
 * \brief  Atmel Crypto Auth status codes
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

#ifndef _ATCA_STATUS_H
#define _ATCA_STATUS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* all status codes for the ATCA lib are defined here */

typedef enum {
	ATCA_SUCCESS					= 0x00, //!< Function succeeded.
	ATCA_CONFIG_ZONE_LOCKED			= 0x01,
	ATCA_DATA_ZONE_LOCKED			= 0x02,
	ATCACERT_E_ERROR				= 0xA1,  //!< General error.
	ATCACERT_E_BAD_PARAMS			= 0xA2,  //!< Invalid/bad parameter passed to function.
	ATCACERT_E_BUFFER_TOO_SMALL		= 0xA3,  //!< Supplied buffer for output is too small to hold the result.
	ATCACERT_E_DECODING_ERROR		= 0xA4,  //!< Data being decoded/parsed has an invalid format.
	ATCACERT_E_INVALID_DATE			= 0xA5,  //!< Date is invalid.
	ATCACERT_E_UNIMPLEMENTED		= 0xA6,  //!< Function is unimplemented for the current configuration.
	ATCACERT_E_UNEXPECTED_ELEM_SIZE	= 0xA7,  //!< A certificate element size was not what was expected.
	ATCACERT_E_ELEM_MISSING			= 0xA8,  //!< The certificate element isn't defined for the certificate definition.
	ATCACERT_E_ELEM_OUT_OF_BOUNDS   = 0xA9,  //!< Certificate element is out of bounds for the given certificate.
	ATCACERT_E_BAD_CERT				= 0xAA,  //!< Certificate structure is bad in some way.
	ATCACERT_E_WRONG_CERT_DEF		= 0xAB, 
	ATCACERT_E_VERIFY_FAILED		= 0xAC,  //!< Certificate or challenge/response verification failed.
	ATCA_WAKE_FAILED				= 0xD0, //!< response status byte indicates CheckMac failure (status byte = 0x01)
	ATCA_CHECKMAC_VERIFY_FAILED		= 0xD1, //!< response status byte indicates CheckMac failure (status byte = 0x01)
	ATCA_PARSE_ERROR				= 0xD2, //!< response status byte indicates parsing error (status byte = 0x03)
	ATCA_STATUS_CRC					= 0xD4, //!< response status byte indicates CRC error (status byte = 0xFF)
	ATCA_STATUS_UNKNOWN				= 0xD5, //!< response status byte is unknown
	ATCA_STATUS_ECC					= 0xD6, //!< response status byte is ECC fault (status byte = 0x05)
	ATCA_FUNC_FAIL					= 0xE0, //!< Function could not execute due to incorrect condition / state.
	ATCA_GEN_FAIL					= 0xE1, //!< unspecified error
	ATCA_BAD_PARAM					= 0xE2, //!< bad argument (out of range, null pointer, etc.)
	ATCA_INVALID_ID					= 0xE3, //!< invalid device id, id not set
	ATCA_INVALID_SIZE				= 0xE4, //!< Count value is out of range or greater than buffer size.
	ATCA_BAD_CRC					= 0xE5, //!< incorrect CRC received
	ATCA_RX_FAIL					= 0xE6, //!< Timed out while waiting for response. Number of bytes received is > 0.
	ATCA_RX_NO_RESPONSE				= 0xE7, //!< Not an error while the Command layer is polling for a command response.
	ATCA_RESYNC_WITH_WAKEUP			= 0xE8, //!< Re-synchronization succeeded, but only after generating a Wake-up
	ATCA_PARITY_ERROR				= 0xE9, //!< for protocols needing parity
	ATCA_TX_TIMEOUT					= 0xEA, //!< for Atmel PHY protocol, timeout on transmission waiting for master
	ATCA_RX_TIMEOUT					= 0xEB, //!< for Atmel PHY protocol, timeout on receipt waiting for master
	ATCA_COMM_FAIL					= 0xF0, //!< Communication with device failed. Same as in hardware dependent modules.
	ATCA_TIMEOUT					= 0xF1, //!< Timed out while waiting for response. Number of bytes received is 0.
	ATCA_BAD_OPCODE					= 0xF2, //!< opcode is not supported by the device
	ATCA_WAKE_SUCCESS				= 0xF3, //!< received proper wake token
	ATCA_EXECUTION_ERROR			= 0xF4, //!< chip was in a state where it could not execute the command, response status byte indicates command execution error (status byte = 0x0F)
	ATCA_UNIMPLEMENTED				= 0xF5, //!< Function or some element of it hasn't been implemented yet
	ATCA_ASSERT_FAILURE				= 0xF6, //!< Code failed run-time consistency check
	ATCA_TX_FAIL					= 0xF7, //!< Failed to write
	ATCA_NOT_LOCKED					= 0xF8, //!< required zone was not locked
	ATCA_NO_DEVICES					= 0xF9, //!< For protocols that support device discovery (kit protocol), no devices were found
	ATCA_VERIFY_FAIL				= 0xFA, //!< ECDSA Verify Failed
} ATCA_STATUS;

#ifdef __cplusplus
}
#endif
#endif
