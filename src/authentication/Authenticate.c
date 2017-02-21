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
 * \file	Authenticate.c
 *
 * \brief	Authenticate class.
 **************************************************************************************************/
#include "Authenticate.h"
#include "astekcrypto.h"
#include "cryptoauthlib.h"
#include "atcacert/atcacert_client.h"
#include "atcacert/atcacert_host_hw.h"
#include "atcacert/atcacert_host_sw.h"
#include "custom/cert_def_1_signer.h"
#include "custom/cert_def_2_device.h"
#include "custom/custom_auth_def.h"
#include "crypto/atca_crypto_sw_sha2.h"
#include "atca_command.h"
#include "host/atca_host.h"

/** \brief global variables for public keys */
uint8_t root_pub_key[ATCA_PUB_KEY_SIZE];
uint8_t signer_pub_key[ATCA_PUB_KEY_SIZE];
uint8_t device_pub_key[ATCA_PUB_KEY_SIZE];
size_t  key_size = sizeof(root_pub_key);

/** \brief global variables for signer certificate */
uint8_t g_signer_cert[ATCA_MAX_CERT_SIZE];
size_t  g_signer_cert_size = sizeof(g_signer_cert);

/** \brief global storage for device certificate */
uint8_t g_device_cert[ATCA_MAX_CERT_SIZE];
size_t  g_device_cert_size = sizeof(g_device_cert);

/** \brief global storage for the challenge data to sign by the device */
uint8_t g_challenge[RANDOM_NUM_SIZE];
uint8_t g_response[ATCA_PUB_KEY_SIZE];

static ATCA_STATUS rebuild_certs(void)
{
	ATCA_STATUS ret = ATCACERT_E_UNIMPLEMENTED;
	
	//Re-initialize cert size variable
	g_signer_cert_size = sizeof(g_signer_cert);
	g_device_cert_size = sizeof(g_device_cert);
	
	ret = atcacert_read_cert(&g_cert_def_1_signer, root_pub_key, &g_signer_cert[0], &g_signer_cert_size);
	if (ret != ATCA_SUCCESS) return ret;
	
	ret = atcacert_get_subj_public_key(&g_cert_def_1_signer, &g_signer_cert[0], g_signer_cert_size, signer_pub_key);
	if (ret != ATCA_SUCCESS) return ret;
	
	ret = atcacert_read_cert(&g_cert_def_2_device, signer_pub_key, &g_device_cert[0], &g_device_cert_size);
	if (ret != ATCA_SUCCESS) return ret;
	
	ret = atcacert_get_subj_public_key(&g_cert_def_2_device, &g_device_cert[0], g_device_cert_size, device_pub_key);
	if (ret != ATCA_SUCCESS) return ret;
	
	return ret;
}

static ATCA_STATUS cert_chain_verify(void)
{
	ATCA_STATUS ret = ATCACERT_E_UNIMPLEMENTED;
	
	//Validate signer certificate against root public key
	ret = atcacert_verify_cert_hw(&g_cert_def_1_signer, &g_signer_cert[0], g_signer_cert_size, root_pub_key);
	if (ret != ATCA_SUCCESS) return ret;
	
	//Assumes signer key was already read from certificate in rebuilt certificate method
	
	//Validate the device cert against signer key
	ret = atcacert_verify_cert_hw(&g_cert_def_2_device, &g_device_cert[0], g_device_cert_size, signer_pub_key);
	if (ret != ATCA_SUCCESS) return ret;
	
	//return 0 if all succeeded
	return ret; 
}

static ATCA_STATUS cert_chain_verify_sw(void)
{
	ATCA_STATUS ret = ATCACERT_E_UNIMPLEMENTED;
	
	ret = atcacert_verify_cert_sw(&g_cert_def_1_signer, &g_signer_cert[0], g_signer_cert_size, root_pub_key);
	if (ret != ATCA_SUCCESS) return ret;
	
	//Assumes signer key was already read from certificate in rebuilt certificate method
	
	//Validate the device cert against signer key
	ret = atcacert_verify_cert_sw(&g_cert_def_2_device, &g_device_cert[0], g_device_cert_size, signer_pub_key);
	if (ret != ATCA_SUCCESS) return ret;
	
	//return 0 if all succeeded
	return ret;
}

