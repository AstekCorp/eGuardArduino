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
 * \brief Atmel CryptoAuthentication device command builder - this is the main object that builds the command
 * byte strings for the given device.  It does not execute the command.  The basic flow is to call
 * a command method to build the command you want given the parameters and then send that byte string
 * through the device interface.
 *
 * The primary goal of the command builder is to wrap the given parameters with the correct packet size and CRC.
 * The caller should first fill in the parameters required in the ATCAPacket parameter given to the command.
 * The command builder will deal with the mechanics of creating a valid packet using the parameter information.
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

#include <stdlib.h>
#include <string.h>
#include "atca_command.h"
#include "atca_devtypes.h"

/** \defgroup command ATCACommand (atca_)
    \brief CryptoAuthLib command builder object, ATCACommand.  Member functions for the ATCACommand object.
   @{ */

/** \brief atca_command is the C object backing ATCACommand.  See the atca_command.h file for
 * details on the ATCACommand methods
 */
struct atca_command {
	ATCADeviceType dt;
	uint16_t *execution_times;
};


/** \brief constructor for ATCACommand
 * \param[in] device_type - specifies which set of commands and execution times should be associated with this command object
 * \return ATCACommand instance
 */
ATCACommand newATCACommand( ATCADeviceType device_type )  // constructor
{
	ATCA_STATUS status = ATCA_SUCCESS;
	ATCACommand cacmd = (ATCACommand)malloc(sizeof(struct atca_command));

	cacmd->dt = device_type;
	status = atInitExecTimes(cacmd, device_type);  // setup typical execution times for this device type

	if (status != ATCA_SUCCESS) {
		free(cacmd);
		cacmd = NULL;
	}

	return cacmd;
}


// full superset of commands goes here

/** \brief ATCACommand CheckMAC method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atCheckMAC(ATCAPacket *packet )
{
	// Set the opcode & parameters
	packet->opcode = ATCA_CHECKMAC;
	packet->txsize = CHECKMAC_COUNT;
	packet->rxsize = CHECKMAC_RSP_SIZE;

	atCalcCrc( packet );
	return ATCA_SUCCESS;
}

/** \brief ATCACommand Counter method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atCounter(ATCACommand cacmd, ATCAPacket *packet )
{
	if ( !atIsECCFamily( cacmd->dt ) )
		return ATCA_BAD_OPCODE;

	// Set the opcode & parameters
	packet->opcode = ATCA_COUNTER;
	packet->txsize = COUNTER_COUNT;
	packet->rxsize = COUNTER_RSP_SIZE;

	atCalcCrc( packet );
	return ATCA_SUCCESS;
}

/** \brief ATCACommand DeriveKey method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \param[in] hasMAC  hasMAC determines if MAC data is present in the packet input
 * \return ATCA_STATUS
 */
ATCA_STATUS atDeriveKey(ATCAPacket *packet, bool hasMAC)
{
	// Set the opcode & parameters
	packet->opcode = ATCA_DERIVE_KEY;

	// hasMAC must be given since the packet does not have any implicit information to
	// know if it has a mac or not unless the size is preset
	if (hasMAC)
	{
		packet->txsize = DERIVE_KEY_COUNT_LARGE;
	}
	else
	{
		packet->txsize = DERIVE_KEY_COUNT_SMALL;
	}

	packet->rxsize = DERIVE_KEY_RSP_SIZE;
	atCalcCrc( packet );
	return ATCA_SUCCESS;
}

/** \brief ATCACommand ECDH method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atECDH(ATCAPacket *packet )
{

	// Set the opcode & parameters
	packet->opcode = ATCA_ECDH;
	packet->txsize = ECDH_COUNT;
	packet->rxsize = ECDH_RSP_SIZE;

	atCalcCrc( packet );
	return ATCA_SUCCESS;
}

/** \brief ATCACommand Generate Digest method
 * \param[in] cacmd     instance
 * \param[in] packet    pointer to the packet containing the command being built
 * \param[in] hasMACKey
 * \return ATCA_STATUS
 */
ATCA_STATUS atGenDig(ATCAPacket *packet, bool hasMACKey)
{

	// Set the opcode & parameters
	packet->opcode = ATCA_GENDIG;

	if ( packet->param1 == NONCE_MODE_MASK ) // shared nonce mode
		packet->txsize = GENDIG_COUNT + GENDIG_OTHER_DATA_SIZE;
	else if ( hasMACKey == true )
		packet->txsize = GENDIG_COUNT_DATA;
	else
		packet->txsize = GENDIG_COUNT;

	packet->rxsize = GENDIG_RSP_SIZE;

	atCalcCrc( packet );
	return ATCA_SUCCESS;
}

