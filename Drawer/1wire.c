/*
 * Various 1 wire routines
 * Search routine is copied from the Dallas owpd library with mods
 * available from here http://www.ibutton.com/software/1wire/wirekit.html
 *
 * $Id: 1wire.c,v 1.4 2004/09/01 08:15:24 darius Exp $
 *
 * Copyright (c) 2004
 *      Daniel O'Connor <darius@dons.net.au>.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "1wire.h"
#include "1wire-delay.h"

#if OW_DEBUG
	void		uart_putsP(const char *addr);
	void		uart_puts(const char *addr);
	void		uart_getc();
	int			uart_putc(char c);
#endif

static uint8_t OW_LastDevice = 0;
static uint8_t OW_LastDiscrepancy = 0;
static uint8_t OW_LastFamilyDiscrepancy = 0;

static void OWdelay(void) 
{
    DELAY_I;
}

/*-----------------------------------------------------------------------------
 * Generate a 1-Wire reset, return 0 if presence pulse was found,
 * return 1 otherwise.
 * (NOTE: Does not handle alarm presence from DS2404/DS1994)
 */
uint8_t
OWTouchReset(void) 
{
    DELAY_G;
    OWIREOUTPORT &= ~(_BV(OWIREOUTPIN));
    OWIREDDR |= _BV(OWIREOUTPIN);
    DELAY_H;
    OWIREDDR &= ~(_BV(OWIREOUTPIN));
    DELAY_I;

    return(OWIREINPORT & _BV(OWIREINPIN) ? 0 : 1);
}

/*-----------------------------------------------------------------------------
 * Send a 1-wire write bit.
 */
void OWWriteBit(uint8_t bit) 
{
	OWdelay();
    if (bit) 
	{
		OWIREOUTPORT &= ~(_BV(OWIREOUTPIN));
		OWIREDDR |= _BV(OWIREOUTPIN);
		DELAY_A;
		OWIREDDR &= ~(_BV(OWIREOUTPIN));
		DELAY_B;
    } 
	else 
	{
		OWIREOUTPORT &= ~(_BV(OWIREOUTPIN));
		OWIREDDR |= _BV(OWIREOUTPIN);
		DELAY_C;
		OWIREDDR &= ~(_BV(OWIREOUTPIN));
		DELAY_D;
    }
}

/*-----------------------------------------------------------------------------
 * Read a bit from the 1-wire bus and return it.
 */
uint8_t OWReadBit(void) 
{
    OWdelay();
    OWIREOUTPORT &= ~(_BV(OWIREOUTPIN));
    OWIREDDR |= _BV(OWIREOUTPIN);
    DELAY_A;
    OWIREDDR &= ~(_BV(OWIREOUTPIN));
    DELAY_E;
    return(OWIREINPORT & _BV(OWIREINPIN) ? 1 : 0);
}

/*-----------------------------------------------------------------------------
 * Write a byte to the 1-wire bus
 */
void OWWriteByte(uint8_t data) 
{
    uint8_t i;    
    /* Send LSB first */
    for (i = 0; i < 8; i++) 
	{
		OWWriteBit(data & 0x01);
		data >>= 1;
    }
}

/*-----------------------------------------------------------------------------
 * Read a byte from the 1-wire bus
 */
uint8_t OWReadByte(void) 
{
    int i, result = 0;    
    for (i = 0; i < 8; i++) 
	{
		result >>= 1;
		if (OWReadBit())
	    	result |= 0x80;
    }
    return(result);
}

/*-----------------------------------------------------------------------------
 * Write a 1-wire data byte and return the sampled result.
 */
uint8_t OWTouchByte(uint8_t data) 
{
    uint8_t i, result = 0;    
    for (i = 0; i < 8; i++) 
	{
		result >>= 1;
		/* If sending a 1 then read a bit, otherwise write a 0 */
		if (data & 0x01) 
		{
	    	if (OWReadBit())
				result |= 0x80;
		} 
		else
	    	OWWriteBit(0);
		data >>= 1;
    }
    return(result);
}

