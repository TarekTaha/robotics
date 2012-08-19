/*
  Example 27.2
 Control four digital outputs via SMS
 tronixstuff.com/tutorials > chapter 27
 CC by-sa-nc 
 NOT for Arduino Mega
 */

#include <NewSoftSerial.h>  //Include the NewSoftSerial library to send serial commands to the cellular module.
char inchar;                //Will hold the incoming character from the Serial Port.
NewSoftSerial cell(2,3);    //Create a 'fake' serial port. Pin 2 is the Rx pin, pin 3 is the Tx pin.

int relayLine = 8;

void setup()
{
  // prepare the digital output pins
  pinMode(relayLine, OUTPUT);
  digitalWrite(relayLine, LOW);
  //Initialize GSM module serial port for communication.
  cell.begin(9600);
  delay(30000); // give time for GSM module to register on network etc.
  cell.println("AT+CMGF=1"); // set SMS mode to text
  delay(200);
  // set module to send SMS data to serial out upon receipt 
  cell.println("AT+CNMI=3,3,0,0"); 
  delay(200);
}

void loop() 
{
  //If a character comes in from the cellular module...
  if(cell.available() >0)
  {
    inchar=cell.read();    
    if (inchar=='#')
    {
      delay(10);
      inchar=cell.read();  
      if (inchar=='m')
      {
        delay(10);
        inchar=cell.read();
        if (inchar=='0')
        {
          digitalWrite(relayLine, LOW);
        } 
        else if (inchar=='1')
        {
          digitalWrite(relayLine, HIGH);
        }
        delay(10);
        cell.println("AT+CMGD=1,4"); // delete all SMS
      }
    }
  }
}