/** \brief ATCACommand Generate Key method
 * \param[in] cacmd     instance
 * \param[in] packet    pointer to the packet containing the command being built
 * \param[in] isPubKey  indicates whether "other data" is present in packet
 * \return ATCA_STATUS
 */
ATCA_STATUS atGenKey(ATCAPacket *packet, bool isPubKey)
{

	// Set the opcode & parameters
	packet->opcode = ATCA_GENKEY;

	if (isPubKey)
	{
		packet->txsize = GENKEY_COUNT_DATA;
	}
	else
	{
		packet->txsize = GENKEY_COUNT;	
	}

	packet->rxsize = GENKEY_RSP_SIZE_LONG;

	atCalcCrc( packet );
	return ATCA_SUCCESS;
}

/** \brief ATCACommand HMAC method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atHMAC(ATCAPacket *packet )
{

	// Set the opcode & parameters
	packet->opcode = ATCA_HMAC;
	packet->txsize = HMAC_COUNT;
	packet->rxsize = HMAC_RSP_SIZE;

	atCalcCrc( packet );
	return ATCA_SUCCESS;
}

/** \brief ATCACommand Info method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atInfo( ATCAPacket *packet )
{

	// Set the opcode & parameters
	packet->opcode = ATCA_INFO;
	packet->txsize = INFO_COUNT;
	packet->rxsize = INFO_RSP_SIZE;

	atCalcCrc( packet );
	return ATCA_SUCCESS;
}

/** \brief ATCACommand Lock method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atLock(  ATCAPacket *packet )
{

	// Set the opcode & parameters
	packet->opcode = ATCA_LOCK;
	packet->txsize = LOCK_COUNT;
	packet->rxsize = LOCK_RSP_SIZE;

	atCalcCrc( packet );
	return ATCA_SUCCESS;
}

/** \brief ATCACommand MAC method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atMAC( ATCAPacket *packet )
{

	// Set the opcode & parameters
	// variable packet size
	packet->opcode = ATCA_MAC;
	if ( packet->param1 == 0 )
		packet->txsize = MAC_COUNT_LONG;
	else
		packet->txsize = MAC_COUNT_SHORT;

	packet->rxsize = MAC_RSP_SIZE;

	atCalcCrc( packet );
	return ATCA_SUCCESS;
}

/** \brief ATCACommand Nonce method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atNonce( ATCAPacket *packet )
{
	// Set the opcode & parameters
	// variable packet size
	packet->opcode = ATCA_NONCE;
	int mode = packet->param1 & NONCE_MODE_MASK;

	if ( (mode == NONCE_MODE_SEED_UPDATE || mode == NONCE_MODE_NO_SEED_UPDATE) ) {       // mode[0:1] == 0 | 1 then NumIn is 20 bytes
		packet->txsize = NONCE_COUNT_SHORT; // 20 byte challenge
		packet->rxsize = NONCE_RSP_SIZE_LONG;
	} else if ( mode == NONCE_MODE_PASSTHROUGH ) {            // NumIn is 32 bytes
		packet->txsize = NONCE_COUNT_LONG;  // 32 byte challenge
		packet->rxsize = NONCE_RSP_SIZE_SHORT;
	} else
		return ATCA_BAD_PARAM;

	atCalcCrc( packet );
	return ATCA_SUCCESS;
}

/** \brief ATCACommand Pause method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atPause( ATCAPacket *packet )
{
	// Set the opcode & parameters
	packet->opcode = ATCA_PAUSE;
	packet->txsize = PAUSE_COUNT;
	packet->rxsize = PAUSE_RSP_SIZE;

	atCalcCrc( packet );
	return ATCA_SUCCESS;
}

/** \brief ATCACommand PrivWrite method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atPrivWrite( ATCAPacket *packet )
{
	// Set the opcode & parameters
	packet->opcode = ATCA_PRIVWRITE;
	packet->txsize = PRIVWRITE_COUNT;
	packet->rxsize = PRIVWRITE_RSP_SIZE;

	atCalcCrc( packet );
	return ATCA_SUCCESS;
}

/** \brief ATCACommand Random method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atRandom( ATCAPacket *packet )
{

	// Set the opcode & parameters
	packet->opcode = ATCA_RANDOM;
	packet->txsize = RANDOM_COUNT;
	packet->rxsize = RANDOM_RSP_SIZE;

	atCalcCrc( packet );
	return ATCA_SUCCESS;
}

/** \brief ATCACommand Read method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atRead( ATCAPacket *packet )
{

	// Set the opcode & parameters
	packet->opcode = ATCA_READ;
	packet->txsize = READ_COUNT;

	// variable response size based on read type
	if ((packet->param1 & READ_ZONE_MASK) == 0 )
		packet->rxsize = READ_4_RSP_SIZE;
	else
		packet->rxsize = READ_32_RSP_SIZE;

	atCalcCrc( packet );
	return ATCA_SUCCESS;
}

/** \brief ATCACommand SHA method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atSHA( ATCAPacket *packet )
{
	if ( packet->param2 > SHA_BLOCK_SIZE )
		return ATCA_BAD_PARAM;

	if ( packet->param1 == SHA_MODE_SHA256_UPDATE && packet->param2 != SHA_BLOCK_SIZE )
		return ATCA_BAD_PARAM;                                              // updates should always have 64 bytes of data

	if ( packet->param1 == SHA_MODE_SHA256_END && packet->param2 > SHA_BLOCK_SIZE - 1 )    // END should be 0-63 bytes
		return ATCA_BAD_PARAM;

	// Set the opcode & parameters
	packet->opcode = ATCA_SHA;

	switch ( packet->param1 ) {
	case SHA_MODE_SHA256_START: // START
		packet->rxsize = SHA_RSP_SIZE_SHORT;
		packet->txsize = SHA_COUNT_LONG;
		break;
	case SHA_MODE_SHA256_UPDATE: // UPDATE
		packet->rxsize = SHA_RSP_SIZE_SHORT;
		packet->txsize = SHA_COUNT_LONG + SHA_BLOCK_SIZE;
		break;
	case SHA_MODE_SHA256_END: // END
		packet->rxsize = SHA_RSP_SIZE_LONG;
		// check the given packet for a size variable in param2.  If it is > 0, it should
		// be 0-63, incorporate that size into the packet
		packet->txsize = SHA_COUNT_LONG + packet->param2;
		break;
	default:
		return ATCA_BAD_PARAM;
		break;
	}

	atCalcCrc( packet );
	return ATCA_SUCCESS;
}

/** \brief ATCACommand Sign method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atSign( ATCAPacket *packet )
{

	// Set the opcode & parameters
	packet->opcode = ATCA_SIGN;
	packet->txsize = SIGN_COUNT;

	// could be a 64 or 72 byte response depending upon the key configuration for the KeyID
	packet->rxsize = ATCA_RSP_SIZE_64;

	atCalcCrc( packet );
	return ATCA_SUCCESS;
}

/** \brief ATCACommand UpdateExtra method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atUpdateExtra( ATCAPacket *packet )
{

	// Set the opcode & parameters
	packet->opcode = ATCA_UPDATE_EXTRA;
	packet->txsize = UPDATE_COUNT;
	packet->rxsize = UPDATE_RSP_SIZE;

	atCalcCrc( packet );
	return ATCA_SUCCESS;
}

/** \brief ATCACommand ECDSA Verify method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atVerify( ATCAPacket *packet )
{

	// Set the opcode & parameters
	packet->opcode = ATCA_VERIFY;

	// variable packet size based on mode
	switch ( packet->param1 ) {
	case VERIFY_MODE_STORED:  // Stored mode
		packet->txsize = VERIFY_256_STORED_COUNT;
		break;
	case VERIFY_MODE_VALIDATEEXTERNAL:  // ValidateExternal mode
		packet->txsize = VERIFY_256_EXTERNAL_COUNT;
		break;
	case VERIFY_MODE_EXTERNAL:  // External mode
		packet->txsize = VERIFY_256_EXTERNAL_COUNT;
		break;
	case VERIFY_MODE_VALIDATE:     // Validate mode
	case VERIFY_MODE_INVALIDATE:     // Invalidate mode
		packet->txsize = VERIFY_256_VALIDATE_COUNT;
		break;
	default:
		return ATCA_BAD_PARAM;
		break;
	}
	packet->rxsize = VERIFY_RSP_SIZE;

	atCalcCrc( packet );
	return ATCA_SUCCESS;
}

/** \brief ATCACommand Write method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atWrite( ATCAPacket *packet )
{
	int macsize;
	int writesize;

	// Set the opcode & parameters
	packet->opcode = ATCA_WRITE;

	macsize = ( packet->param1 & WRITE_ZONE_WITH_MAC ? ATCA_ZONE_ACCESS_32 : 0 );  // if encrypted, use MAC
	writesize = ( packet->param1 & ATCA_ZONE_READWRITE_32 ? ATCA_ZONE_ACCESS_32 : ATCA_ZONE_ACCESS_4 );

	if ( macsize == WRITE_ZONE_WITH_MAC && writesize == ATCA_ZONE_ACCESS_32 )
		packet->txsize = WRITE_COUNT_LONG_MAC;
	else if ( macsize == WRITE_ZONE_WITH_MAC && writesize == ATCA_ZONE_ACCESS_4 )
		packet->txsize = WRITE_COUNT_SHORT_MAC;
	else if ( macsize == 0 && writesize == ATCA_ZONE_ACCESS_32 )
		packet->txsize = WRITE_COUNT_LONG;
	else if ( macsize == 0 && writesize == ATCA_ZONE_ACCESS_4 )
		packet->txsize = WRITE_COUNT_SHORT;

	packet->rxsize = WRITE_RSP_SIZE;
	atCalcCrc( packet );
	return ATCA_SUCCESS;
}

/** \brief ATCACommand Write encrypted method
 * \param[in] cacmd   instance
 * \param[in] packet  pointer to the packet containing the command being built
 * \return ATCA_STATUS
 */
