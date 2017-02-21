#include "atca_hal.h"
#include "hal/atca_hal.h"
#include "custom_hal.h"
#include "atca_device.h"
#include <avr/io.h>
#include "twi.h"
#include <util/delay.h>

//! \internal Pointer to the applicative TWI receive buffer.
static volatile uint8_t *twim_rx_data = NULL;

ATCAIfaceCfg device_e0 = {
	.iface_type       = ATCA_I2C_IFACE,
	.devtype        = ATECC508A,
	.atcai2c.slave_address  = 0xE0,
	.atcai2c.bus      = 2,
	.atcai2c.baud     = 400000,
	//.atcai2c.baud = 100000,
	.wake_delay       = 800,
	.rx_retries       = 20
};

ATCAIfaceCfg *cfg_eGuard = &device_e0;

/*Timer functions*/
/*Examples use atmel ASF supplied routines*/
void atca_delay_ms(uint32_t delay)
{
	int i;
	for (i = 0; i <= (delay); i++)
	{
		_delay_ms(1);
	}
}

void atca_delay_us(uint32_t delay)
{
	int i;
	for (i = 0; i <= (delay); i++)
	{
		_delay_us(1);
	}
}

void atca_delay_10us(uint32_t delay)
{

	int i;
	for (i = 0; i <= (10*delay); i++)
	{
		_delay_us(1);
	}

}


/** \brief initialize an I2C interface using given config
 * \param[in] hal - opaque ptr to HAL data
 * \param[in] cfg - interface configuration
 */
ATCA_STATUS hal_i2c_init(void *hal, ATCAIfaceCfg *cfg)
{
    if(cfg->atcai2c.baud == 400000)
	{
		//set SCL to 400kHz
		TWSR = 0x00;
		TWBR = 0x0C;
	}
	else
	{
		//set SCL to 100kHz
		TWSR = 0x00;
		TWBR = 0x48;		
	}
	
    //enable TWI
    TWCR = (1<<TWEN);

	
return ATCA_SUCCESS;
}

/** \brief HAL implementation of I2C post init
 * \param[in] iface  instance
 * \return ATCA_STATUS
 * \This is a place holder for possible post init hardware requirements
 * \if this function is not used it must return ATCA_SUCCESS
 */
ATCA_STATUS hal_i2c_post_init(ATCAIface iface)
{ 
return ATCA_SUCCESS;
}

/** \brief HAL implementation of I2C write byte
 * \param[in] length of buffer
 * \param[in] ptr to buffer
 * \return ATCA_STATUS
**/
ATCA_STATUS twi_write_bytes(uint32_t length, uint8_t *buffer)
{
	static uint8_t twi_status;
    int attemptsCtr;
	
	while (length > 0)
	{
        attemptsCtr = 1000;
  	    TWDR = *buffer++;
		TWCR = (1 << TWEN) | (1 << TWINT);

 	    // timeout if the system doesn't respond quickly. 
	    do {
	    	attemptsCtr--;
	    } while (bit_is_clear(TWCR, TWINT) && attemptsCtr);
	
	    if (attemptsCtr == 0) {
		    return ATCA_TX_TIMEOUT;
	    }
			
		// Verify success on each byte.
		twi_status = TWSR_status;
		
		//if not ACK force stop
		if((twi_status != TW_MT_SLA_ACK) &&
		(twi_status != TW_MT_DATA_ACK))
		{
			// Issue a TWI stop command to terminate transaction.
			TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTO);
			return TWI_BUS_ERROR;
		}

		length--;
	}
	return TWI_SUCCESS;
}

/** \brief HAL implementation of I2C master write
 * \param[in] ptr to packet structure
 * \return ATCA_STATUS
**/
ATCA_STATUS twi_master_write(const twi_package_t *packet)
{
	uint8_t twi_status;
	uint8_t twi_address = packet->chip;
    int attemptsCtr = 1000;
	
	 /*Enable the transceiver in Master Transmit mode and initiate a START condition.*/
	// timeout if the system doesn't respond quickly.
	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTA);
	do {
		attemptsCtr--;
	} while (bit_is_clear(TWCR, TWINT) && attemptsCtr);
	
	if (attemptsCtr == 0) {
		return ATCA_TX_TIMEOUT;
	}
	
	/*Initialize status*/
	twi_status = TWSR_status;
	
	/*If bus is busy*/
	if (!(twi_status & TW_START) && !(twi_status & TW_REP_START))
	return ATCA_COMM_FAIL;

	 /*Transmit TWI address*/
	twi_status = twi_write_bytes(1, &twi_address);
	if (twi_status != TWI_SUCCESS)
	return twi_status;

	/*Send memory address if exists*/
	if (packet->addr_length > 0) {
		
		uint8_t mem_address[TWI_MEM_ADDR_LEN_MAX];
		mem_address[0] = (packet->addr >> 16) & 0xFF;
		mem_address[1] = (packet->addr >> 8) & 0xFF;
		mem_address[2] = packet->addr & 0xFF;
		twi_status = twi_write_bytes(packet->addr_length, &mem_address[TWI_MEM_ADDR_LEN_MAX - packet->addr_length]);
		if (twi_status != TWI_SUCCESS)
		return twi_status;
	}

	 /*Transmit all of payload or terminate and return on error.*/
	twi_status = twi_write_bytes(packet->length, packet->buffer);
	
	if (twi_status != TWI_SUCCESS)
	return twi_status;

	/*Issue a TWI stop command to terminate transaction*/
	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTO);

	return twi_status;
}
	