static ATCA_STATUS client_generate_response(void)
{
	ATCA_STATUS ret = ATCACERT_E_UNIMPLEMENTED;
	
	ret = atcacert_get_response(g_cert_def_2_device.private_key_slot, g_challenge, g_response);
	if (ret != ATCA_SUCCESS) return ret;
	
	return ret;
}



ATCA_STATUS auth_hw_pki_2(pki_chain_auth_struct* auth_struct, uint8_t* tbs_digest)
{
	ATCA_STATUS status = ATCA_UNIMPLEMENTED;
	uint8_t signer_public_key[ATCA_PUB_KEY_SIZE];
	uint8_t device_public_key[ATCA_PUB_KEY_SIZE];


	//Check if parameters are valid
	if (auth_struct == NULL || tbs_digest == NULL)
	{
		return ATCA_BAD_PARAM;
	}

	//Verify public keys match between FW and authentication structure
	if ( memcmp(&auth_struct->root_pubkey[0], &g_signer_1_ca_public_key[0], ATCA_PUB_KEY_SIZE) != 0)
	{
		return ATCA_BAD_PARAM;		//Return bad parameter if public keys don't match
	}

	//Validate signer certificate against root public key
	status = atcacert_verify_cert_hw(&g_cert_def_1_signer, &auth_struct->signer_cert[0], SIGNER_CERT_SIZE, g_signer_1_ca_public_key);
	if (status != ATCA_SUCCESS) return status;


	/*1st Chain of Trust Level Verification*/
	//Extract signer public key from signer cert
	status = atcacert_get_subj_public_key(&g_cert_def_1_signer, &auth_struct->signer_cert[0], SIGNER_CERT_SIZE, signer_public_key);
	if (status != ATCA_SUCCESS) return status;
	
	//Validate the device cert against signer key
	status = atcacert_verify_cert_hw(&g_cert_def_2_device, &auth_struct->device_cert[0], DEVICE_CERT_SIZE, signer_public_key);
	if (status != ATCA_SUCCESS) return status;


	/*2nd Chain of Trust Level Verification*/
	//Extract device public key from device cert
	status = atcacert_get_subj_public_key(&g_cert_def_2_device, &auth_struct->device_cert[0], DEVICE_CERT_SIZE, device_public_key);
	if (status != ATCA_SUCCESS) return status;


	/*3rd Chain of Trust Level Verification*/
	//verify signature against digest and device public key
	return atcacert_verify_response_hw(device_public_key, tbs_digest, auth_struct->msg_signature);

}

ATCA_STATUS auth_chain_hw(ATCAIfaceCfg *host_cfg, ATCAIfaceCfg *client_cfg)
{
	ATCA_STATUS ret = ATCACERT_E_UNIMPLEMENTED;
	
	//Read root public key from host
	egSelectDevice(host_cfg);
	ret = atcab_read_pubkey(ROOT_PUBKEY_ID, &root_pub_key[0]);
	if (ret != ATCA_SUCCESS) return ret;

	//Verify public keys match between FW and authentication structure
	if ( memcmp(&root_pub_key[0], &g_signer_1_ca_public_key[0], ATCA_PUB_KEY_SIZE) != 0)
	{
		return ATCA_BAD_PARAM;		//Return bad parameter if public keys don't match
	}
	
	//rebuild certificates from client
	egSelectDevice(client_cfg);
	ret = rebuild_certs();
	if (ret != ATCA_SUCCESS) return ret;
	
	//Host verify chain -- Currently using SHA256_sw implementation so no comms with IC
	egSelectDevice(host_cfg);
	ret = cert_chain_verify();
	if (ret != ATCA_SUCCESS) return ret;
	
	//Host generate challenge
	egSelectDevice(host_cfg);  //sanity check to avoid client generating challenge
	ret = atcacert_gen_challenge_hw(g_challenge);
	if (ret != ATCA_SUCCESS) return ret;
	
	//Client generate response
	egSelectDevice(client_cfg);
	ret = client_generate_response();
	if (ret != ATCA_SUCCESS) return ret;
	
	//Host verify response
	egSelectDevice(host_cfg);
	return atcacert_verify_response_hw(device_pub_key, g_challenge, g_response);
}