/*-----------------------------------------------------------------------------
 * Write a block of bytes to the 1-wire bus and return the sampled result in
 * the same buffer
 */
void OWBlock(uint8_t *data, int len) 
{
    int i;    
    for (i = 0; i < len; i++)
		data[i] = OWTouchByte(data[i]);
}


/*-----------------------------------------------------------------------------
 * Send a 1 wire command to a device, or all if no ROM ID provided
 */
void OWSendCmd(uint8_t *ROM, uint8_t cmd) 
{
    uint8_t	i;
    OWTouchReset();
    if (ROM == NULL)
		OWWriteByte(OW_SKIP_ROM_CMD);
    else 
	{
		OWWriteByte(OW_MATCH_ROM_CMD);
		for (i = 0; i < 8; i++)
	    	OWWriteByte(ROM[i]);
    }
    OWWriteByte(cmd);
}

/*-----------------------------------------------------------------------------
 * Search algorithm from App note 187 (and 162)
 *
 * OWFirst/OWNext return..
 *         1 when something is found,
 *         0 no more modules
 *	   -1 if no presense pulse,
 *         -2 if bad CRC,
 *         -3 if bad wiring.
 */
uint8_t OWFirst(uint8_t *ROM, uint8_t do_reset, uint8_t alarm_only) 
{
    /* Reset state */
    OW_LastDiscrepancy = 0;
    OW_LastDevice = 0;
    OW_LastFamilyDiscrepancy = 0;    
    /* Go looking */
    return (OWNext(ROM, do_reset, alarm_only));
}

