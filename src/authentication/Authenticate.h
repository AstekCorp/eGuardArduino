#ifndef AUTHENTICATE_H_
#define AUTHENTICATE_H_
#include "cryptoauthlib.h"

#define ROOT_PUBKEY_ID 0xD
#define SIGNER_PRIV_KEY_ID 0X3
#define SYMMETRIC_KEY_ID	0x1
#define COMPANY_SIZE              (17)
#define COMPANY_NAME  "ASTEK CORPORATION"



/**********************************************************************************************//**
 * \fn	ATCA_STATUS auth_chain_hw(ATCAIfaceCfg *host_cfg, ATCAIfaceCfg *client_cfg)
 *
 * \brief	Hardware Authentication of full Chain of Trust. 
 * 			Utilizes host crypto chip to authenticate client. One master MCU talks to both Host and
 * 			Client crypto chips. Whole chain of trust is authenticated, from device up to root.
 *
 * \param [in]	host_cfg  	Ptr to the host crypto chip configuration structure
 * \param [in]	client_cfg	Ptr to the client crypto chip configuration structure
 *
 * \return	ATCA_STATUS Status of operation
 **************************************************************************************************/
ATCA_STATUS auth_chain_hw(ATCAIfaceCfg *host_cfg, ATCAIfaceCfg *client_cfg);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS auth_chain_sw(ATCAIfaceCfg *client_cfg);
 *
 * \brief	Software Authentication of full Chain of Trust.
 * 			Function may be called to authenticate the crypto chip connected to the system in order 
 * 			to verify its authenticity before relaying on the chip for all cryptography operations
 *
 *
 * \return	An ATCA_STATUS	Return ATCA_SUCCESS on successful authentication
 **************************************************************************************************/
ATCA_STATUS auth_chain_sw();

/**********************************************************************************************//**
 * \fn	ATCA_STATUS auth_dev_sw(ATCAIfaceCfg *dev_cfg, uint8_t *pub_key)
 *
 * \brief	Device software authentication. 
 * 			PKI authentication of crypto device connected to the system implemented in SW.
 * 			Function may be called to authenticate crypto chip connected to the system. Authentication 
 * 			will only succeed if device's private key is pair to input public key
 *
 * \param [in]	pub_key		Ptr to the device public key.
 *
 * \return ATCA_STATUS	Return ATCA_SUCCESS on successful authentication
 **************************************************************************************************/
ATCA_STATUS auth_dev_sw(uint8_t *pub_key);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS auth_symmetric_sw();
 *
 * \brief	Symmetric software authentication. Uses symmetric key on slot 01 to authenticate crypto device
 * 			connected to system. Symmetric key must be valid in symmetric key slot otherwise
 * 			returns error. MAC computation is executed in order to verify key.
 *
 *
 * \return	ATCA_STATUS		Return ATCA_SUCCESS on successful authentication.
 **************************************************************************************************/
ATCA_STATUS auth_symmetric_sw();


// DOXYGEN
ATCA_STATUS sign_product(uint8_t* buffer, size_t length, uint8_t* signature);

// DOXYGEN
ATCA_STATUS verify_product(uint8_t* buffer, size_t length, uint8_t* signature, uint8_t* public_key);



#endif /* AUTHENTICATE_H_ */