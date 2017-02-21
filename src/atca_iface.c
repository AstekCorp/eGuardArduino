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
 *
 * \brief  Atmel Crypto Auth hardware interface object
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
#include "atca_iface.h"
#include "hal/atca_hal.h"

/** \defgroup interface ATCAIface (atca_)
 *  \brief Abstract interface to all CryptoAuth device types.  This interface
 *  connects to the HAL implementation and abstracts the physical details of the
 *  device communication from all the upper layers of CryptoAuthLib
   @{ */

/** \brief atca_iface is the C object backing ATCAIface.  See the atca_iface.h file for
 * details on the ATCAIface methods
 */

struct atca_iface {
	ATCAIfaceType mType;
	ATCAIfaceCfg  *mIfaceCFG;   // points to previous defined/given Cfg object, caller manages this

	ATCA_STATUS (*atinit)(void *hal, ATCAIfaceCfg *);
	ATCA_STATUS (*atpostinit)(ATCAIface hal);
	ATCA_STATUS (*atsend)(ATCAIface hal, uint8_t *txdata, uint16_t txlength);
	ATCA_STATUS (*atreceive)( ATCAIface hal, uint8_t *rxdata, uint16_t *rxlength);
	ATCA_STATUS (*atwake)(ATCAIface hal);
	ATCA_STATUS (*atidle)(ATCAIface hal);
	ATCA_STATUS (*atsleep)(ATCAIface hal);

	// treat as private
	void *hal_data;     // generic pointer used by HAL to point to architecture specific structure
	                    // no ATCA object should touch this except HAL, HAL manages this pointer and memory it points to
};

ATCA_STATUS _atinit(ATCAIface caiface, ATCAHAL_t *hal);

/** \brief constructor for ATCAIface objects
 * \param[in] cfg  points to the logical configuration for the interface
 * \return ATCAIface
 */

ATCAIface newATCAIface(ATCAIfaceCfg *cfg)  // constructor
{
	ATCAIface caiface = (ATCAIface)malloc(sizeof(struct atca_iface));

	caiface->mType = cfg->iface_type;
	caiface->mIfaceCFG = cfg;

	if (atinit(caiface) != ATCA_SUCCESS) {
		free(caiface);
		caiface = NULL;
	}

	return caiface;
}

// public ATCAIface methods

ATCA_STATUS atinit(ATCAIface caiface)
{
	ATCA_STATUS status = ATCA_COMM_FAIL;
	ATCAHAL_t hal;

	status = _atinit( caiface, &hal );
  if (status == ATCA_SUCCESS) {
  	status = caiface->atinit( &hal, caiface->mIfaceCFG );
  	if (status == ATCA_SUCCESS) {
	  	caiface->hal_data = hal.hal_data;

		  // Perform the post init
		  status = caiface->atpostinit( caiface );
  	}
  }
	return status;
}

ATCA_STATUS atsend(ATCAIface caiface, uint8_t *txdata, uint16_t txlength)
{
	return caiface->atsend(caiface, txdata, txlength);
}

ATCA_STATUS atreceive( ATCAIface caiface, uint8_t *rxdata, uint16_t *rxlength)
{
	return caiface->atreceive(caiface, rxdata, rxlength);
}

ATCA_STATUS atwake(ATCAIface caiface)
{
	return caiface->atwake(caiface);
}

ATCA_STATUS atidle(ATCAIface caiface)
{
	atca_delay_ms(CMD_DELAY);
	return caiface->atidle(caiface);
}

ATCA_STATUS atsleep(ATCAIface caiface)
{
	atca_delay_ms(CMD_DELAY);
	return caiface->atsleep(caiface);
}

ATCAIfaceCfg * atgetifacecfg(ATCAIface caiface)
{
	return caiface->mIfaceCFG;
}

void* atgetifacehaldat(ATCAIface caiface)
{
	return caiface->hal_data;
}

void deleteATCAIface(ATCAIface *caiface) // destructor
{
	if ( *caiface ) {
		hal_iface_release( (*caiface)->mType, (*caiface)->hal_data);  // let HAL clean up and disable physical level interface if ref count is 0
		free((void*)*caiface);
	}

	*caiface = NULL;
}

ATCA_STATUS _atinit(ATCAIface caiface, ATCAHAL_t *hal)
{
  ATCA_STATUS status;

	// get method mapping to HAL methods for this interface
	status = hal_iface_init( caiface->mIfaceCFG, hal );
	caiface->atinit     = hal->halinit;
	caiface->atpostinit = hal->halpostinit;
	caiface->atsend     = hal->halsend;
	caiface->atreceive  = hal->halreceive;
	caiface->atwake     = hal->halwake;
	caiface->atsleep    = hal->halsleep;
	caiface->atidle     = hal->halidle;
	caiface->hal_data   = hal->hal_data;

	return status;
}
/** @} */
