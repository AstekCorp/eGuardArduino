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
#ifndef ECDSA_H_
#define ECDSA_H_

#include "cryptoauthlib.h"

/**********************************************************************************************//**
 * \fn	ATCA_STATUS ecdsa_custom_sign(uint8_t* buffer, size_t length, uint8_t* signature);
 *
 * \brief	Generates signature of input buffer using programmer private key (defined in custom
 * 			files). Uses elliptic curve digital signature algorithm to generate signature. \n
 * 			This function allows OEM products to be signed (certified). By verifying the
 * 			signatures against its UID and any data used to generate the signature the system is
 * 			able to only accept OEM products. \n The "To Be Signed" (TBS) buffer should contain
 * 			non-volatile information, such as a silicon UID, that can't be changed.
 *
 * \param [in] 	buffer   	Pointer to message to be signed.
 * \param [in] 	length   	Length of input message.
 * \param [out]	signature	Pointer to 64 byte signature.
 *
 * \return	ATCA_STATUS		Return ATCA_SUCCESS if signature if created successfully.
 **************************************************************************************************/
ATCA_STATUS ecdsa_custom_sign(uint8_t* msg, size_t length, uint8_t* signature);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS ecdsa_custom_verify(uint8_t* buffer, size_t length, uint8_t* signature);
 *
 * \brief	Verify signature generated by ecdsa_custom_sign function. Uses signer public key to
 * 			verify it was signed with signer private key. This function allows system to verify a
 * 			signed product or buffer and prevent unwanted/certified products from being accepted
 * 			during runtime.
 *
 * \param [in]	buffer   	Pointer to message to be signed.
 * \param [in]	length   	Length of input message.
 * \param [in]	signature	Pointer to 64 byte signature.
 *
 * \return	ATCA_STATUS		Return ATCA_SUCCESS on successful authentication. Otherwise error code.
 **************************************************************************************************/
ATCA_STATUS ecdsa_custom_verify(uint8_t* msg, size_t length, uint8_t* signature);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS ecdsa_custom_verify_sw(uint8_t* buffer, size_t length, uint8_t* signature);
 *
 * \brief	Verify signature in SW generated by ecdsa_custom_sign using tag signer private key.
 *
 * \param [in]	buffer   	Pointer to to be signed message
 * \param 	  	length   	Length of input message
 * \param [in]	signature	Pointer to 64 byte signature
 *
 * \return	An ATCA_STATUS.
 **************************************************************************************************/
ATCA_STATUS ecdsa_custom_verify_sw(uint8_t* msg, size_t length, uint8_t* signature);


#endif /* ECDSA_H_ */