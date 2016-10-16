#include "Authenticate.h"
#include "astekcrypto.h"
#include "cryptoauthlib.h"
#include "atcacert/atcacert_client.h"
#include "atcacert/atcacert_host_hw.h"
#include "atcacert/atcacert_host_sw.h"
#include "cert_def_1_signer.h"
#include "cert_def_2_device.h"
#include "crypto/atca_crypto_sw_sha2.h"
#include "atca_command.h"
#include "host/atca_host.h"

/** \brief global variables for public keys */
uint8_t root_pub_key[64];
uint8_t signer_pub_key[64];
uint8_t device_public_key[64];
size_t  key_size = sizeof(root_pub_key);

/** \brief global variables for signer certificate */
uint8_t g_signer_cert[1024];
size_t  g_signer_cert_size = sizeof(g_signer_cert);

/** \brief global storage for device certificate */
uint8_t g_device_cert[1024];
size_t  g_device_cert_size = sizeof(g_device_cert);

/** \brief global storage for the challenge data to sign by the device */
uint8_t g_challenge[32];
uint8_t g_response[64];

static ATCA_STATUS rebuild_certs(void)
{
	ATCA_STATUS ret = ATCACERT_E_UNIMPLEMENTED;
	
	//Re-initialize cert size variable
	g_signer_cert_size = sizeof(g_signer_cert);
	g_device_cert_size = sizeof(g_device_cert);
	
	ret = atcacert_read_cert(&g_cert_def_1_signer, root_pub_key, &g_signer_cert, &g_signer_cert_size);
	if (ret != ATCA_SUCCESS) return ret;
	
	ret = atcacert_get_subj_public_key(&g_cert_def_1_signer, &g_signer_cert, g_signer_cert_size, signer_pub_key);
	if (ret != ATCA_SUCCESS) return ret;
	
	ret = atcacert_read_cert(&g_cert_def_2_device, signer_pub_key, &g_device_cert, &g_device_cert_size);
	if (ret != ATCA_SUCCESS) return ret;
	
	ret = atcacert_get_subj_public_key(&g_cert_def_2_device, &g_device_cert, g_device_cert_size, device_public_key);
	if (ret != ATCA_SUCCESS) return ret;
	
	return ret;
}

static ATCA_STATUS cert_chain_verify(void)
{
	ATCA_STATUS ret = ATCACERT_E_UNIMPLEMENTED;
	
	//Validate signer certificate against root public key
	ret = atcacert_verify_cert_hw(&g_cert_def_1_signer, &g_signer_cert, g_signer_cert_size, root_pub_key);
	if (ret != ATCA_SUCCESS) return ret;
	
	//Assumes signer key was already read from certificate in rebuilt certificate method
	
	//Validate the device cert against signer key
	ret = atcacert_verify_cert_hw(&g_cert_def_2_device, &g_device_cert, g_device_cert_size, signer_pub_key);
	if (ret != ATCA_SUCCESS) return ret;
	
	//return 0 if all succeeded
	return ret; 
}

static ATCA_STATUS cert_chain_verify_sw(void)
{
	ATCA_STATUS ret = ATCACERT_E_UNIMPLEMENTED;
	uint8_t signature[64];
	bool is_verified = false;
	
	ret = atcacert_verify_cert_sw(&g_cert_def_1_signer, &g_signer_cert, g_signer_cert_size, root_pub_key);
	if (ret != ATCA_SUCCESS) return ret;
	
	//Assumes signer key was already read from certificate in rebuilt certificate method
	
	//Validate the device cert against signer key
	ret = atcacert_verify_cert_sw(&g_cert_def_2_device, &g_device_cert, g_device_cert_size, signer_pub_key);
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


#pragma region Authentication


ATCA_STATUS auth_chain_hw(ATCAIfaceCfg *host_cfg, ATCAIfaceCfg *client_cfg)
{
	ATCA_STATUS ret = ATCACERT_E_UNIMPLEMENTED;
	
	//Read root public key from host
	egSelectDevice(host_cfg);
	ret = atcab_read_pubkey(ROOT_PUBKEY_ID, &root_pub_key[0]);
	if (ret != ATCA_SUCCESS) return ret;
	
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
	return atcacert_verify_response_hw(device_public_key, g_challenge, g_response);
}

ATCA_STATUS auth_chain_sw()
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
	ret = hal_random_number(g_challenge);						//HAL user implemented PRG
	if (ret != ATCA_SUCCESS) return ret;
	
	//Client generate response
	ret = client_generate_response();
	if (ret != ATCA_SUCCESS) return ret;
	
	//Host(FW) verify response
	return atcacert_verify_response_sw(device_public_key, g_challenge, g_response);
}

ATCA_STATUS auth_dev_sw(uint8_t *pub_key)
{
	ATCA_STATUS status = ATCA_UNIMPLEMENTED;
	
	//FW acting as host needs to generate random number
	status = hal_random_number(g_challenge);				//HAL User implemented PRG
	if (status != ATCA_SUCCESS) return status;
	
	//Target crypto device generate response
	status = client_generate_response();
	if (status != ATCA_SUCCESS) return status;
	
	//Host(FW) verify response
	return atcacert_verify_response_sw(pub_key, g_challenge, g_response);
}

