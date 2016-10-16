#ifndef ASTEKCRYPTO_H_
#define ASTEKCRYPTO_H_

#include "atca_iface.h"
#include "cryptoauthlib.h"


#ifdef __cplusplus
extern "C" {
#endif

//External variable declared in astek_hal.c
extern ATCAIfaceCfg *cfg_eGuard;

/**********************************************************************************************//**
 * \struct	OTPConfig
 *
 * \brief	The OTP Zone contents
 **************************************************************************************************/
typedef struct 
{
	//Format/Version of OTP zone
	uint8_t OTPFormat;
	//Configuration Type
	uint8_t ConfigType[2];
	//Major Revision
	uint8_t RevMaj;
	//Minor Revision
	uint8_t RevMin;
	//For future use
	uint8_t Reserved[11];
	
	//ASCII fields
	//Astek in ASCII
	uint8_t Astek[5];
	//Provisioning year
	uint8_t ProvYear[2];
	//Provisioning month
	uint8_t ProvMonth[2];
	//Provisioning day
	uint8_t ProvDay[2];
	//For future use
	uint8_t Reserved2[5];
	//32 byte description field
	uint8_t Description[32];
}OTPConfig;

/**********************************************************************************************//**
 * \enum	AuthenticationType
 *
 * \brief	Enumeration representing different types of authentications
 **************************************************************************************************/
typedef enum {SYMMETRIC, PKI_DEVICE, PKI_CHAIN}AuthenticationType;

/**********************************************************************************************//**
 * \fn	ATCA_STATUS egGetRev(char *rev);
 *
 * \brief	Returns 12 byte string containing SW revision
 *
 * \param [in,out]	ptr to revision buffer 12 characters long.
 *
 * \return	Status of operation, returns 0 if success
 **************************************************************************************************/
ATCA_STATUS egGetRev(char *rev);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS egGetConfig(OTPConfig *otpconfig);
 *
 * \brief	Get 64 byte device configuration written into OTP zone
 *
 * \param [in,out]	ptr to 64 byte buffer
 *
 * \return	Status of operation, return 0 if success otherwise error code
 * 			
 * \example of usage
 * 			ATCA_STATUS status;
 * 			OTPConfig my_otp_struct;
 * 			
 * 			egSelectDevice(cfg_eGuard);
 * 			status = egGetConfig(&my_otp_struct);
 * 			if (status == ATCA_SUCESS){
 * 				printf("%s\n", my_otp_struct.Description);
 * 			}
 **************************************************************************************************/
ATCA_STATUS egGetConfig(OTPConfig *otpconfig);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS egSelectDevice(ATCAIfaceCfg *cfg);
 *
 * \brief	Selects target device for crypto operations. This function only needs to be called 
 * 			when initializing a device or when changing between different target devices.
 *
 * \param [in,out]	ptr to device configuration structure.
 * 					Example structures can be found in astek_hal.c file
 *
 * \return	Status of operation, return 0 if success
 * 			
 * \example of usage	
 * 			ATCA_STATUS status;
 * 			status = egSelectDevice(&cfg_device_e0);
 * 			if (status != ATCA_SUCCESS){
 * 				//Operation failed and new device couldn't be selected as target device
 * 			}
 **************************************************************************************************/
ATCA_STATUS egSelectDevice(ATCAIfaceCfg *cfg);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS egDetectDevice();
 *
 * \brief	Determines if device is properly connected and reachable
 *
 * \return	Status of operation. Returns 0 if device is detected successfully, error code otherwise
 * 			
 * \example of usage	
 * 			ATCA_STATUS = status;
 * 			
 * 			egSelectDevice(cfg_eGuard);		//Sanity check to make sure correct device is targeted
 * 			
 * 			status = egDetectDevice();
 * 			if (status == ATCA_SUCCESS){
 * 				// Device was detected successfully
 * 			}
 *			else{
 *				// Device couldn't be detected
 *			}
 **************************************************************************************************/
ATCA_STATUS egDetectDevice();

/**********************************************************************************************//**
 * \fn	ATCA_STATUS egSerNum(uint8_t *sernum);
 *
 * \brief	Returns crypto IC serial number in array pointed to by *sernum
 *
 * \param [in,out]	ptr to 9 bytes serial number array
 *
 * \return	Status of operation. Returns 0 if device is detected successfully, error code otherwise
 * 			
 * \example of usage
 * 			ATCA_STATUS status; 			
 * 			uint8_t my_sernum[ATCA_SERIAL_NUM_SIZE];		//serial number array buffer
 * 			
 *			egSelectDevice(cfg_eGuard);						//initalize device
 *			status = egSerNum(&my_sernum[0]);				//retrieve serial number
 *			if(status == ATCA_SUCCESS){						//display serial number if success
 *				for(int i=0;i<ATCA_SERIAL_NUM_SIZE;++i)
 *					printf("%d", my_sernum[i]);
 *			}
 **************************************************************************************************/
ATCA_STATUS egSerNum(uint8_t *sernum);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS egGenRandom(uint8_t *randnum);
 *
 * \brief	Generate a 32 byte random number from crypto IC
 *
 * \param [in,out]	ptr to 32 byte buffer for random number
 *
 * \return	Status of operation. Returns 0 if device is detected successfully, error code otherwise
 * 			
 * \example of usage
 *			ATCA_STATUS status;
 *			uint8_t my_randnumber[ATCA_BLOCK_SIZE];		//randum number array buffer
 *
 *			egSelectDevice(cfg_eGuard);					//initialize device
 *			status = egGenRandom(&my_randnumber[0]);	//generate random number
 *			if(status == ATCA_SUCCESS){					//display random number if success
 *				for(int i=0;i<ATCA_SERIAL_NUM_SIZE;++i)
 *					printf("%d", my_sernum[i]);
 *			}
 **************************************************************************************************/
ATCA_STATUS egGenRandom(uint8_t *randnum);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS egGetPubKey(uint8_t *pubkey) ATCA_STATUS egAuthenticate(SecurityLevel level, uint8_t *param);
 *
 * \brief	Returns public key corresponding to private key slot designated in device configuration file.
 *
 * \param [in,out]	pubkey	ptr to 64 byte public key buffer 
 *
 * \return	Status of operation. Returns 0 if device is detected successfully, error code otherwise
 *
 * \example of usage
 *			ATCA_STATUS status;
 *			uint8_t my_pubkey[ATCA_PUB_KEY_SIZE];		//64 byte public key buffer
 *
 *			egSelectDevice(cfg_eGuard);					//initialize device
 *			status = egDevicePubKey(&my_pubkey[0]);		//Retrieve device public key
 *			if(status == ATCA_SUCCESS){					//display random number if success
 *				for(int i=0;i<ATCA_PUB_KEY_SIZE;++i)
 *					printf("%d", my_pubkey[i]);
 *			}
 **************************************************************************************************/
ATCA_STATUS egDevicePubKey(uint8_t *pubkey);

/**********************************************************************************************//**
 * \fn	ATCA_STATUS egAuthenticate(AuthenticationType auth_type, uint8_t *param);
 *
 * \brief	Performs different authentication schemes based on input type.
 *
 * \param 		  	auth_type	The type of authentication. One of the values AuthenticationType
 * 								enumeration.
 * 								SYMMETRIC:	Performs symmetric authentication between key in FW and key in HW
 * 								PKI_DEVICE:	Performs elliptic curve authentication only verifying if device private
 * 											key is pair to the input public key
 * 											param argument should point to 64 byte public key when this mode is used
 * 								PKI_CHAIN:	Performs elliptic curve authentication verifying the entire chain of trust
 * 											up to the root key. This authentication will fail if device is not properly
 * 											provisioned or not part of the chain of trust specified by root key in FW.
 * 								
 * \param [in,out]	param	 	Auxiliary parameter used in different ways by function depending on input type called.
 * 								SYMMETRIC:	parameter should be NULL
 * 								PKI_DEVICE:	parameter should point to 64 byte device public key
 * 								PKI_CHAIN:	parameter should be NULL
 *
 * \return	Status of operation. Returns 0 if device is detected successfully, error code otherwise
 * 			
 * \example of usage
 * 		SYMMETRIC
 *			ATCA_STATUS status;
 *
 *			egSelectDevice(cfg_eGuard);					//initialize device
 *			status = egAuthenticate(SYMMETRIC, NULL);	//Perform symmetric authentication with the IC
 *			if(status != ATCA_SUCCESS){
 *				return ATCA_VERIFY_FAIL;				//Authentication failed so don't let program run
 *			}
 *						
 *		PKI_DEVICE
 *			ATCA_STATUS status;
 *
 *			egSelectDevice(cfg_eGuard);							//initialize device
 *			
 *			status = egAuthenticate(PKI_DEVICE, &device_pubkey);//device_public key should be stored in FW already
 *			if(status != ATCA_SUCCESS){
 *				return ATCA_VERIFY_FAIL;						//authentication failed so don't let program run
 *			}
 *					
 *		PKI_CHAIN
  *			ATCA_STATUS status;
  *
  *			egSelectDevice(cfg_eGuard);					//initialize device
  *
  *			status = egAuthenticate(PKI_CHAIN, NULL);	//device_public key should be stored in FW already
  *			if(status != ATCA_SUCCESS){
  *				return ATCA_VERIFY_FAIL;				//authentication failed so don't let program run
  *			}
  *	
 **************************************************************************************************/
ATCA_STATUS egAuthenticate(AuthenticationType auth_type, uint8_t *param);



#ifdef __cplusplus
}
#endif

#endif /* ASTEKCRYPTO_H_ */