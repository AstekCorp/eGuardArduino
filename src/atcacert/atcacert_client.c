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
 * \brief Client side cert i/o methods. These declarations deal with the client-side, the node being authenticated,
 *        of the authentication process. It is assumed the client has an ECC CryptoAuthentication device
 *        (e.g. ATECC508A) and the certificates are stored on that device.
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


#include "atcacert_client.h"
#include "cryptoauthlib.h"
#include "basic/atca_basic.h"

int atcacert_read_cert( const atcacert_def_t* cert_def,
                        const uint8_t ca_public_key[ATCA_PUB_KEY_SIZE],
                        uint8_t*              cert,
                        size_t*               cert_size)
{
	int ret = 0;
	atcacert_device_loc_t device_locs[ATCA_SLOT_COUNT];
	size_t device_locs_count = 0;
	size_t i = 0;
	atcacert_build_state_t build_state;

	if (cert_def == NULL || ca_public_key == NULL || cert == NULL || cert_size == NULL)
		return ATCACERT_E_BAD_PARAMS;

	ret = atcacert_get_device_locs(
	    cert_def,
	    device_locs,
	    &device_locs_count,
	    sizeof(device_locs) / sizeof(device_locs[0]),
	    ATCA_BLOCK_SIZE);
	if (ret != ATCA_SUCCESS)
		return ret;

	ret = atcacert_cert_build_start(&build_state, cert_def, cert, cert_size, ca_public_key);
	if (ret != ATCA_SUCCESS)
		return ret;

	for (i = 0; i < device_locs_count; i++) {
		uint8_t data[ATCA_MAX_SLOT_SIZE];
		if (device_locs[i].zone == DEVZONE_DATA && device_locs[i].is_genkey) {
			ret = atcab_get_pubkey(device_locs[i].slot, data);
			if (ret != ATCA_SUCCESS)
				return ret;
		}else {
			size_t start_block = device_locs[i].offset / 32;
			uint8_t block;
			size_t end_block = (device_locs[i].offset + device_locs[i].count) / 32;
			for (block = (uint8_t)start_block; block < end_block; block++) {
				ret = atcab_read_zone(device_locs[i].zone, device_locs[i].slot, block, 0, &data[block * ATCA_BLOCK_SIZE - device_locs[i].offset], ATCA_BLOCK_SIZE);
				if (ret != ATCA_SUCCESS)
					return ret;
			}
		}

		ret = atcacert_cert_build_process(&build_state, &device_locs[i], data);
		if (ret != ATCA_SUCCESS)
			return ret;
	}

	ret = atcacert_cert_build_finish(&build_state);
	if (ret != ATCA_SUCCESS)
		return ret;

	return ATCA_SUCCESS;
}

int atcacert_get_response( uint8_t device_private_key_slot,
                           const uint8_t challenge[RANDOM_NUM_SIZE],
                           uint8_t response[SIGN_RSP_SIZE])
{
	if (device_private_key_slot > ATCA_KEY_ID_MAX || challenge == NULL || response == NULL)
		return ATCACERT_E_BAD_PARAMS;

	return atcab_sign(device_private_key_slot, challenge, response);
}