/** \brief HAL implementation of I2C Master Write
 * \param[in] iface     instance
 * \param[in] txdata    pointer to space to bytes to send
 * \param[in] txlength  number of bytes to send
 * \return ATCA_STATUS
 */
ATCA_STATUS i2c_master_write(ATCAIface iface, uint8_t *txdata, uint16_t txlength)
{	
	ATCAIfaceCfg *cfg = atgetifacecfg(iface);

    twi_package_t packet;
	/*! TWI chip address to communicate with*/
	packet.chip = cfg->atcai2c.slave_address;
	/*not used for 508A*/
	packet.addr = 0;
	/*not used for 508A*/
	packet.addr_length = 0;
	/*Where to find the data to be written*/
	packet.buffer = txdata;
	/*How many bytes do we want to write*/
	packet.length = txlength;	
	/*! This flag tells the low level drive
	  to check for ack only*/
	packet.chk_ack_only_flag = false;
   
	return twi_master_write(&packet);
}


ATCA_STATUS twi_master_read(const twi_package_t *package)
{

	uint8_t twi_status;
	uint16_t i;
	twim_rx_data = package->buffer;
    int attemptsCtr = 1000;
	
	/*Enable the transmitter, send a START condition and check for successful execution*/
	// timeout if the system doesn't respond quickly.
	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTA) | (1 << TWEA);
	do {
		attemptsCtr--;
	} while (bit_is_clear(TWCR, TWINT) && attemptsCtr);
	
	if (attemptsCtr == 0) {
		return ATCA_RX_TIMEOUT;
	}
	twi_status = TWSR_status;
	
	/*if bus is busy return error*/
	if (!(twi_status & TW_START) && !(twi_status & TW_REP_START))
	return TWI_START_ERROR;

	/*Transmit the device address with TWI READ bit set and check for successful execution*/
	/*Device Address*/
	TWDR = package->chip | 0x01;

	TWCR = (1 << TWEN) | (1 << TWINT);
    // timeout if the system doesn't respond quickly. 
    attemptsCtr = 1000;
	do {
    	attemptsCtr--;
    } while (bit_is_clear(TWCR, TWINT) && attemptsCtr);
	
    if (attemptsCtr == 0) {
	    return ATCA_RX_TIMEOUT;
    }
	
	/*Stop if not ACK*/
	if (TWSR_status_is_not(TW_MR_SLA_ACK)){
		/* Issue a TWI stop command to terminate transaction. */
		TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTO);
		
		/*Terminate on error and return to caller*/
		return TWI_BUS_ERROR;
	}

	/* Receive RxSize number of bytes and place them into buffer pointed to by pRxBuffer */
	for (i = 0; i< package->length ; i++)
	{
		
		if(i < ((package->length)-1)){
			
			TWCR = (1 << TWEN) | (1 << TWINT)| (1 << TWEA);
            // timeout if the system doesn't respond quickly. 
            attemptsCtr = 1000;
        	do {
            	attemptsCtr--;
            } while (bit_is_clear(TWCR, TWINT) && attemptsCtr);
	
            if (attemptsCtr == 0) {
        	    return ATCA_RX_TIMEOUT;
            }
			
			if (TWSR_status_is_not(TW_MR_DATA_ACK))
			return TWI_BUS_ERROR;
		}
		else{
			
			TWCR &= ~(1 <<TWEA);
			TWCR = (1 << TWEN) | (1 << TWINT);
			
            // timeout if the system doesn't respond quickly. 
            attemptsCtr = 1000;
        	do {
            	attemptsCtr--;
            } while (bit_is_clear(TWCR, TWINT) && attemptsCtr);
	
            if (attemptsCtr == 0) {
        	    return ATCA_RX_TIMEOUT;
            }
			
			if (TWSR_status_is_not(TW_MR_DATA_NACK))
			return TWI_BUS_ERROR;
		}
		*twim_rx_data++ = TWDR;
	}
	

	/* Issue a TWI stop command to terminate transaction. */
	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTO);

	return ATCA_SUCCESS;
}


