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
#include "delay_x.h"
//#include <util/delay.h>

#define DELAY_A	DelayUs(6)

/* 64 usec */
#define DELAY_B DelayUs(64)

/* 60 usec */
#define DELAY_C DelayUs(60)

/* 10 usec */
#define DELAY_D DelayUs(10)

/* 9 usec */
#define DELAY_E DelayUs(9)

/* 55 usec */
#define DELAY_F DelayUs(55)
/* 0 usec */    
#define DELAY_G DelayUs(0)

/* 480 usec */
#define DELAY_H DelayUs(480)

/* 70 usec */    
#define DELAY_I DelayUs(70)

