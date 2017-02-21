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
#include "secureboot.h"
#include "atcacert/atcacert_client.h"
#include "atcacert/atcacert_host_hw.h"
#include "atcacert/atcacert_def.h"
#include "crypto/atca_crypto_sw_sha2.h"
#include "custom/cert_def_1_signer.h"
#include "custom/cert_def_2_device.h"
#include "custom/custom_auth_def.h"
#include "authentication/Authenticate.h"


/*brief: generates all parameters to be saved into flash for secure boot operation
 *param[out] params			- Pointer to secure boot parameters structure
 *param[in]  app_tbs_digest - Pointer to application to be signed digest
 *param[in]  app_size       - Pointer to application image size value
 *return     ATCA_STATUS
*/



ATCA_STATUS secureboot_verify_fw_img(pki_chain_auth_struct* params, uint8_t* AppImage)
{
	ATCA_STATUS ret = ATCA_UNIMPLEMENTED;
	uint8_t signer_pubkey[ATCA_PUB_KEY_SIZE];
	uint8_t device_pubkey[ATCA_PUB_KEY_SIZE];
	uint8_t digest[ATCA_SHA_DIGEST_SIZE];
	bool is_verified = false;
	
	//Check root public keys match
	if ( memcmp( &(params->root_pubkey), &g_signer_1_ca_public_key, ATCA_PUB_KEY_SIZE ) != 0 )
	{
		return ATCA_INVALID_ID;
	}
	
	/*Validate signer certificate against root public key*/
	ret = atcacert_verify_cert_hw(&g_cert_def_1_signer, &params->signer_cert[0], SIGNER_CERT_SIZE, params->root_pubkey);
	if (ret != ATCA_SUCCESS) return ret;
	
	/*Extract signer public key*/
	ret = atcacert_get_subj_public_key(&g_cert_def_1_signer, &params->signer_cert[0], SIGNER_CERT_SIZE, signer_pubkey);
	if (ret != ATCA_SUCCESS) return ret;
	
	/*Validate the device cert against signer key*/
	ret = atcacert_verify_cert_hw(&g_cert_def_2_device, &params->device_cert[0], DEVICE_CERT_SIZE, signer_pubkey);
	if (ret != ATCA_SUCCESS) return ret;
	
	/*Extract device public key.*/
	ret = atcacert_get_subj_public_key(&g_cert_def_2_device, &params->device_cert[0], DEVICE_CERT_SIZE, device_pubkey);
	if (ret != ATCA_SUCCESS) return ret;

	/*Check device public key matches tag_signer_pubkey*/
	if ( memcmp( &device_pubkey[0], &tag_signer_pubkey, ATCA_PUB_KEY_SIZE ) != 0 )
	{
		return ATCA_INVALID_ID;
	}
	
	/*Calculate app digest*/
	ret = atcac_sw_sha2_256(AppImage, params->msg_size, digest);
	if (ret != ATCA_SUCCESS) return ret;
	
	/*Verify digest against signature*/
	ret = atcab_verify_extern(&digest[0], &(params->msg_signature[0]), &tag_signer_pubkey[0], &is_verified);
	if (ret != ATCA_SUCCESS) return ret;

	return is_verified ? ATCA_SUCCESS : ATCACERT_E_VERIFY_FAILED;
}


