#include <stdint.h>
#include "astekcrypto.h"
#include "cryptoauthlib.h"
#include "authentication/Authenticate.h"
#include "authentication/cert_def_2_device.h"

char astek_version[] = { "1.1.0.00110" };  // Maj.Min.svnMaj.svnMin

extern ATCADevice _gDevice;
extern ATCACommand _gCommandObj;
extern ATCAIface _gIface;

ATCAIfaceCfg *gCfg = NULL; //Store device configuration


ATCA_STATUS egGetRev(char *verstr)
{
	strcpy( verstr, astek_version );
	return ATCA_SUCCESS;
}

ATCA_STATUS egGetConfig(OTPConfig *otpconfig)
{
	return atcab_read_bytes_zone(ATECC508A, ATCA_ZONE_OTP, 0, ATCA_OTP_SIZE, (uint8_t*)otpconfig);
}

ATCA_STATUS egSelectDevice(ATCAIfaceCfg *cfg)
{
	return atcab_init(cfg);
}

ATCA_STATUS egDetectDevice()
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

ATCA_STATUS egDevicePubKey(uint8_t *pubkey)
{
	return atcab_get_pubkey(g_cert_def_2_device.private_key_slot,pubkey);
}

ATCA_STATUS egAuthenticate(AuthenticationType level, uint8_t *param)
{
	switch (level)
	{
		case SYMMETRIC:
			//call symmetric authentication function
			return auth_symmetric_sw();
		break;

		case PKI_DEVICE:
			//call pki device authentication function
			return auth_dev_sw(param);
		break;

		case PKI_CHAIN:
			//call SW authentication function
			return auth_chain_sw();
		break;
	}

	return ATCA_GEN_FAIL;
}