ATCA_STATUS atWriteEnc(ATCAPacket *packet)
{
	// Set the opcode & parameters
	packet->opcode = ATCA_WRITE;
	packet->txsize = WRITE_COUNT_LONG_MAC;
	packet->rxsize = WRITE_RSP_SIZE;
	atCalcCrc( packet );
	return ATCA_SUCCESS;
}

/** \brief ATCACommand destructor
 * \param[in] cacmd instance of a command object
 */

void deleteATCACommand( ATCACommand *cacmd )  // destructor
{
	if ( *cacmd )
		free((void*)*cacmd);

	*cacmd = NULL;
}


/** \brief execution times for x08a family, these are based on the typical value from the datasheet
 */
uint16_t exectimes_x08a[] = {   // in milleseconds
	1,                          // WAKE_TWHI
	13,                         // CMD_CHECKMAC
	20,                         // CMD_COUNTER
	50,                         // CMD_DERIVEKEY
	58,                         // CMD_ECDH
	11,                         // CMD_GENDIG
	115,                        // CMD_GENKEY
	23,                         // CMD_HMAC
	2,                          // CMD_INFO
	32,                         // CMD_LOCK
	14,                         // CMD_MAC
	29,                         // CMD_NONCE
	3,                          // CMD_PAUSE
	48,                         // CMD_PRIVWRITE
	23,                         // CMD_RANDOM with SEED Update mode takes ~21ms, high side of range
	1,                          // CMD_READMEM
	9,                          // CMD_SHA
	60,                         // CMD_SIGN
	10,                         // CMD_UPDATEEXTRA
	72,                         // CMD_VERIFY
	26                          // CMD_WRITE
};


