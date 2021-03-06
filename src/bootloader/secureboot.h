/**
 * \file
 * \brief
 *
 * Copyright (c) 2016 Astek Corporation. All rights reserved.
 *
 * \astek_eguard_library_license_start
 *
 * \page eGuard_License
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
 * \astek_eguard_library_license_stop
 */
#ifndef SECUREBOOT_H_
#define SECUREBOOT_H_

#define SIGNER_CERT_SIZE 506
#define DEVICE_CERT_SIZE 428

#include "cryptoauthlib.h"
#include "authentication/Authenticate.h"

/*brief: Checks application to be run before jumping to application section. 
 *This should be called in booloader section before authorizing application to run
 *param[in] params			- Pointer to secure boot parameters structure, generate from generate boot method using astek usb signer
 *param[in] AppImage		- Pointer to application image
 *return     ATCA_STATUS
*/

/**********************************************************************************************//**
 * \fn	ATCA_STATUS secureboot_verify_fw_img(pki_chain_auth_struct* params, uint8_t* AppImage);
 *
 * \brief	Performs check of fw image against Fw image certificate(pki_chain_auth_struct).\n
 * 			This should be called buring bootloader before jumping into application code
 *
 * \param [in,out]	params  	Pointer to Certificate (authentication structure)
 * \param [in,out]	AppImage	Pointer to beginning of application image
 *
 * \return	 ATCA_SUCCESS otherwise error code
 **************************************************************************************************/
ATCA_STATUS secureboot_verify_fw_img(pki_chain_auth_struct* params, uint8_t* AppImage);

#endif /* SECUREBOOT_H_ */