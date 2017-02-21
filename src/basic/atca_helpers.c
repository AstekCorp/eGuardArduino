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
 * \brief Helpers to support the CryptoAuthLib Basic API methods
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

#include "atca_helpers.h"
#include <stdlib.h>


#ifdef ATCAPRINTF

#include <stdio.h>

/** \brief convert a binary buffer to a hex string suitable for human reading
 *  \param[in] binary input buffer to convert
 *  \param[in] binLen length of buffer to convert
 *  \param[out] asciihex buffer that receives hex string
 *  \param[out] hexlen the length of the asciihex buffer
 * \return ATCA_STATUS
 */
ATCA_STATUS atcab_bin2hex(const uint8_t* binary, int binLen, char* asciihex, int* asciihexlen)
{
	return atcab_bin2hex_(binary, binLen, asciihex, asciihexlen, true);
}

/** \brief convert a binary buffer to a hex string suitable for human reading
 *  \param[in] inbuff input buffer to convert
 *  \param[in] inbuffLen length of buffer to convert
 *  \param[out] asciihex buffer that receives hex string
 *  \param[inout] asciihexlen the length of the asciihex buffer
 *  \param[inout] addspace indicates whether spaces and returns should be added for pretty printing
 * \return ATCA_STATUS
 */
ATCA_STATUS atcab_bin2hex_(const uint8_t* binary, int binLen, char* asciihex, int* asciihexlen, bool addspace)
{
	int i;
	int hexlen = 0;

	// Verify the inputs
	if ((binary == NULL) || (asciihex == NULL) || (asciihexlen == NULL))
		return ATCA_BAD_PARAM;

	// Initialize the return bytes to all 0s
	memset(asciihex, 0, *asciihexlen);

	// Convert one byte at a time
	for (i = 0; i < binLen; i++) {
		if (hexlen > *asciihexlen) break;
		if ((i % 16 == 0 && i != 0) && addspace) {
			sprintf(&asciihex[hexlen], "\r\n");
			hexlen += 2;
		}
		if (addspace) {
			sprintf(&asciihex[hexlen], "%02X ", *binary++);
			hexlen += 3;
		}else {
			sprintf(&asciihex[hexlen], "%02X", *binary++);
			hexlen += 2;
		}
	}
	*asciihexlen = (int)strlen(asciihex);

	return ATCA_SUCCESS;
}

/** \brief convert a binary buffer to a hex string suitable for human reading
 *  \param[in] inbuff input buffer to convert
 *  \param[in] inbuffLen length of buffer to convert
 *  \param[out] outbuff buffer that receives hex string
 *  \return string length of the output buffer
 */
ATCA_STATUS atcab_hex2bin(const char* asciiHex, int asciiHexLen, uint8_t* binary, int* binLen)
{
	int i = 0;
	int j = 0;
	uint32_t byt;
	char* packedHex = NULL;
	int packedLen = asciiHexLen;
	char hexByte[3];

	// Verify the inputs
	if ((binary == NULL) || (asciiHex == NULL) || (binLen == NULL))
		return ATCA_BAD_PARAM;

	// Pack the bytes (remove white space & make even number of characters)
	packedHex = (char*)malloc(packedLen);
	memset(packedHex, 0, packedLen);
	packHex(asciiHex, asciiHexLen, packedHex, &packedLen);

	// Initialize the binary buffer to all 0s
	memset(binary, 0, *binLen);
	memset(hexByte, 0, 3);

	// Convert the ascii bytes to binary
	for (i = 0, j = 0; i < packedLen; i += 2, j++) {
		if (i > packedLen || j > *binLen) break;
		// Copy two characters to be scanned
		memcpy(hexByte, &packedHex[i], 2);
		sscanf(hexByte, "%x", (unsigned int*)&byt);
		// take the msb of the uint32_t
		binary[j] = byt;
	}
	*binLen = j;
	free(packedHex);
	return ATCA_SUCCESS;
}

//#else


#endif

/**
 * \brief Checks to see if a character is an ASCII representation of a digit ((c ge '0') and (c le '9'))
 * \param[in] c  character to check
 * \return True if the character is a digit
 */
bool isDigit(char c)
{
	return (c >= '0') && (c <= '9');
}

/**
 * \brief Checks to see if a character is whitespace ((c == '\n') || (c == '\r') || (c == '\t') || (c == ' '))
 * \param[in] c  character to check
 * \return True if the character is whitespace
 */
bool isWhiteSpace(char c)
{
	return (c == '\n') || (c == '\r') || (c == '\t') || (c == ' ');
}

/**
 * \brief Checks to see if a character is an ASCII representation of hex ((c ge 'A') and (c le 'F')) || ((c ge 'a') and (c le 'f'))
 * \param[in] c  character to check
 * \return True if the character is a hex
 */
bool isHexAlpha(char c)
{
	return ((c >= 'A') && (c <= 'F')) || ((c >= 'a') && (c <= 'f'));
}

/**
 * \brief Returns true if this character is a valid hex character or if this is whitespace (The character can be
 *        included in a valid hexstring).
 * \param[in] c  character to check
 * \return True if the character can be included in a valid hexstring
 */
bool isHex(char c)
{
	return isDigit(c) || isWhiteSpace(c) || isHexAlpha(c);
}

/**
 * \brief Returns true if this character is a valid hex character.
 * \param[in] c  character to check
 * \return True if the character can be included in a valid hexstring
 */
bool isHexDigit(char c)
{
	return isDigit(c) || isHexAlpha(c);
}

ATCA_STATUS packHex(const char* asciiHex, int asciiHexLen, char* packedHex, int* packedLen)
{
	int i = 0;
	int j = 0;

	// Verify the inputs
	if ((asciiHex == NULL) || (packedHex == NULL) || (packedLen == NULL))
		return ATCA_BAD_PARAM;

	// Loop through each character and only add the hex characters
	for (i = 0; i < asciiHexLen; i++) {
		if (isHexDigit(asciiHex[i])) {
			if (j > *packedLen) break;
			packedHex[j++] = asciiHex[i];
		}
	}
	// If there are not an even number of characters, then pad with a '0'

	return ATCA_SUCCESS;
}

