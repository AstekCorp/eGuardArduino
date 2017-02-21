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
#include <stdint.h>
#include "astekcrypto.h"
#include "cryptoauthlib.h"
#include "custom/cert_def_2_device.h"
#include "custom/custom_auth_def.h"
#include "crypto/atca_crypto_sw_sha2.h"
#include "atcacert/atcacert_host_hw.h"
#include "authentication/Authenticate.h"
#include "ecdsa/ecdsa.h"
#include "bootloader/secureboot.h"

// TODO Update version before release
const uint8_t astek_version[] = { "1.1.282.0" };  // Maj.Min.svnMaj.svnMin

const uint8_t* egGetRev(void)
{
	return &astek_version[0];
}

ATCA_STATUS egGetInfo(OTPConfig *otpconfig)
{
	return atcab_read_bytes_zone(ATECC508A, ATCA_ZONE_OTP, 0, ATCA_OTP_SIZE, (uint8_t*)otpconfig);
}

ATCA_STATUS egSelectDevice(ATCAIfaceCfg *cfg)
{
	return atcab_init(cfg);
}

ATCA_STATUS egDetectDevice(void)
{
	return atcab_wakeup(); //try to wake - tests i2c hal and delay function
}

ATCA_STATUS egSerNum(uint8_t *sernum)
{
	return atcab_read_serial_number(sernum);
}

ATCA_STATUS egGenRandom(uint8_t *randnum)
{
	return atcab_random(randnum);
}

ATCA_STATUS egSHA256(uint8_t* message, size_t length, uint8_t* digest)
{
	return atcab_sha(length, message, digest);				//generate digest of message using crypto IC
}

ATCA_STATUS egDevicePubKey(uint8_t *pubkey)
{
	return atcab_get_pubkey(g_cert_def_2_device.private_key_slot, pubkey);
}

ATCA_STATUS egAuthenticate(AuthenticationType auth_type, void *param)
{
	struct auth_chain_inparams *auth3_in;

	switch (auth_type)
	{
		case SYMMETRIC:
			if (param != NULL)
			{
				return ATCA_BAD_PARAM;
			}
			//call symmetric authentication function
			return auth_symmetric_sw();
			break;

		case SW_PKI:
			if (param == NULL)
			{
				return ATCA_BAD_PARAM;
			}

			//call pki device authentication function
			return auth_sw_pki(param);
			break;

		case SW_PKI_CHAIN:
			if (param != NULL)
			{
				return ATCA_BAD_PARAM;
			}
			//call SW authentication function
			return auth_sw_pki_2();
			break;

		case HW_PKI_CHAIN:
			if (param == NULL)
			{
				return ATCA_BAD_PARAM;
			}

			auth3_in = (struct auth_chain_inparams *)param;
			return auth_hw_pki_2(&auth3_in->auth_struct, &auth3_in->digest[0]);
			break;
		
		default:
			return ATCA_BAD_PARAM;
			break;
			
	}

	return ATCA_GEN_FAIL;
}

ATCA_STATUS egAuthResponse(pki_chain_auth_struct* auth_struct, uint8_t* tbs_digest, uint32_t msg_size)
{
	return gen_auth_2_response(auth_struct, tbs_digest, msg_size);
}

ATCA_STATUS egSignTag(uint8_t* buffer, size_t length, uint8_t* signature)
{
	return ecdsa_custom_sign(buffer, length, signature);
}

ATCA_STATUS egVerifyTag(uint8_t* buffer, size_t length, uint8_t* signature)
{
	return ecdsa_custom_verify(buffer, length, signature);
}

ATCA_STATUS egSecureBoot(pki_chain_auth_struct* fw_cert, uint8_t* app_image)
{
	return secureboot_verify_fw_img(fw_cert, app_image);	
}
