
 
#ifndef CUSTOM_HAL_
#define CUSTOM_HAL_

#include "cryptoauthlib.h"

extern ATCAIfaceCfg *cfg_eGuard;

/** \brief Custom configurations for crypto ICs */
#define DEV1_NAME	cfg_device_e0
#define DEV1_ADDR	0xE0
#define DEV1_BUS	2
extern ATCAIfaceCfg *DEV1_NAME;

#define DEV2_NAME	cfg_device_e2
#define DEV2_ADDR	0xE2
#define DEV2_BUS	2
extern ATCAIfaceCfg *DEV2_NAME;

#define DEV3_NAME	cfg_device_c0
#define DEV3_ADDR	0xC0
#define DEV3_BUS	2
extern ATCAIfaceCfg *DEV3_NAME;

#define DEV4_NAME	cfg_device_c2
#define DEV4_ADDR	0xC2
#define DEV4_BUS	2
extern ATCAIfaceCfg *DEV4_NAME;

#define DEV5_NAME	cfg_device_f0
#define DEV5_ADDR	0xF0
#define DEV5_BUS	2
extern ATCAIfaceCfg *DEV5_NAME;


#endif /* CUSTOM_HAL_ */