/** \brief execution times for 204a, these are based on the typical value from the datasheet
 */
uint16_t exectimes_204a[] = {
	3,  // WAKE_TWHI
	38, // CMD_CHECKMAC
	0,
	62, // CMD_DERIVEKEY
	0,
	43, // CMD_GENDIG
	0,
	69, // CMD_HMAC
	2,  // CMD_DevRev
	24, // CMD_LOCK
	35, // CMD_MAC
	60, // CMD_NONCE
	2,  // CMD_PAUSE
	0,
	50, // CMD_RANDOM
	4,  // CMD_READMEM
	22, // CMD_SHA
	0,
	12, // CMD_UPDATEEXTRA
	0,
	42  // CMD_WRITEMEM
};

/** \brief initialize the execution times for a given device type
 *
 * \param[in] cacmd - the command containing the list of execution times for the device
 * \param[in] device_type - the device type - execution times vary by device type
 * \return ATCA_STATUS
 */

ATCA_STATUS atInitExecTimes(ATCACommand cacmd, ATCADeviceType device_type)
{
	switch ( device_type ) {
	case ATECC108A:
	case ATECC508A:
		cacmd->execution_times = exectimes_x08a;
		break;
	case ATSHA204A:
		cacmd->execution_times = exectimes_204a;
		break;
	default:
		return ATCA_BAD_PARAM;
		break;
	}

	return ATCA_SUCCESS;
}

/** \brief return the typical execution type for the given command
 *
 * \param[in] cacmd the command object for which the execution times are associated
 * \param[in] cmd - the specific command for which to lookup the execution time
 * \return typical execution time in milleseconds for the given command
 */

uint16_t atGetExecTime( ATCACommand cacmd, ATCA_CmdMap cmd )
{
	return cacmd->execution_times[cmd];
}


