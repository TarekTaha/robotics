#include <stdlib.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#ifndef F_CPU
	#define F_CPU 3686400
#endif

#include <util/delay.h>
#include "lcd.h"
#include "1wire.h"
#include "uart.h"

#define MAIN_MENU     1
#define ADMIN_MENU    2
#define ACCESS_MENU   3
#define ADD_USER_MENU 4

/* 9600 baud */
#define UART_BAUD_RATE      9600      

/*
Assume matrix is

        cl3   cl2   cl1   cl0
PG0->   '7'   '8'   '9'   '>'   row 0
PG1->   '4'   '5'   '6'   '<'   row 1
PG2->   '1'   '2'   '3'   ' '   row 2
PG3->   'c'   '0'   'e'   '.'   row 3
          V    V     V     V
         PA3  PA2   PA1   PA0
*/

const uint8_t keys[] = { "c1470258e369. <>" };
static volatile uint8_t key,key_pressed,menu;

int getKey()
{
   uint8_t col, row ;
   for (row=0; row<4; row++) 
   {
	  PORTG = row;
	  PORTA |= 0x00;
      if ((PINA & 0x0F) != 0x0F) 
	  {
         if ((PINA & 0x08) == 0) 
		 {
            col = 3;
         }
         else if ((PINA & 0x04) == 0) 
		 {
            col = 2;
         }
         else if ((PINA & 0x02) == 0) 
		 {
            col = 1;
         }
         else if ((PINA & 0x01) == 0) 
		 {
            col = 0;
         }
		 if ((key !=keys[row*4 + col]) || (!key_pressed))
		 {
         	key = keys[row*4 + col];
		 	key_pressed =1;
		 	return key_pressed;
		 }
		 else
		 	return 0;
      }
   }
    key_pressed = 0;
	return 0;
}
 
void Delay(unsigned int millisec)
{
	// mt, int i did not work in the simulator:  int i; 
	uint8_t i;

	while (millisec--) {
		for (i=0; i<125; i++) {
			asm volatile ("nop"::);
		}
	}
}

void MainMenu()
{
    /* initialize display, cursor off */
    lcd_init(LCD_DISP_ON);
    /* clear display and home cursor */
    lcd_clrscr();       
    /* put string to display (line 1) with linefeed */
	lcd_gotoxy(0,0);
    lcd_puts(" <Drawer Security> ");

	lcd_gotoxy(0,1);
    lcd_puts("1- Access Drawer> ");

	lcd_gotoxy(0,2);
    lcd_puts("2- Admin Access");

}
void AccessMenu()
{
    lcd_clrscr();       
    /* put string to display (line 1) with linefeed */
	lcd_gotoxy(0,0);
    lcd_puts("   <-ACESS MENU->");

	lcd_gotoxy(0,1);
    lcd_puts("Your Pin: ");

	lcd_gotoxy(0,2);
	lcd_puts("---> Press  ENT <---");
}
void AddUserMenu()
{
    lcd_clrscr();       
    /* put string to display (line 1) with linefeed */
	lcd_gotoxy(0,0);
    lcd_puts("Enter Pin: ");

	lcd_gotoxy(0,1);
	lcd_puts("Confirm P: ");
}

