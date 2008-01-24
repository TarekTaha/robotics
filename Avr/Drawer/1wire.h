/*
 * 1 wire header code
 *
 * $Id: 1wire.h,v 1.4 2004/09/01 08:15:24 darius Exp $
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
#include <util/crc16.h>

#define EEPROM_SIZE 12

unsigned char Buffer_RX[EEPROM_SIZE];
unsigned char ROM[8];

uint8_t		OWTouchReset(void);
void		OWWriteBit(uint8_t bit);
uint8_t		OWReadBit(void);
void		OWWriteByte(uint8_t data);
uint8_t		OWReadByte(void);
uint8_t		OWTouchByte(uint8_t data);
void		OWBlock(uint8_t *data, int len);
uint8_t		OWFirst(uint8_t *ROM, uint8_t do_reset, uint8_t alarm_only);
uint8_t		OWNext(uint8_t *ROM, uint8_t do_reset, uint8_t alarm_only);
void		OWCRC(uint8_t x, uint8_t *crc);
void		OWSendCmd(uint8_t *ROM, uint8_t cmd);

#define OWIREOUTPORT	PORTC
#define OWIREOUTPIN		3

#define OWIREINPORT		PINC
#define OWIREDDR		DDRC
#define OWIREINPIN		OWIREOUTPIN

/* Return codes for OWFirst()/OWNext() */
#define OW_BADWIRE		-3
#define OW_BADCRC		-2
#define OW_NOPRESENCE	-1
#define OW_NOMODULES	0
#define OW_FOUND		1

/* General 1 wire commands */
#define OW_OVRDRV_SKIP_CMD	0x3c
#define OW_SEARCH_ALRM_CMD	0xec
#define OW_SEARCH_ROM_CMD	0xf0
#define OW_READ_MEMORY_CMD	0xf0
#define OW_WRITE_SCRATCHPAD	0x0f
#define OW_READ_SCRATCHPAD	0xaa
#define OW_COPY_SCRATCHPAD	0x55
#define OW_READ_ROM_CMD		0x33
#define OW_MATCH_ROM_CMD	0x55
#define OW_SKIP_ROM_CMD		0xcc

/* DS1820 commands */
#define OW_CONVERTT_CMD		0x44
#define OW_RD_SCR_CMD		0xbe
#define OW_WR_SCR_CMD		0x4e
#define OW_CPY_SCR_CMD		0x48
#define OW_RECALL_CMD		0xb8
#define OW_RD_PSU_CMD		0xb4

/* Family codes */
#define OW_FAMILY_ROM		0x09
#define OW_FAMILY_TEMP		0x10