/* Returns 1 when something is found, 0 if nothing left */
uint8_t OWNext(uint8_t *ROM, uint8_t do_reset, uint8_t alarm_only) 
{
    uint8_t	bit_test, search_direction, bit_number;
    uint8_t	last_zero, rom_byte_number, rom_byte_mask;
    uint8_t	lastcrc8, crcaccum;
    int8_t	next_result;

    /* Init for search */
   	bit_number = 1;
   	last_zero = 0;
   	rom_byte_number = 0;
   	rom_byte_mask = 1;
   	next_result = OW_NOMODULES;
   	lastcrc8 = 0;
   	crcaccum = 0;

   /* if the last call was not the last one */
   	if (!OW_LastDevice) 
	{
       /* check if reset first is requested */
   	if (do_reset) 
	{
	   /* reset the 1-wire
	    * if there are no parts on 1-wire, return 0 */
		#if OW_DEBUG
	   		uart_putsP(PSTR("Resetting\n\r"));
		#endif
        if (OWTouchReset()) 
		{
	     	/* reset the search */
	     	OW_LastDiscrepancy = 0;
	     	OW_LastFamilyDiscrepancy = 0;
			#if OW_DEBUG
	     		uart_putsP(PSTR("No devices on bus\n\r"));
			#endif
	     	return OW_NOPRESENCE;
        }
	}

	/* If finding alarming devices issue a different command */
    if (alarm_only)
		OWWriteByte(OW_SEARCH_ALRM_CMD);	/* issue the alarming search command */
	else
		OWWriteByte(OW_SEARCH_ROM_CMD);	/* issue the search command */

    /* pause before beginning the search */
    OWdelay();
    OWdelay();
    OWdelay();
      
    /* loop to do the search */
    do 
	{
		/* read a bit and its compliment */
	  	bit_test = OWReadBit() << 1;
	  	bit_test |= OWReadBit();

		#if OW_DEBUG
	  		sprintf_P(errstr, PSTR("bit_test = %d\n\r"), bit_test);
	  		uart_puts(errstr);
		#endif

	  	/* check for no devices on 1-wire */
	  	if (bit_test == 3) 
		{
			#if OW_DEBUG
	      		sprintf_P(errstr, PSTR("bit_test = %d\n\r"), bit_test);
	      		uart_puts(errstr);
			#endif
	      	return(OW_BADWIRE);
	  	}
	  	else 
		{
	    	/* all devices coupled have 0 or 1 */
	      	if (bit_test > 0)
		  		search_direction = !(bit_test & 0x01);  /* bit write value for search */
	      	else 
			{
		  		/* if this discrepancy is before the Last Discrepancy
		   		* on a previous OWNext then pick the same as last time */
		  		if (bit_number < OW_LastDiscrepancy)
		      		search_direction = ((ROM[rom_byte_number] & rom_byte_mask) > 0);
		  		else
		      		/* if equal to last pick 1, if not then pick 0 */
			 		search_direction = (bit_number == OW_LastDiscrepancy);
		  			/* if 0 was picked then record its position in LastZero */
		  		if (search_direction == 0) 
				{
		      		last_zero = bit_number;
		      		/* check for Last discrepancy in family */
		      		if (last_zero < 9)
			  			OW_LastFamilyDiscrepancy = last_zero;
		  		}
	      	}

	      	/* set or clear the bit in the ROM byte rom_byte_number
	       	* with mask rom_byte_mask */
	      	if (search_direction == 1)
		  		ROM[rom_byte_number] |= rom_byte_mask;
	      	else
		  		ROM[rom_byte_number] &= ~rom_byte_mask;

	      	/* serial number search direction write bit */
	      	OWWriteBit(search_direction);

	      	/* increment the byte counter bit_number
	      	 * and shift the mask rom_byte_mask */
	      	bit_number++;
	      	rom_byte_mask <<= 1;

	      	/* if the mask is 0 then go to new ROM byte rom_byte_number
	       	* and reset mask */
	      	if (rom_byte_mask == 0) 
			{
		  		OWCRC(ROM[rom_byte_number], &crcaccum);  /* accumulate the CRC */
		  		lastcrc8 = crcaccum;
		  
		  		rom_byte_number++;
		  		rom_byte_mask = 1;
	      	}
	  	}
      } 
	  while (rom_byte_number < 8);  /* loop until through all ROM bytes 0-7 */

      /* if the search was successful then */
	if (!(bit_number < 65) || lastcrc8) 
	{
		if (lastcrc8) 
		{
			#if OW_DEBUG
	      		sprintf_P(errstr, PSTR("Bad CRC (%d)\n\r"), lastcrc8);
	      		uart_puts(errstr);
			#endif
	      	next_result = OW_BADCRC;
	  	} 
		else 
		{
	    	/*  search successful so set LastDiscrepancy,LastDevice,next_result */
	      	OW_LastDiscrepancy = last_zero;
	      	OW_LastDevice = (OW_LastDiscrepancy == 0);
			#if OW_DEBUG
	      		sprintf_P(errstr, PSTR("Last device = %d\n\r"), OW_LastDevice);
	      		uart_puts(errstr);
			#endif
	      	next_result = OW_FOUND;
	  	}
	}
}

   /* if no device found then reset counters so next 'next' will be
    * like a first */
	if (next_result != OW_FOUND || ROM[0] == 0) 
   	{
    	OW_LastDiscrepancy = 0;
       	OW_LastDevice = 0;
       	OW_LastFamilyDiscrepancy = 0;
   	}

   if (next_result == OW_FOUND && ROM[0] == 0x00)
       next_result = OW_BADWIRE;
   return next_result;
}

uint8_t PROGMEM dscrc_table[] = { 
    0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
    157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
    35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
    190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
    70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
    219, 133,103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
    101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
    248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
    140,210, 48, 110, 237, 179, 81, 15, 78, 16, 242,  172, 47, 113,147, 205,
    17, 79, 173, 243, 112, 46, 204, 146, 211,141, 111, 49, 178, 236, 14, 80,
    175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82,176, 238,
    50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
    202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
    87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
    233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
    116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};
int checkcrc(uint8_t serno[])
	{
    	uint8_t crc = 0, i;
        for (i = 0; i < sizeof serno / sizeof serno[0]; i++)
            crc = _crc_ibutton_update(crc, serno[i]);
        return crc; // must be 0
    }
void OWCRC(uint8_t x, uint8_t *crc) 
	{
    	*crc = pgm_read_byte(&dscrc_table[(*crc) ^ x]);
	}	