/** \brief HAL implementation of I2C Master Read function
 * \param[in] iface     instance
 * \param[in] rxdata    pointer to space to receive the data
 * \param[in] rxlength  ptr to expected number of receive bytes to request
 * \return ATCA_STATUS
 */
ATCA_STATUS i2c_master_read( ATCAIface iface, uint8_t *rxdata, uint16_t *rxlength)
{
	ATCAIfaceCfg *cfg = atgetifacecfg(iface);
	ATCA_STATUS ret = ATCA_UNIMPLEMENTED;
	twi_package_t package;
	
	int retries = cfg->rx_retries;

	/*! TWI chip address to communicate with*/
	package.chip = cfg->atcai2c.slave_address;
	/*not used for 508A*/
	package.addr = 0;
	/*not used for 508A*/
	package.addr_length = 0;
	/*Where to find the data to be written*/
	package.buffer = rxdata;
	/*How many bytes do we want to write*/
	package.length = *rxlength;
	/*! This flag tells the low level drive
	  to check for ack only*/
	package.chk_ack_only_flag = false;
	
	while ((retries-- > 0 ) && (ret != ATCA_SUCCESS))
	{
		ret = twi_master_read(&package);
	}	
	
	return ret;
	
}



/** \brief method to change the bus speed of I2C
 * \param[in] iface  interface on which to change bus speed
 * \param[in] speed  baud rate (typically 100000 or 400000)
 */
void change_i2c_speed( ATCAIface iface, uint32_t speed )
{
	/*Disable TWI transceiver*/
	TWCR &= ~(1 << TWEN);   

 switch(speed)
 {
	case SCLFREQ100KHZ:
		TWBR	=  0x12;		// Define the baud rate using formula
		TWSR	|= (1<<TWPS0);
	break;
	
	default:
		TWBR	=  0x03;		// Define the baud rate using formula
		TWSR	|= (1<<TWPS0);
	break;

	}


	/*Enable TWI transceiver*/
	TWCR	= (1 << TWEN);   

}



/***** FUNCTIONS BELOW ARE OPTIONAL DEPENDING ON FUNCTIONALITIES USED ****/

/** \brief manages reference count on given bus and releases resource if no more references exist
 * \param[in] hal_data - opaque pointer to hal data structure - known only to the HAL implementation
 */
ATCA_STATUS hal_i2c_release( void *hal_data )
{
	/* implement your code here */
}

ATCA_STATUS hal_i2c_discover_buses(uint16_t i2c_buses[], uint16_t max_buses)
{
	return ATCA_UNIMPLEMENTED;
}

ATCA_STATUS hal_i2c_discover_devices(uint16_t busNum, ATCAIfaceCfg cfg[], uint16_t *found )
{

	return ATCA_UNIMPLEMENTED;
}

 /* brief return software generated 32 byte random number
  * param[out] random_number - ptr to space to receive the random number
 */
ATCA_STATUS hal_random_number(uint8_t* random_number)
{
  /* below is a psuedo-random number generator. Note: this generator does not meet the NIST requirements
     for a random number generator. However, it is sufficient for the purposes of this general purpose 
	 security evaluation platform. For additional entroy in the random number generator, an alternate
	 algorithm is recommended. */
  uint8_t random_array[32];
  uint8_t rval;
                 // UNDONE
                uint8_t challenge[32]= {
                                0x3D, 0x1A, 0x85, 0x25, 0x37, 0xB5, 0xC3, 0x2C,  0xF3, 0x6E, 0x3B, 0x13, 0x20, 0x4A, 0xC4, 0xEF,
                0x1F, 0x9E, 0xBE, 0xFB, 0xf5, 0x9B, 0x98, 0x59,  0x38, 0x23, 0xC6, 0x4A, 0xE1, 0x04, 0x8A, 0x66};
                
                memcpy(random_number, challenge, ATCA_KEY_SIZE);
                
                return ATCA_SUCCESS;

   
  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
//  randomSeed(analogRead(0));

//  for (int i; i < 32; i++) {
//	  rval = random(0x00, 0x100);  // min value is inclusive, max value is exclusive.
//	  random_array[i] = rval;
//  }
     
//  memcpy(random_number, random_array, 32); // copy 32 byts from random_array to the random_number variable.
}