ATCA_STATUS auth_symmetric_sw()
{
	ATCA_STATUS status = ATCA_UNIMPLEMENTED;					//Status of operations
	
	uint8_t random_number[32], digest[32], response[32];		//buffer for variables to be calculated
	uint8_t symmetric_key[32];
	uint8_t temporary[ATCA_MSG_SIZE_MAC];						//buffer for temporary operations
	uint8_t *p_temp;											//pointer to temporary buffer

//Generate random number
	hal_random_number(random_number);							//HAL user implemented PRG
	
//Request MAC computation from crypto IC
	status = atcab_mac(0x00, SYMMETRIC_KEY_ID, random_number, digest);
	if (status != ATCA_SUCCESS)
	{
		return status;
	}
		
//Compute key
	atcac_sw_sha2_256(COMPANY_NAME, COMPANY_SIZE, symmetric_key);
	
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
	*p_temp++ = SYMMETRIC_KEY_ID & 0xFF;								//Big endian format
	*p_temp++ = (SYMMETRIC_KEY_ID >> 8) & 0xFF;							

	memset(p_temp, 0, ATCA_OTP_SIZE_8);							// use 8 zeros for (6)
	p_temp += ATCA_OTP_SIZE_8;

	memset(p_temp, 0, ATCA_OTP_SIZE_3);							// use 3 zeros for (7)
	p_temp += ATCA_OTP_SIZE_3;
	
	// (8) 1 byte SN[8] = 0xEE
	*p_temp++ = ATCA_SN_8;
	
	memset(p_temp, 0, ATCA_SN_SIZE_4);							//use zeros for (9)
	p_temp += ATCA_SN_SIZE_4;

	// (10) 2 bytes SN[0:1] = 0x0123
	*p_temp++ = ATCA_SN_0;
	*p_temp++ = ATCA_SN_1;
	
	memset(p_temp, 0, ATCA_SN_SIZE_2);							//use zeros for (9)
	p_temp += ATCA_SN_SIZE_2;
	
	// Calculate SHA256 to get the MAC digest
	atcac_sw_sha2_256(temporary, ATCA_MSG_SIZE_MAC, response);
	
	//Verify keys match
	// TODO - Implement verification to prevent timing attack
	if ( memcmp(&response, &digest, 32) == 0)
	{
		return ATCA_SUCCESS;
	}
	
	return ATCA_CHECKMAC_VERIFY_FAILED;
}


#pragma endregion Authentication


/*brief: sign a unique buffer related to specific product
 *       This function allows you to guarantee only OEM products are allowed to interact
 *		 the system by signing the UID of the product and then using verify function during runtime
 *params[in] buffer:	 ptr to buffer to be signed
 *params[in] length:	 length of buffer
 *params[out] signature: ptr to signature of buffer
*/
//TODO	-	Test and validate function sign_product
ATCA_STATUS sign_product(uint8_t* buffer, size_t length, uint8_t* signature)
{
	ATCA_STATUS ret = ATCA_UNIMPLEMENTED;
	
	/************************************************************************/
	return ATCA_UNIMPLEMENTED;		// UNDONE Function not validated
	/************************************************************************/
	
	auto uint8_t tbs_digest[32];
	
	//SW SHA256 of message to generate tbs_digest
	ret = atcac_sw_sha2_256(buffer, length, tbs_digest);
	
	
	//Check parameters before signing data
	if (SIGNER_PRIV_KEY_ID > 15 || &tbs_digest == NULL || signature == NULL)
	{
		return ATCACERT_E_BAD_PARAMS;	
	}
	atcab_sign(SIGNER_PRIV_KEY_ID, &tbs_digest, signature);
}

/*brief: Verify product signature generated by sign_product function
 *       This function allows you to guarantee only OEM products are allowed to interact
 *		 the system by signing the UID of the product and then using verify function during runtime
 *params[in] buffer:	 ptr to buffer to be signed
 *params[in] length:	 length of buffer
 *params[in] signature:	 ptr to signature of buffer
 *params[in] public_key: ptr to public key of key pair used to create signature
 *return:	Success if everything succeeded and verified or error code if fail
*/
//TODO	-	Test and validate function verify_product
ATCA_STATUS verify_product(uint8_t* buffer, size_t length, uint8_t* signature, uint8_t* public_key)
{
	ATCA_STATUS ret = ATCA_UNIMPLEMENTED;
	
	/************************************************************************/
	return ATCA_UNIMPLEMENTED;		// UNDONE Function not validated
	/************************************************************************/
	
	auto uint8_t tbs_digest[32];
	
	//SW SHA256 of message to generate tbs_digest
	ret = atcac_sw_sha2_256(buffer, length, tbs_digest);
	if (ret != ATCA_SUCCESS)
	{
		return ret;
	}
	
	ret = atcacert_verify_response_hw(public_key, tbs_digest, signature);	
	if (ret != ATCA_SUCCESS)
	{
		return ret;
	}	
}