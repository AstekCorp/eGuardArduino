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
/**********************************************************************************************//**
 * \file	Authenticate.h
 *
 * \brief	Declares the authenticate class.
 **************************************************************************************************/
#ifndef AUTHENTICATE_H_
#define AUTHENTICATE_H_

#include "cryptoauthlib.h"


#define SIGNER_CERT_SIZE 506
#define DEVICE_CERT_SIZE 428

/**********************************************************************************************//**
 * \struct	challenge_params
 * 
 * \brief	Structure to hold input parameters for challenge in authentication \n\n
 * 
 * 			Structure can be used in one of two schemes: \n
 *  				32 Byte random challenge sent directly (If challenge is used, msg_size is 32) \n 32
 *  				digested sent as challenge generated from SHA256 of original message \n.
 **************************************************************************************************/
typedef struct challenge_params {
	/** The to be signed digest or random challenge. */
	uint8_t tbs_digest[32];
	/** Size of the original message that generated the digest(challenge). */
	/*0 if challenge instead of message digest*/
	uint32_t msg_size;
} challenge_params;

/**********************************************************************************************//**
 * \struct	pki_3_auth_struct
 * 
 * \brief	A device authentication structure. Structure contains 3 levels of chain of trust for
 * 			authentication. \n\n
 * 
 * 			1 level of chain of trust) Signer Certificate: Verifies Signer PK against Root PK  \n 2
 * 			level of chain of trust) Device Certificate: Verifies Device PK against Signer PK  \n.
 **************************************************************************************************/
typedef struct pki_chain_auth_struct {
	uint8_t msg_signature[64];
	uint32_t msg_size;
	uint8_t root_pubkey[64];
	uint8_t signer_cert[SIGNER_CERT_SIZE];
	uint8_t device_cert[DEVICE_CERT_SIZE];
} pki_chain_auth_struct;

/**********************************************************************************************//**
 * \fn	ATCA_STATUS auth_hw_pki_2(pki_chain_auth_struct* auth_struct, uint8_t* tbs_digest);
 *
 * \brief	Authenticates 2 levels of chain of trust from generated auth_device_response input.
 *
 * \param [in,out]	auth_struct	Device authentication structure. Contains signature, device cert,
 * 								and signer cert.
 * \param [in,out]	tbs_digest 	To be signed digest generated from original message. Challenge
 * 								itself in case of challenge authentication.
 *
 * \return	ATCA_STATUS.		Returns success if authentication passes, error code otherwise.
 **************************************************************************************************/
ATCA_STATUS auth_hw_pki_2(pki_chain_auth_struct* auth_struct, uint8_t* tbs_digest);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS auth_chain_hw(ATCAIfaceCfg *host_cfg, ATCAIfaceCfg *client_cfg);
 *
 * \brief	Hardware Authentication of full Chain of Trust between 2 crypto chips on same bus.
 * 			Utilizes host crypto chip to authenticate client. One master MCU talks to both Host
 * 			and Client crypto chips. Whole chain of trust is authenticated, from device up to
 * 			root.
 *
 * \param [in]	host_cfg  	Pointer to the host crypto chip configuration structure.
 * \param [in]	client_cfg	Pointer to the client crypto chip configuration structure.
 *
 * \return	ATCA_STATUS Returns success on successful authentication, otherwise error code
 **************************************************************************************************/
ATCA_STATUS auth_chain_hw(ATCAIfaceCfg *host_cfg, ATCAIfaceCfg *client_cfg);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS auth_sw_pki_3(void);
 *
 * \brief	Software Authentication of Chain of Trust to make sure device was signed by root. Function may be called to
 * 			authenticate the crypto chip connected to the system in order to verify its
 * 			authenticity before relaying on the chip for all cryptography operations.
 *
 * \return	An ATCA_STATUS	Return ATCA_SUCCESS on successful authentication.
 **************************************************************************************************/
ATCA_STATUS auth_sw_pki_2(void);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS auth_dev_sw(uint8_t *pub_key);
 *
 * \brief	Device software authentication. PKI authentication of crypto device connected to the
 * 			system implemented in SW. Function may be called to authenticate crypto chip
 * 			connected to the system. Authentication will only succeed if device's private key is
 * 			pair to input public key.
 *
 * \param [in]	pub_key	Pointer to the device public key.
 *
 * \return	ATCA_STATUS	Return ATCA_SUCCESS on successful authentication.
 **************************************************************************************************/
ATCA_STATUS auth_sw_pki(uint8_t *pub_key);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS auth_symmetric_sw(void);
 *
 * \brief	Symmetric software authentication. Uses symmetric key from symmetric_key_ID to
 * 			authenticate crypto device connected to system. Symmetric key must be valid in
 * 			symmetric key slot otherwise returns error. MAC computation is executed in order to
 * 			verify key.
 *
 * \return	ATCA_STATUS		Return ATCA_SUCCESS on successful authentication.
 **************************************************************************************************/
ATCA_STATUS auth_symmetric_sw(void);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS gen_device_auth_response(pki_3_auth_struct* params, uint8_t* app_tbs_digest, uint32_t app_size);
 *
 * \brief	Generates a device authentication response (2 levels of chain of trust). Message signature is generated from signing tbs_digest
 *
 * \param [in,out]	auth_struct	Pointer to device authentication structure.
 * \param [in,out]	tbs_digest 	to be signer message digest.
 * \param 		  	msg_size   	Size of the buffer or message used to generate tbs_digest. If
 * 								tbs_digest is random number then msg_size is 32.
 *
 * \return	Status of operation, return 0 if success otherwise error code
 **************************************************************************************************/
ATCA_STATUS gen_auth_2_response(pki_chain_auth_struct* auth_struct, uint8_t* tbs_digest, uint32_t msg_size);


#endif /* AUTHENTICATE_H_ */
