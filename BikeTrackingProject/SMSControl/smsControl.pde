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

int led1 = 9;
int led2 = 10;
int led3 = 11;
int led4 = 12;

void setup()
{
  // prepare the digital output pins
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
  //Initialize GSM module serial port for communication.
  cell.begin(9600);
  delay(30000); // give time for GSM module to register on network etc.
  cell.println("AT+CMGF=1"); // set SMS mode to text
  delay(200);
  cell.println("AT+CNMI=3,3,0,0"); // set module to send SMS data to serial out upon receipt 
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
      if (inchar=='a')
      {
        delay(10);
        inchar=cell.read();
        if (inchar=='0')
        {
          digitalWrite(led1, LOW);
        } 
        else if (inchar=='1')
        {
          digitalWrite(led1, HIGH);
        }
        delay(10);
        inchar=cell.read();  
        if (inchar=='b')
        {
            inchar=cell.read();
          if (inchar=='0')
          {
            digitalWrite(led2, LOW);
          } 
          else if (inchar=='1')
          {
            digitalWrite(led2, HIGH);
          }
          delay(10);
          inchar=cell.read();  
          if (inchar=='c')
          {
              inchar=cell.read();
            if (inchar=='0')
            {
              digitalWrite(led3, LOW);
            } 
            else if (inchar=='1')
            {
              digitalWrite(led3, HIGH);
            }
            delay(10);
            inchar=cell.read();  
            if (inchar=='d')
            {
              delay(10);
              inchar=cell.read();
              if (inchar=='0')
              {
                digitalWrite(led4, LOW);
              } 
              else if (inchar=='1')
              {
                digitalWrite(led4, HIGH);
              }
              delay(10);
            }
          }
          cell.println("AT+CMGD=1,4"); // delete all SMS
        }
      }
    }
  }
}






