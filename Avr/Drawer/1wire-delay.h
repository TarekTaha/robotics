/*
 * Delay routines for the 1 wire bus
 * Search routine is copied from the Dallas owpd library with mods.
 *
 * $Id: 1wire-delay.h,v 1.2 2004/07/12 14:29:00 darius Exp $
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

/*
 * Note that all these routines are clock speed sensitive!
 * (4Mhz AT908515)
 */

//#if (XTAL_CPU == 4000000)
/* 6 usec */
#define DELAY_A	asm volatile(	\
	"ldi r21, 6\n\t"	\
	"L_%=: nop\n\t"		\
	"dec r21\n\t"		\
	"brne L_%=\n\t"		\
	::: "r21")

/* 64 usec */
#define DELAY_B asm volatile(	\
	"ldi r21, 32\n\t"	\
	"L_%=: nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"dec r21\n\t"		\
	"brne L_%=\n\t"		\
	::: "r21")

/* 60 usec */
#define DELAY_C asm volatile(	\
	"ldi r21, 30\n\t"	\
	"L_%=: nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"dec r21\n\t"		\
	"brne L_%=\n\t"		\
	::: "r21")

/* 10 usec */
#define DELAY_D asm volatile(	\
	"ldi r21, 9\n\t"	\
	"L_%=: nop\n\t"		\
	"dec r21\n\t"		\
	"brne L_%=\n\t"		\
	::: "r21")

/* 9 usec */
#define DELAY_E asm volatile(	\
	"ldi r21, 8\n\t"	\
	"L_%=: nop\n\t"		\
	"dec r21\n\t"		\
	"brne L_%=\n\t"		\
	::: "r21")

/* 55 usec */
#define DELAY_F asm volatile(	\
	"ldi r21, 27\n\t"	\
	"L_%=: nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"dec r21\n\t"		\
	"brne L_%=\n\t"		\
	::: "r21")

/* 0 usec */    
#define DELAY_G

/* 480 usec */
#define DELAY_H asm volatile(	\
	"ldi r21, 120\n\t"	\
	"L_%=: nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"dec r21\n\t"		\
	"brne L_%=\n\t"		\
	::: "r21")

/* 70 usec */    
#define DELAY_I asm volatile(	\
	"ldi r21, 35\n\t"	\
	"L_%=: nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"nop\n\t"		\
	"dec r21\n\t"		\
	"brne L_%=\n\t"		\
	::: "r21")
//#else
//#error No 1 wire delay routines for selected clock speed
//#endif

