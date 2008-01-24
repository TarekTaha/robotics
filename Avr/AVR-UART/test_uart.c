/*************************************************************************
Title:    example program for the Interrupt controlled UART library
Author:   Peter Fleury <pfleury@gmx.ch>   http://jump.to/fleury
File:     $Id: test_uart.c,v 1.4 2005/07/10 11:46:30 Peter Exp $
Software: AVR-GCC 3.3
Hardware: any AVR with built-in UART, tested on AT90S8515 at 4 Mhz

DESCRIPTION:
          This example shows how to use the UART library uart.c

*************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "lcd.h"
#include "1wire.h"

/* define CPU frequency in Mhz here if not defined in Makefile */
#define F_CPU 16000000UL

/* 9600 baud */
#define UART_BAUD_RATE      4800      

// enable PB1 as output
#define LED_INIT DDRB|=_BV(PB2)
// led on, pin=0 
#define LED_ON PORTB&=~_BV(PB2)
// led off, pin=1 
#define LED_OFF PORTB|=_BV(PB2)
// 
#define LED_IS_OFF (PORTB & (1<<PB2))

//#include "delay_x.h"
//#include <util/delay.h>

/************************************************** **************
* Delay and timing stuff....
*
* Functions available:
* DelayUs(x) Delay specified number of microseconds
* DelayMs(x) Delay specified number of milliseconds
************************************************** **************/


int main(void)
{
    unsigned int c;
    int  num=0;

    /*
     *  Initialize UART library, pass baudrate and AVR cpu clock
     *  with the macro 
     *  UART_BAUD_SELECT() (normal speed mode )
     *  or 
     *  UART_BAUD_SELECT_DOUBLE_SPEED() ( double speed mode)
     */
    //uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
    
    /*
     * now enable interrupt, since UART library is interrupt controlled
     */
    //sei();
    
    /*
     *  Transmit string to UART
     *  The string is buffered by the uart library in a circular buffer
     *  and one character at a time is transmitted to the UART using interrupts.
     *  uart_puts() blocks if it can not write the whole string to the circular 
     *  buffer
     */
    //uart_puts("String stored in SRAM\n");
    
    /*
     * Transmit string from program memory to UART
     */
    //uart_puts_P("String stored in FLASH\n");
    /*
     * Transmit single character to UART
     */
    //uart_putc('\r');

    /* initialize display, cursor off */
    lcd_init(LCD_DISP_ON);
    /* clear display and home cursor */
    lcd_clrscr();

    LED_INIT;
    LED_OFF;
	unsigned char Buffer_RX[32];
	int i;
    for(;;)
    {
        /*
         * Get received character from ringbuffer
         * uart_getc() returns in the lower byte the received character and 
         * in the higher byte (bitmask) the last receive error
         * UART_NO_DATA is returned when no data is available.
         *
         */
	//uart_puts_P("Kiss my ass\n");
        //c = uart_getc();
	c=0;
	//uart_puts("TAREK  TAHA ===>>> Wireless is Up and Running\n");
	lcd_clrscr();
	lcd_gotoxy(16-(num++)%16,0);
	lcd_puts("<<LCD  TEST 1>>\n");
	
	if (OWTouchReset() == 1)
	{
		lcd_puts("No Pulse found\n");
	}
	else
	{
		lcd_puts("Pulse found\n");
		OWWriteByte(OW_READ_ROM_CMD);
	  	for (i=0;i<=8;i++)
		{
	  		Buffer_RX[i] = OWReadByte();
			lcd_putc(Buffer_RX[i]);
		}
	}
	DelayMs(100);
	continue;

        if ( c & UART_NO_DATA )
        {
            /* 
             * no data available from UART 
             */
        }
        else
        {
            /*
             * new data available from UART
             * check for Frame or Overrun error
             */
            if ( c & UART_FRAME_ERROR )
            {
                /* Framing Error detected, i.e no stop bit detected */
                uart_puts_P("UART Frame Error: ");
            }
            if ( c & UART_OVERRUN_ERROR )
            {
                /* 
                 * Overrun, a character already present in the UART UDR register was 
                 * not read by the interrupt handler before the next character arrived,
                 * one or more received characters have been dropped
                 */
                uart_puts_P("UART Overrun Error: ");
            }
            if ( c & UART_BUFFER_OVERFLOW )
            {
                /* 
                 * We are not reading the receive buffer fast enough,
                 * one or more received character have been dropped 
                 */
                uart_puts_P("Buffer overflow error: ");
            }
            /* 
             * send received character back
             */
	   if(c=='0')
	   {
		LED_OFF;
		uart_puts_P("--->>>Light OFF\n");
	   }
	   else
	   if(c=='1')
	   {
		LED_ON;
		uart_puts_P("--->>>Light ON\n");
	   }
   	   uart_puts_P("Press 0 or 1 to toggle Light\n");
           //uart_putc( (unsigned char)c );
        }
	//delay_ms(100);
	uart_puts("TAREK  TAHA");
    }
    
}