ATCA_STATUS auth_sw_pki_2(void)
{
	ATCA_STATUS ret = ATCACERT_E_UNIMPLEMENTED;
	//Root pub key is already stored in FW - Cert files
	
	memcpy(root_pub_key, g_signer_1_ca_public_key, key_size);
	
	atcab_wakeup(); //Sanity check
	
	//rebuild certificates from client -- only chip to work with
	ret = rebuild_certs();
	if (ret != ATCA_SUCCESS) return ret;
	
	//Verify certificates using SW functions
	ret = cert_chain_verify_sw();
	if (ret != ATCA_SUCCESS) return ret;
	
	//FW acting as host needs to generate random number
	ret = hal_random_number(&g_challenge[0]);						//HAL user implemented PRG
	if (ret != ATCA_SUCCESS) return ret;
	
	//Client generate response
	ret = client_generate_response();
	if (ret != ATCA_SUCCESS) return ret;
	
	//Host(FW) verify response
	return atcacert_verify_response_sw(device_pub_key, g_challenge, g_response);
}

ATCA_STATUS auth_sw_pki(uint8_t *pub_key)
{
	ATCA_STATUS status = ATCA_UNIMPLEMENTED;
	
	//FW acting as host needs to generate random number
	status = hal_random_number(&g_challenge[0]);				//HAL User implemented PRG
	if (status != ATCA_SUCCESS) return status;
	
	//Target crypto device generate response
	status = client_generate_response();
	if (status != ATCA_SUCCESS) return status;
	
	//Host(FW) verify response
	return atcacert_verify_response_sw(pub_key, g_challenge, g_response);
}

ATCA_STATUS auth_symmetric_sw(void)
{
	ATCA_STATUS status = ATCA_UNIMPLEMENTED;					//Status of operations
	
	uint8_t random_number[RANDOM_NUM_SIZE];		
	uint8_t digest[ATCA_SHA_DIGEST_SIZE], response[ATCA_SHA_DIGEST_SIZE], mac_response_1[ATCA_SHA_DIGEST_SIZE], mac_response_2[ATCA_SHA_DIGEST_SIZE];				
	uint8_t symmetric_key[ATCA_KEY_SIZE];
	uint8_t temporary[ATCA_MSG_SIZE_MAC];						//buffer for temporary operations
	uint8_t *p_temp;											//pointer to temporary buffer

//Generate random number
	status = hal_random_number(&random_number[0]);							//HAL user implemented PRG
	if (status != ATCA_SUCCESS)
	{
		return status;
	}
	
//Request MAC computation from crypto IC
	status = atcab_mac(0x00, SYMMETRIC_KEY_ID, random_number, digest);
	if (status != ATCA_SUCCESS)
	{
		return status;
	}
		
//Compute key
	atcac_sw_sha2_256(&g_symmetric_key[0], g_symmetric_key_size, symmetric_key);
	
//SW MAC computation
	// Start calculation
	p_temp = temporary;											//Get pointer to structure

	// (1) first 32 bytes
	memcpy(p_temp, symmetric_key, ATCA_KEY_SIZE);                // use Key[KeyID]
	p_temp += ATCA_KEY_SIZE;

	// (2) second 32 bytes
	memcpy(p_temp, random_number, ATCA_KEY_SIZE);				// use Key[KeyID]
	p_temp += ATCA_KEY_SIZE;

	// (3) 1 byte opcode
	*p_temp++ = ATCA_MAC;										//Insert Opcode

	// (4) 1 byte mode parameter
	*p_temp++ = 0x00;											//Parameter2
	
	// (5) 2 bytes keyID
	*p_temp++ = LO_BYTE(SYMMETRIC_KEY_ID);						//Big endian format
	*p_temp++ = HI_BYTE(SYMMETRIC_KEY_ID);						

	memset(p_temp, 0, ATCA_OTP_SIZE_8);							// use 8 zeros for 6
	p_temp += ATCA_OTP_SIZE_8;

	memset(p_temp, 0, ATCA_OTP_SIZE_3);							// use 3 zeros for 7
	p_temp += ATCA_OTP_SIZE_3;
	
	// (8) 1 byte SN[8] = 0xEE
	*p_temp++ = ATCA_SN_8;
	
	memset(p_temp, 0, ATCA_SN_SIZE_4);							//use zeros for 9
	p_temp += ATCA_SN_SIZE_4;

	// (10) 2 bytes SN[0:1] = 0x0123
	*p_temp++ = ATCA_SN_0;
	*p_temp++ = ATCA_SN_1;
	
	memset(p_temp, 0, ATCA_SN_SIZE_2);							//use zeros for 9
	
	// Calculate SHA256 to get the MAC digest
	status = atcac_sw_sha2_256(temporary, ATCA_MSG_SIZE_MAC, response);
	if (status != ATCA_SUCCESS)
	{
		return status;
	}

	//Prevent timing attack by hashing both digests before comparing
	atcac_sw_sha2_256(response, ATCA_SHA_DIGEST_SIZE, mac_response_1);
	atcac_sw_sha2_256(digest, ATCA_SHA_DIGEST_SIZE, mac_response_2);
	
	//Verify keys match
	if ( memcmp(&mac_response_1, &mac_response_2, ATCA_SHA_DIGEST_SIZE) == 0)
	{
		return ATCA_SUCCESS;
	}
	
	return ATCA_CHECKMAC_VERIFY_FAILED;
}


