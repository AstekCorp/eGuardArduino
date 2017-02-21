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

#include "atca_crypto_sw_rand.h"
#include "basic/atca_basic.h"
#include "custom/cert_def_1_signer.h"
#include "atca_crypto_sw_sha2.h"
#include "hal/atca_hal.h"
#include <stdlib.h>


uint8_t seed[32];

static void onetimepad(uint8_t* msg, uint8_t* key, int length)
{
	int i;
	for (i=0; i<length; i++)
	*(msg + i) ^= *(key + i);
}


/** \brief return software generated random number
 * \param[out] data       ptr to space to receive the random number
 * \param[in]  data_size  size of data buffer
 * return ATCA_STATUS
 */
int atcac_sw_random(uint8_t* data, size_t data_size)
{
	ATCA_STATUS ret;
	uint8_t nonce[32], randnum[32];
	
	//Request random number from IC
	atcab_random(nonce);
	
	hal_random_number(&randnum[0]);
	onetimepad(nonce,randnum, 32);
	
	if ((seed[0] == 0) && (seed[10] == 0))
	{
		memcpy(seed, g_signer_1_ca_public_key, 32);				//if seed hasn't been initialized use root public key
	}
	
	onetimepad(nonce,seed,32);									//one time pad to create cipher seed with nonce
	
	ret = atcac_sw_sha2_256(nonce, sizeof(nonce), seed);		//create digest of seed and store in nonce variable
	if (ret != ATCA_SUCCESS)
	return ret;
	
	ret = atcac_sw_sha2_256(seed, sizeof(seed), nonce);			//create digest of seed and store in nonce variable
	if (ret != ATCA_SUCCESS)
	return ret;
	
	memcpy(data, nonce, data_size);
	
	return ATCA_SUCCESS;
}