//--------------------------------------------------------------------------
// DESCRIPTION:  		Procedure to read DS1971 ROM												 
// PARAMETERS IN:         
// PARAMETERS OUT:		
// LOCAL VARIABLE:      unsigned char i; 
// 		 				unsigned char data;
// GLOBAL VARIABLE:     Buffer_RX;
// COSTANTS:        
//---------------------------------------------------------------------------
int OWReadRom (void)
{
	unsigned char i;
  	unsigned char chip_presence = 0;
     
  	chip_presence = OWTouchReset();  
  	if (chip_presence == 1)								//Is an iButton Present?
    {
    	OWWriteByte(OW_READ_ROM_CMD);     				//Read Its ROM
	  	for (i=0;i<=8;i++)
	  		ROM[i] = OWReadByte();
		return 1;
	}
  	return 0;
}


//----------------------------------------------------------------------------
// DESCRIPTION:  		Procedure to write, verify one byte to DS1971 EEPROM 											 
// PARAMETERS IN:       unsigned char address;
//                      unsigned char data.
// PARAMETERS OUT:		
// LOCAL VARIABLE:      unsigned char chip_presence;
// GLOBAL VARIABLE:    
// COSTANTS:        
//----------------------------------------------------------------------------
void OWWriteEEPROM (unsigned char address, unsigned char data)
{
  	long wait;
  	unsigned char chip_presence = 0;
  	unsigned char verify_data = 0;
    
  	chip_presence = OWTouchReset();
  	if (chip_presence == 1)
    {
      	OWWriteByte(OW_SKIP_ROM_CMD);  			//Send "Skip ROM" command
      	OWWriteByte(OW_WRITE_SCRATCHPAD);  		//Send "Write Scratchpad" command
      	OWWriteByte(address);					//Send address 
      	OWWriteByte(data);						//Send data 
  	}
  	//for(wait=0;wait<100000;wait++);
	DELAY_A;

  	chip_presence = OWTouchReset();
  	if (chip_presence == 1)
    {
      	OWWriteByte(OW_SKIP_ROM_CMD);  			//Send "Skip ROM" command
      	OWWriteByte(OW_READ_SCRATCHPAD);  		//Send "Read Scratchpad" command
      	OWWriteByte(address);	     			//Send  address 
      	verify_data = OWReadByte(); 			//Read and verify data  
  	}
  	//for(wait=0;wait<100000;wait++);
	DELAY_A;
	  
  	chip_presence = OWTouchReset();
  	if (chip_presence == 1)
    {
      	OWWriteByte(OW_SKIP_ROM_CMD);  			//Send  "Skip ROM" command
      	OWWriteByte(OW_COPY_SCRATCHPAD);		//Send  "Copy Scratchpad" command
      	OWWriteByte(0xA5);						//Send  "Validation Key" 
    }
  	//for(wait=0;wait<100000;wait++);
	DELAY_A;
	  	
	return;
}


//----------------------------------------------------------------------------
// DESCRIPTION:  		Procedure to read DS1971 total EEPROM 											 
// PARAMETERS IN:       
// PARAMETERS OUT:		
// LOCAL VARIABLE:      
// GLOBAL VARIABLE:     Buffer_RX;
// COSTANTS:        
//----------------------------------------------------------------------------
void OWReadAllEEPROM (void)
{
  unsigned char chip_presence = 0;
  unsigned char i = 0; 
   
  for(i=0;i<=EEPROM_SIZE;i++)				
    Buffer_RX[i] = 0;				//Clear all receive buffer

  chip_presence = OWTouchReset();
  if (chip_presence == 1)
    {
	  OWWriteByte(OW_SKIP_ROM_CMD);		//Send "Skip ROM" command	   
      OWWriteByte(OW_READ_MEMORY_CMD);  //Send "Read Memory" command
      OWWriteByte(0x00);				//Send  start address 0x00
	   
	  for (i=0;i<=EEPROM_SIZE;i++)
		Buffer_RX[i] = OWReadByte();  
	}
  return;
}