ATCA_STATUS gen_auth_2_response(pki_chain_auth_struct* auth_struct, uint8_t* tbs_digest, uint32_t msg_size)
{
	ATCA_STATUS ret = ATCA_UNIMPLEMENTED;
	uint8_t signer_pubkey[ATCA_PUB_KEY_SIZE];
	
	//Re-initialize cert size variable
	g_signer_cert_size = sizeof(g_signer_cert);
	g_device_cert_size = sizeof(g_device_cert);

	//Check if parameters are valid
	if (auth_struct == NULL || tbs_digest == NULL || msg_size == 0)
	{
		return ATCA_BAD_PARAM;
	}
	
	//Retrieve CA root public key from device
	ret = atcab_read_pubkey(ROOT_PUBKEY_ID, &root_pub_key[0]);
	if (ret != ATCA_SUCCESS) return ret;

	//Verify public keys match between FW and authentication structure
	if ( memcmp(&root_pub_key[0], &g_signer_1_ca_public_key[0], ATCA_PUB_KEY_SIZE) != 0)
	{
		return ATCA_BAD_PARAM;		//Return bad parameter if public keys don't match
	}

	//Copy CA root key to response struct
	memcpy(auth_struct->root_pubkey, g_signer_1_ca_public_key, ATCA_PUB_KEY_SIZE);
	
	//Generate signer certificate
	ret = atcacert_read_cert(&g_cert_def_1_signer, auth_struct->root_pubkey, &g_signer_cert[0], &g_signer_cert_size);
	if (ret != ATCA_SUCCESS) return ret;
	
	//Extract signer public key
	ret = atcacert_get_subj_public_key(&g_cert_def_1_signer, &g_signer_cert[0], g_signer_cert_size, signer_pubkey);
	if (ret != ATCA_SUCCESS) return ret;
	
	//Sanity check for signer certificate size
	if (g_signer_cert_size > SIGNER_CERT_SIZE)
	{
		return ATCACERT_E_UNEXPECTED_ELEM_SIZE;
	}else
	{
		//Retrieve signer certificate
		memcpy(auth_struct->signer_cert, g_signer_cert, SIGNER_CERT_SIZE);
	}

	//Generate device certificate
	ret = atcacert_read_cert(&g_cert_def_2_device, signer_pubkey, &g_device_cert[0], &g_device_cert_size);
	if (ret != ATCA_SUCCESS) return ret;
	
	//Sanity check for device certificate size
	if (g_device_cert_size > DEVICE_CERT_SIZE)
	{
		return ATCACERT_E_UNEXPECTED_ELEM_SIZE;
	}else
	{
		//Retrieve device certificate
		memcpy(auth_struct->device_cert, g_device_cert, DEVICE_CERT_SIZE);
	}
	
	//Assign application image size
	auth_struct->msg_size = msg_size;
	
	//sign application image digest
	ret = atcacert_get_response(g_cert_def_2_device.private_key_slot, tbs_digest, auth_struct->msg_signature);
	if (ret != ATCA_SUCCESS) return ret;
	
	
	return ret;
}

