/*  
 Example 26.2  GSM shield making and ending a telephone call
 http://tronixstuff.com/tutorials > chapter 26 
 */
#include <NewSoftSerial.h>  //Include the NewSoftSerial library to send serial commands to the cellular module.
NewSoftSerial cell(2,3);  //Create a 'fake' serial port. Pin 2 is the Rx pin, pin 3 is the Tx pin.

void setup()
{  
  //Initialize serial ports for communication.
  cell.begin(9600);
  delay(25000); 
  // give the GSM module time to initialise, locate network etc.
  // this delay time varies. Use example 26.1 sketch to measure the amount
  // of time from board reset to SIND: 4, then add five seconds just in case
}

void loop()
{
  cell.println("ATD0433292432"); // dial the phone number xxxxxxxxx
  // change xxxxxxx to your desired phone number (with area code)
  delay(20000); // wait 20 seconds.
  cell.println("ATH"); // end call
  do // remove this loop at your peril
  {    
    delay(1);  
  }
  while (1>0);
}