/** \brief This function calculates CRC given raw data, puts the CRC to given pointer
 *
 * \param[in] length size of data not including the CRC byte positions
 * \param[in] data pointer to the data over which to compute the CRC
 * \param[out] crc pointer to the place where the two-bytes of CRC will be placed
 */

void atCRC( uint8_t length, uint8_t *data, uint8_t *crc)
{
	uint8_t counter;
	uint16_t crc_register = 0;
	uint16_t polynom = CRC_POLYNOM;
	uint8_t shift_register;
	uint8_t data_bit, crc_bit;

	for (counter = 0; counter < length; counter++) {
		for (shift_register = CRC_SHIFT1; shift_register > CRC_SHIFT0; shift_register <<= MAXSHIFT) {
			data_bit = (data[counter] & shift_register) ? MAXSHIFT : 0;
			crc_bit = crc_register >> ENDSHIFT;
			crc_register <<= MAXSHIFT;
			if (data_bit != crc_bit)
				crc_register ^= polynom;
		}
	}
	crc[0] = (uint8_t)(LO_BYTE(crc_register)); 
	crc[1] = (uint8_t)(HI_BYTE(crc_register));
}


/** \brief This function calculates CRC and adds it to the correct offset in the packet data
 * \param[in] packet Packet to calculate CRC data for
 */

void atCalcCrc( ATCAPacket *packet )
{
	uint8_t length, *crc;

	length = packet->txsize - ATCA_CRC_SIZE;
	// computer pointer to CRC in the packet
	crc = &(packet->txsize) + length;

	// stuff CRC into packet
	atCRC(length, &(packet->txsize), crc);
}


/** \brief This function checks the consistency of a response.
 * \param[in] response pointer to response
 * \return status of the consistency check
 */

uint8_t atCheckCrc(uint8_t *response)
{
	uint8_t crc[ATCA_CRC_SIZE];
	uint8_t count = response[ATCA_COUNT_IDX];

	count -= ATCA_CRC_SIZE;
	atCRC(count, response, crc);

	return (crc[0] == response[count] && crc[1] == response[count + 1]) ? ATCA_SUCCESS : ATCA_BAD_CRC;
}


/** \brief determines if a given device type is a SHA device or a superset of a SHA device
 * \param[in] deviceType - the type of device to check for family type
 * \return boolean indicating whether the given device is a SHA family device.
 */

bool atIsSHAFamily( ATCADeviceType deviceType )
{
	switch (deviceType) {
	case ATSHA204A:
	case ATECC108A:
	case ATECC508A:
		return true;
		break;
	default:
		return false;
		break;
	}
}

/** \brief determines if a given device type is an ECC device or a superset of a ECC device
 * \param[in] deviceType - the type of device to check for family type
 * \return boolean indicating whether the given device is an ECC family device.
 */

bool atIsECCFamily( ATCADeviceType deviceType )
{
	switch (deviceType) {
	case ATECC108A:
	case ATECC508A:
		return true;
		break;
	default:
		return false;
		break;
	}
}

/** \brief checks for basic error frame in data
 * \param[in] data pointer to received data - expected to be in the form of a CA device response frame
 * \return ATCA_STATUS indicating type of error or no error
 */

ATCA_STATUS isATCAError( uint8_t *data )
{
	uint8_t good[ATCA_RSP_SIZE_MIN] = { 0x04, 0x00, 0x03, 0x40 };

	if ( memcmp( data, good, ATCA_RSP_SIZE_MIN ) == 0 )
		return ATCA_SUCCESS;

	if ( data[0] == ATCA_RSP_SIZE_MIN ) {    // error packets are always 4 bytes long
		switch ( data[1] ) {
		case CHECKMAC_CMD_MISMATCH:              // checkmac or verify failed
			return ATCA_CHECKMAC_VERIFY_FAILED;
			break;
		case CMD_STATUS_BYTE_PARSE: // command received byte length, opcode or parameter was illegal
			return ATCA_BAD_OPCODE;
			break;
		case CMD_STATUS_BYTE_EXEC: // chip can't execute the command
			return ATCA_EXECUTION_ERROR;
			break;
		case CMD_STATUS_WAKEUP: // chip was successfully woken up
			return ATCA_WAKE_SUCCESS;
			break;
		case CMD_STATUS_BYTE_COMM: // bad crc found or other comm error
			return ATCA_STATUS_CRC;
			break;
		default:
			return ATCA_GEN_FAIL;
			break;
		}
	} else
		return ATCA_SUCCESS;
}

/** @} */