int main(void)
{
    unsigned int c;
    char buffer[7];
    int  num=134;

    char pinCode[8];
	pinCode[0]='\n';
    int  pin_chars=0;
	int line,pos;
    unsigned char i;
   
	DDRA  = 0x00;  // PortA inputs
	DDRG  = 0xFF;  // PortG outputs 
    PORTA = 0x0F;  // enable the pull ups on the 3..0 inputs 

	MainMenu();
	menu = MAIN_MENU;
	i=0;
    uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
    sei();                       // enable interrupts 
    uart_puts("String stored in SRAM\n");
    
    /*
     * Transmit string from program memory to UART
     */
    uart_puts_P("String stored in FLASH\n");
    
        
    /* 
     * Use standard avr-libc functions to convert numbers into string
     * before transmitting via UART
     */     
    itoa( num, buffer, 10);   // convert interger into string (decimal format)         
    uart_puts(buffer);        // and transmit string to UART

//	lcd_gotoxy(0,3);
//	lcd_puts("TESTING EEPROM");	

//	DDRC = 0xfc;
//	PORTC= 0x00;
//	test_eeprom_write();
//	test();
//	unsigned char pin[8]={'1','2','3','4','5','6','7','8'};
//	add_pin(pin);
//	pin[5]='A';
//	add_pin(pin);
//	pin[5]='B';
//	add_pin(pin);
//	pin[5]='C';
//	add_pin(pin);
//	print_pins();
	MainMenu();
    for (;;) 
	{
	uart_puts_P("UART Frame Error: ");
/*		lcd_gotoxy(0,3);
		if (OWTouchReset() == 1)
		{
			//OWWriteEEPROM(0x00, 0x00);//Write byte 0xAA @  EEPROM ADDRESS 0x00;
			//OWWriteEEPROM(0x01, 'T'); //Write byte 0xBB @  EEPROM ADDRESS 0x01;
			//OWWriteEEPROM(0x02, 'a'); //Write byte 0xCC @  EEPROM ADDRESS 0x02;  
			//OWWriteEEPROM(0x03, 'R'); //Write byte 0xCC @  EEPROM ADDRESS 0x03;  
			//OWWriteEEPROM(0x04, 'e'); //Write byte 0xCC @  EEPROM ADDRESS 0x04; 
			//OWWriteEEPROM(0x05, 'K'); //Write byte 0xCC @  EEPROM ADDRESS 0x05; 
			//OWReadRom();
			OWReadAllEEPROM();
			lcd_putc(Buffer_RX[1]); lcd_putc(Buffer_RX[2]); lcd_putc(Buffer_RX[3]); lcd_putc(Buffer_RX[4]); lcd_putc(Buffer_RX[5]);
			//lcd_puts(ROM);
			//lcd_puts("YES");
		}
		else
			lcd_puts("NOT THERE ");
*/
//	lcd_gotoxy(0,3);
//	test_eeprom_write();
		if(getKey())
		{
			switch (menu)
			{
				case MAIN_MENU:
					if(key == '1')
					{
						AccessMenu();
						menu = ACCESS_MENU;
					}
					else 
					if(key =='2')
					{
						AddUserMenu();
						menu = ADD_USER_MENU;
					}
					break;
				case ACCESS_MENU:
					if(pin_chars < 8)
					{
						line = 1;
						pos  = 9 + pin_chars;
						pinCode[pin_chars++] = key;
					}
					else
					{
						if (1)//if(check_pin(pinCode))
						{
							lcd_gotoxy(0,0);
							lcd_puts("Access Granted");
						}
						else
						{
							lcd_gotoxy(0,0);
							lcd_puts("Access Denied");							
						}
					}
					lcd_gotoxy(pos,line);
					lcd_putc(key);
					Delay(200);
					lcd_gotoxy(pos,line);
					lcd_putc('*');
					break;
				case ADD_USER_MENU:
					break;
			}
		}
		Delay(100);
		PORTG = 5;
		lcd_gotoxy(0,3);
		if ((PINA & 0x01) == 0) 
		{
        	lcd_puts("A0:0 ");
        }
		else
		{
			lcd_puts("A0:1 ");
		}
		if ((PINA & 0x02) == 0) 
		{
        	lcd_puts("A1:0 ");
        }
		else
		{
			lcd_puts("A1:1 ");
		}
		if ((PINA & 0x04) == 0) 
		{
        	lcd_puts("A2:0 ");
        }
		else
		{
			lcd_puts("A2:1 ");
		}
		if ((PINA & 0x08) == 0) 
		{
        	lcd_puts("A3:0 ");
        }
		else
		{
			lcd_puts("A3:1 ");
		}
        //itoa( num , buffer, 10);
        // put converted string to display 
        //lcd_puts(buffer);                 
    }
}

