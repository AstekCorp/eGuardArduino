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
#ifndef ASTEKCRYPTO_H_
#define ASTEKCRYPTO_H_

#include "cryptoauthlib.h"
#include "atca_iface.h"
#include "authentication/Authenticate.h"
#include "ecdsa/ecdsa.h"
#include "hal/custom_hal.h"


/** \defgroup astekcrypto_ Astek Cryptography Library Methods (astekcrypto_)
 *
 * \brief
 * Astek Crypto Library API Calls for
 * CryptoAuth chips
 *
   @{ */

#ifdef __cplusplus
extern "C" {
#endif




/**********************************************************************************************//**
 * \struct	OTPConfig
 *
 * \brief	The OTP Zone contents.
 **************************************************************************************************/
typedef struct {
	//Format/Version of OTP zone
	uint8_t OTPFormat;
	//Configuration Type
	uint8_t ConfigType[OTPCONFIG_TYPE_SIZE];
	//Major Revision
	uint8_t RevMaj;
	//Minor Revision
	uint8_t RevMin;
	//For future use
	uint8_t Reserved[OTPCONFIG_RESERVED1_SIZE];
	
	//ASCII fields
	//Astek in ASCII
	uint8_t Astek[OTPCONFIG_ASTEK_SIZE];
	//Provisioning year
	uint8_t ProvYear[OTPCONFIG_DATE_SIZE];
	//Provisioning month
	uint8_t ProvMonth[OTPCONFIG_DATE_SIZE];
	//Provisioning day
	uint8_t ProvDay[OTPCONFIG_DATE_SIZE];
	//For future use
	uint8_t Reserved2[OTPCONFIG_RESERVED2_SIZE];
	//32 byte description field
	uint8_t Description[OTPCONFIG_DESCRIPTION_SIZE];
}OTPConfig;


#include "cryptoauthlib.h"

/**********************************************************************************************//**
 * \enum	AuthenticationType
 * \brief	Enumeration representing different types of authentication.
 **************************************************************************************************/
typedef enum
{
	SYMMETRIC, ///<An enum constant representing the symmetric authentication option. Symmetric key config stored in astekcert_def.h

	SW_PKI, ///< An enum constant representing the Software Public Key Authentication. \n Only verifies Signature against Public Key

	SW_PKI_CHAIN, ///< An enum constant representing the Software PUblic Key Authentication of chain of trust. \n  

	HW_PKI_CHAIN///< An enum constant representing the Hardware Public Key Authentication of chain of trust. \n 
}AuthenticationType;

/**********************************************************************************************//**
 * \struct	auth3_in_params
 *
 * \brief	An authentication structure used as input parameter for egAuthenticate(HW_PKI_CHAIN).
 **************************************************************************************************/
typedef struct auth_chain_inparams
{
	/** \brief	The authentication structure of the chain of trust. */
	pki_chain_auth_struct auth_struct;

	/** \brief	The digest[32] used to generate message signature. */
	uint8_t	digest[ATCA_SHA_DIGEST_SIZE];
}auth_chain_inparams;

/**********************************************************************************************//**
 * \fn	char* egGetRev(void);
 *
 * \brief	eGuard Get Revision \n Returns pointer to string containing SW revision.
 *
 * \return	Pointer to version string.
 **************************************************************************************************/
const uint8_t * egGetRev(void);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS egGetInfo(OTPConfig *otpconfig);
 *
 * \brief	eGuard Get Information \n Retrieves configuration information from One Time
 * 			Programmable (OTP) zone.
 *
 * \param [in,out]	otpconfig	to 64 byte buffer.
 *
 * \return	Status of operation. Returns ATCA_SUCCESS if device is detected successfully, error
 * 			code otherwise.
 * 			
 * 			\short	\b Example \n This is example code for utilizing the ATCA_STATUS egGetInfo
 * 			function.
 * 			
 * 			\include Example-egGetConfig.c.
 * 			
 * 			\example Example-egGetConfig.c.
 **************************************************************************************************/
ATCA_STATUS egGetInfo(OTPConfig *otpconfig);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS egSelectDevice(ATCAIfaceCfg *cfg);
 *
 * \brief	eGuard Select Device \n Selects target device for crypto operations. This function
 * 			only needs to be called when initializing a device or when changing between different
 * 			target devices.
 *
 * \param [in,out]	cfg	to device configuration structure. Example structures can be found in
 * 						custom_hal.c.
 *
 * \return	Status of operation. Returns ATCA_SUCCESS if device is detected successfully, error
 * 			code otherwise
 * 			
 * 			\short	\b Example \n This is example code for utilizing the ATCA_STATUS
 * 			egSelectDevice function.
 * 			
 * 			\include Example-egSelectDevice.c.
 * 			
 * 			\example Example-egSelectDevice.c.
 **************************************************************************************************/
ATCA_STATUS egSelectDevice(ATCAIfaceCfg *cfg);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS egDetectDevice(void);
 *
 * \brief	eGuard Detect Device \n Determines if device is properly connected and reachable.
 *
 * \return	Status of operation. Returns ATCA_SUCCESS if device is detected successfully, error
 * 			code otherwise
 * 			
 * 			\short	\b Example \n This is example code for utilizing the ATCA_STATUS
 * 			egDetectDevice function.
 * 			
 * 			\include Example-egDetectDevice.c.
 * 			
 * 			\example Example-egDetectDevice.c.
 **************************************************************************************************/
ATCA_STATUS egDetectDevice(void);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS egSerNum(uint8_t *sernum);
 *
 * \brief	eGuard Serial Number \n Returns crypto IC serial number in array pointed to by
 * 			*sernum.
 *
 * \param [in,out]	sernum	to 9 bytes serial number array.
 *
 * \return	Status of operation. Returns ATCA_SUCCESS if device is detected successfully, error
 * 			code otherwise
 * 			
 * 			\short	\b Example \n This is example code for utilizing the ATCA_STATUS egSerNum
 * 			function.
 * 			
 * 			\include Example-egSerNum.c.
 * 			
 * 			\example Example-egSerNum.c.
 **************************************************************************************************/
ATCA_STATUS egSerNum(uint8_t *sernum);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS egGenRandom(uint8_t *randnum);
 *
 * \brief	eGuard Generate Random Number \n Generate a 32 byte random number from crypto IC.
 *
 * \param [in,out]	randnum	to 32 byte buffer for random number.
 *
 * \return	Status of operation. Returns ATCA_SUCCESS if device is detected successfully, error
 * 			code otherwise
 * 			
 * 			\short	\b Example \n This is example code for utilizing the ATCA_STATUS egGenRandom
 * 			function.
 * 			
 * 			\include Example-egGenRandom.c.
 * 			
 * 			\example Example-egGenRandom.c.
 **************************************************************************************************/
ATCA_STATUS egGenRandom(uint8_t *randnum);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS egSHA256(uint8_t* message, size_t length, uint8_t* digest);
 *
 * \brief	eGuard SHA256 \n
 * 			Returns HW digest using SHA256 hashing function.
 *
 * \param [in] 	message	The message to be hashed.
 * \param 	   	length 	The length of the message.
 * \param [out]	digest 	The digest of the message.
 *
 * \return	Status of operation. Returns ATCA_SUCCESS if device is detected successfully, error code
 * 			otherwise
 **************************************************************************************************/
ATCA_STATUS egSHA256(uint8_t* message, size_t length, uint8_t* digest);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS egDevicePubKey(uint8_t *pubkey);
 *
 * \brief	eGuard Get Device Publice Key \n Returns public key corresponding to private key slot
 * 			designated in device configuration file.
 *
 * \param [in,out]	pubkey	Pointer to 64 byte public key buffer.
 *
 * \return	Status of operation. Returns ATCA_SUCCESS if device is detected successfully, error
 * 			code otherwise
 * 			
 * 			\short	\b Example \n This is example code for utilizing the ATCA_STATUS
 * 			egDevicePubKey function.
 * 			
 * 			\include Example-egDevicePubKey.c.
 * 			
 * 			\example Example-egDevicePubKey.c.
 **************************************************************************************************/
ATCA_STATUS egDevicePubKey(uint8_t *pubkey);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS egAuthenticate(AuthenticationType auth_type, uint8_t *param);
 *
 * \brief	eGuard Authenticate \n Performs different authentication schemes based on input type.
 *
 * \param 		  	auth_type	The type of authentication. One of the values AuthenticationType
 * 								enumeration.\n\n
 * 								
 * 								<b>SYMMETRIC:</b>	Performs symmetric authentication between \n
 * 											key in FW and key in HW\n\n
 * 								
 * 								<b>SW_PKI:</b>	Performs elliptic curve
 * 											authentication only verifying if device private key is pair to
 * 											the input public key param argument should point to 64 byte
 * 											public key when this mode is used\n\n
 * 								
 * 								<b>SW_PKI_CHAIN:</b>	Performs elliptic curve authentication
 * 								verifying the
 * 											2 levels of chain of trust (signer and device). This
 * 											authentication will fail if device is not properly provisioned
 * 											or not part of the chain of trust specified by root key in
 * 											FW.\n\n
 * 								
 * 								<b>HW_PKI_CHAIN:</b>	Performs hw elliptic curve authentication
 * 								verifying
 * 											entire chain of trust using response generated from
 * 											egDevAuthResponse.\n.
 * \param [in,out]	param	 	Auxiliary parameter used in different ways by function depending
 * 								on input type called.\n\n
 * 								<b>SYMMETRIC:</b>	parameter should be NULL\n\n
 * 								<b>SW_PKI:</b>	parameter should point to 64 byte device public
 * 								key\n\n
 * 								<b>SW_PKI_CHAIN:</b>	parameter should be NULL \n\n
 * 								<b>HW_PKI_CHAIN:</b>	parameter should be auth_chain_inparams.
 *
 * \return	Status of operation. Returns ATCA_SUCESS if device is detected successfully, error
 * 			code otherwise
 * 			
 * 			\short	\b Example \n  This is example code for utilizing the ATCA_STATUS
 * 			egAuthenticate function.
 * 			
 * 			\include Example-egAuthenticate.c.
 * 			
 * 			\example Example-egAuthenticate.c.
 **************************************************************************************************/
ATCA_STATUS egAuthenticate(AuthenticationType auth_type, void *param);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS egAuthResponse(pki_chain_auth_struct* auth_struct, uint8_t* tbs_digest, uint32_t msg_size);
 *
 * \brief	eGuard Authentication 2 level of chain of trust Response \n Generates a
 * 			certificate with signatures from 2 levels of chain of trust and tbs_digest signature.
 *
 * \param [in,out]	auth_struct	Pointer to device authentication structure.
 * \param [in,out]	tbs_digest 	to be signed message digest.
 * \param 		  	msg_size   	Size of the buffer or message used to generate tbs_digest. If
 * 								tbs_digest is random number then msg_size is 32.
 *
 * \return	Status of operation, return ATCA_SUCCESS if success otherwise error code
 * 			
 * 			\short	\b Example \n This is example code for utilizing the ATCA_STATUS
 * 			egDevAuthResponse function.
 * 			
 * 			\include Example-egDevAuthResponse.c.
 * 			
 * 			\example Example-egDevAuthResponse.c.
 **************************************************************************************************/
ATCA_STATUS egAuthResponse(pki_chain_auth_struct* auth_struct, uint8_t* tbs_digest, uint32_t msg_size);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS egSignTag(uint8_t* buffer, size_t length, uint8_t* signature);
 *
 * \brief	eGuard Sign Tag \n
 * 			Returns signature of input buffer. Uses elliptic curve digital signature algorithm to
 * 				create a unique signature of the buffer.
 * 			Function can be used to create a "certificate" for OEM products to be later verified
 * 				and prevent "knock-off" products from gaining access to the system
 * 			The "to-be-signed" (TBS) buffer should contain unique non-volatile information of the
 * 				product being signed, such as a UID, to prevent counterfeits from cloning the
 * 				signature
 * 			- Consult ASTEK for more details and correct implementation.
 *
 * \param [in] 	buffer   	Pointer to buffer to be signed.
 * \param [in] 	length   	Length of input buffer.
 * \param [out]	signature	Pointer to 64 byte signature.
 *
 * \return	Status of operation, return ATCA_SUCCESS if success otherwise error code
 * 			
 * 			// TODO example of usage.
 **************************************************************************************************/
ATCA_STATUS egSignTag(uint8_t* buffer, size_t length, uint8_t* signature);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS egVerifyTag(uint8_t* buffer, size_t length, uint8_t* signature);
 *
 * \brief	eGuard Verify Tag \n
 * 			Verify product signature generated by ecdsa_sign function. This function allows
 * 			system to verify a signed product or buffer and prevent unwanted products from being
 * 			accepted during runtime.
 *
 * \param [in]	buffer   	Pointer to buffer to be signed.
 * \param [in]	length   	Length of input buffer.
 * \param [in]	signature	Pointer to 64 byte signature.
 *
 * \return	Status of operation, return ATCA_SUCCESS if success otherwise error code
 * 			
 * 			// TODO example of usage.
 **************************************************************************************************/
ATCA_STATUS egVerifyTag(uint8_t* buffer, size_t length, uint8_t* signature);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS egSecureBoot(pki_chain_auth_struct* auth_struct, uint8_t* app_image);
 *
 * \brief	Performs check of fw image against Fw image certificate.\n
 * 			Certificate should be saved in flash and generated for each fw image before hand using
 * 			Astek Crypto Suite (ACS) and a signing usb dongle.\n
 * 			This function should be called buring bootloader before jumping into application code
 *
 * \param [in,out]	auth_struct	Pointer to certificate in flash memory
 * \param [in,out]	app_image  	Pointer to begging of application image
 *
 * \return	An ATCA_STATUS.
 **************************************************************************************************/
ATCA_STATUS egSecureBoot(pki_chain_auth_struct* fw_cert, uint8_t* app_image);


#ifdef __cplusplus
}
#endif

#endif /* ASTEKCRYPTO_H_ */
