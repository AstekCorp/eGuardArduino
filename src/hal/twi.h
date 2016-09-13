#ifndef TWI_H
#define TWI_H

	#include <stdlib.h>
	#include <stdint.h>
	//#include <avr/eeprom.h>
	//#include <avr/interrupt.h>
	//#include <avr/io.h>
	//#include <avr/pgmspace.h>
	//#include <avr/sleep.h>
	//#include <avr/wdt.h>
	//#include <avr/io.h>
	#include <util/twi.h>

	
	/****************************************************************************************************/
	// CryptoMemory Device Address - Must be 0x0-0xF inclusive.  Note that 0x0B is also broadcast address
	// This is the same value in the lower 4-bits of the Device Configuration Register (DCR)
	/****************************************************************************************************/	
	#define TWI_DEVICE_ADDRESS			0x0B	// Value to program in the CryptoMemory TWI address byte
	
	// AT30TS75x register addresses
	#define TEMPER_REG					0x00	// Temperature Data    (R)   Default = 0x0000
	#define CONFIG_REG					0x01	// NonVol Config Copy  (R/W) Default = N/A
	#define TLOLIM_REG					0x02	// NonVol Lo Lim Copy  (R/W) Default = N/A
	#define THILIM_REG					0x03	// NonVol Hi Lim Copy  (R/W) Default = N/A
	#define NCONFIG_REG					0x11	// Last Saved Config   (R/W) Default = 0x0000
	#define NTLOLIM_REG					0x12	// Last Saved Lo Temp  (R/W) Default = 0x04B0
	#define NTHILIM_REG					0x13	// Last Saved Hi Temp  (R/W) Default = 0x0500


	// AT30TSE002B register addresses
	#define CAPABIL_REG					0x00	// Capability       (R)   Default = 0x00D7
	#define CONFIGUR_REG				   0x01	// Configuration    (R/W) Default = 0x0000
	#define TRIP_UPR_REG				   0x02	// Upper Alarm      (R/W) Default = 0x0000
	#define TRIP_LWR_REG				   0x03	// Lower Alarm      (R/W) Default = 0x0000
	#define TRIP_CRT_REG				   0x04	// Critical Alarm   (R/W) Default = 0x0000
	#define TEMP_DATA_REG				0x05	// Temperature Data (R)   Default = N/A
	#define MANF_ID_REG					0x06	// Manufacturer ID  (R)   Default = 0x001F
	#define DEV_ID_REG					0x07	// Dev ID/Revision  (R)   Default = 0x8201
	#define SMBUS_TO_REG				   0x22	// SMBus Timeout    (R/W) Default = 0x0000


	/****************************************************************************************************/
	// TYPES
	/****************************************************************************************************/
	#define vuint8_t					   volatile uint8_t
	#define vuint16_t					   volatile uint16_t
	#define NORMAL						   0
	#define ADDRESS						1

	/****************************************************************************************************/
	// Temperature Sensor COMMAND SET
	/****************************************************************************************************/

	#define WR_ADDR						0x36	// AT30TSE002B reg write, A2-A0 = 000b (0x30)
	#define RD_ADDR						0x37	// AT30TSE002B reg read,  A2-A0 = 000b (0x31)
	
	#define EE_WRITE					0xA6	// AT30TSE002B or AT30TS75x EE write, A2-A0 = 000b (0xA0)
	#define EE_READ						0xA7	// AT30TSE002B or AT30TS75x EE read,  A2-A0 = 000b (0xA1)


	/****************************************************************************************************/
	// MACROS
	/****************************************************************************************************/
	#define TWSR_status					(TWSR & 0xF8)
	#define TWSR_status_is_not(cond) (TWSR_status != cond)		//< TWI TWSR status query
	#define TWI_MEM_ADDR_LEN_MAX     (3)

	/****************************************************************************************************/
	// RETURN CODES
	/****************************************************************************************************/
	typedef enum {
		TWI_SUCCESS = (vuint8_t)0x0,	//< Function execution completed successfully
		TWI_BUS_ERROR,				      //< TWI communications error
		TWI_START_ERROR,
		TWI_ADDR_LEN_INVALID
	} RETURN_CODE;


	/****************************************************************************************************/
	// AVR Micro controller Hardware configuration
	/****************************************************************************************************/
	//#define TWBR_VAL					0x0C	///< Value to program in the TWI baudrate register
    #define TWBR_VAL					0x03	///< Value to program in the TWI baudrate register
    #define SCLFREQ100KHZ				100000
	#define SCLFREQ400KHZ               400000

	
	/****************************************************************************************************/
	// FUNCTION PROTOTYPES
	/****************************************************************************************************/
  

	/**
	 * \brief Information concerning the data transmission
	 */
	typedef struct
	{
		//! TWI chip address to communicate with.
		uint8_t chip;
		//! TWI address/commands to issue to the other chip (node).
		uint32_t addr;
		//! Length of the TWI data address segment (1-3 bytes).
		uint8_t addr_length;
		//! Where to find the data to be written.
		void *buffer;
		//! How many bytes do we want to write.
		uint32_t length;
	
		//! This flag tells the low level drive
		//  to check for ack only
		uint8_t chk_ack_only_flag;

	}
	__attribute__ ((packed)) twi_package_t;


	/**
	 * \brief Input parameters when initializing the twim module mode
	 */
	typedef struct
	{
		//! The PBA clock frequency.
		uint32_t pba_hz;
		//! The baudrate of the TWI bus.
		uint32_t speed;
		//! The desired address.
		uint32_t chip;
		//! SMBUS mode
		//bool smbus;
	}
	twim_options_t;

// #define twi_master_options_t twim_options_t
// 
// #define TWI_MODULE &AVR32_TWIM0  

  
	ATCA_STATUS twi_master_init(uint8_t);
	ATCA_STATUS twi_master_write(const twi_package_t *packet);   
	ATCA_STATUS twi_master_read(const twi_package_t *packet);
	ATCA_STATUS twi_master_write_read(const twi_package_t *packet);   


#endif


