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
#include "ecdsa.h"
#include "custom/custom_auth_def.h"
#include "crypto/atca_crypto_sw_sha2.h"
#include "crypto/atca_crypto_sw_ecdsa.h"
#include "atcacert/atcacert_host_hw.h"

ATCA_STATUS ecdsa_custom_sign(uint8_t* msg, size_t length, uint8_t* signature)
{
	ATCA_STATUS ret = ATCA_UNIMPLEMENTED;
	
	uint8_t tbs_digest[ATCA_SHA_DIGEST_SIZE];									//to be signed buffer digest variable
	
	if (msg == NULL || length == 0 || signature == NULL)
	{
		return ATCA_BAD_PARAM;
	}

	//SW SHA256 of message to generate tbs_digest
	ret = atcac_sw_sha2_256(msg, length, tbs_digest);	//generate digest of buffer
	if (ret != ATCA_SUCCESS) return ret;
	
	//Check parameters before signing data
	if (TAG_SIGNER_PRIVKEY_ID > ATCA_KEY_ID_MAX || &tbs_digest[0] == NULL || signature == NULL)
	{
		return ATCACERT_E_BAD_PARAMS;						//Verify private key is valid
	}		
													//sign digest using specified private key
	return  atcab_sign(TAG_SIGNER_PRIVKEY_ID, &tbs_digest[0], signature);

}


ATCA_STATUS ecdsa_custom_verify(uint8_t* msg, size_t length, uint8_t* signature)
{
	ATCA_STATUS ret = ATCA_UNIMPLEMENTED;
	
	auto uint8_t tbs_digest[ATCA_SHA_DIGEST_SIZE];							//to be signed buffer digest variable
	
	if (msg == NULL || length == 0 || signature == NULL)
	{
		return ATCA_BAD_PARAM;
	}

	//SW SHA256 of message to generate tbs_digest
	ret = atcac_sw_sha2_256(msg, length, tbs_digest);	//generate digest of buffer
	if (ret != ATCA_SUCCESS) return ret;
	
	return  atcacert_verify_response_hw(tag_signer_pubkey, tbs_digest, signature);
}

ATCA_STATUS ecdsa_custom_verify_sw(uint8_t* msg, size_t length, uint8_t* signature)
{
	ATCA_STATUS ret = ATCA_UNIMPLEMENTED;
	
	auto uint8_t tbs_digest[ATCA_SHA_DIGEST_SIZE];			//to be signed message digest variable
	
	if (msg == NULL || length == 0 || signature == NULL)
	{
		return ATCA_BAD_PARAM;
	}

	//SW SHA256 of message to generate tbs_digest
	ret = atcac_sw_sha2_256(msg, length, tbs_digest);	//generate digest of buffer
	if (ret != ATCA_SUCCESS) return ret;
	
	return atcac_sw_ecdsa_verify_p256(tbs_digest, signature, tag_signer_pubkey);
}